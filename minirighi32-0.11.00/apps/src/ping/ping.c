/*!	\file apps/src/ping/ping.c
 *	\brief Ping.
 *	\author Andrea Righi <righiandr@users.sf.net>
 *	\date Last update: 2004-08-23
 */

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/sys.h>
#include <net/ip.h>
#include <net/icmp.h>
#include <net/network.h>
#include <net/socket.h>

#define	MAX_DUP_CHK	(8 * 128)

#define	A(bit)		rcvd_tbl[(bit)>>3]	/* identify byte in array */
#define	B(bit)		(1 << ((bit) & 0x07))	/* identify bit in byte */
#define	SET(bit)	(A(bit) |= B(bit))
#define	CLR(bit)	(A(bit) &= (~B(bit)))
#define	TST(bit)	(A(bit) & B(bit))

static const int PING_INTERVAL = 100 /* ticks */;

static char rcvd_tbl[MAX_DUP_CHK / 8];

static long ntransmitted, nreceived, nrepeats, pingcount = 3;

/* Calculate the IP checksum */
uint16_t ip_checksum(const void *buf, size_t hdr_len) {
	unsigned long sum = 0;
	const uint16_t *ip1;

	ip1 = buf;
	while (hdr_len > 1) {
		sum += *ip1++;
		if (sum & 0x80000000)
			sum = (sum & 0xFFFF) + (sum >> 16);
		hdr_len -= 2;
	}

	while (sum >> 16)
		sum = (sum & 0xFFFF) + (sum >> 16);

	return(~sum);
}

/*
 * Converts an internet IP addres from number-and-dot string
 * notation into binary data network byte order.
 */
int inet_aton(const char *cp, in_addr_t *inp) {
	static const in_addr_t max[4] = { 0xFFFFFFFF, 0xFFFFFF, 0xFFFF, 0xFF };
	in_addr_t val;
	char c;
	union iaddr {
	  uint8_t bytes[4];
	  uint32_t word;
	} res;
	uint8_t *pp = res.bytes;
	int digit,base;

	res.word = 0;

	c = *cp;
	for (;;) {
		if (!isdigit(c)) goto ret_0;
		val = 0; base = 10; digit = 0;
		for (;;) {
			if (isdigit(c)) {
				val = (val * base) + (c - '0');
				c = *++cp;
				digit = 1;
			} else {
				break;
			}
		}
		if (c == '.') {
			if (pp > res.bytes + 2 || val > 0xff) {
				goto ret_0;
			}
			*pp++ = val;
			c = *++cp;
		} else
			break;
	}

	if (c != '\0' && (!isascii(c) || !isspace(c))) {
		goto ret_0;
	}

	if (!digit) {
		goto ret_0;
	}

	if (val > max[pp - res.bytes]) {
		goto ret_0;
	}

	if (inp != NULL) {
		*inp = res.word | htonl (val);
	}

	return (1);
ret_0:
	return (0);
}

static int pingstats() {
	int status;

	printf("\n--- ping statistics ---\n");
	printf("%ld packets transmitted, ", ntransmitted);
	printf("%ld packets received, ", nreceived);
	if (nrepeats)
		printf("%ld duplicates, ", nrepeats);
	if (ntransmitted)
		printf("%ld%% packet loss\n",
			   (ntransmitted - nreceived) * 100 / ntransmitted);
	if (nreceived != 0)
		status = EXIT_SUCCESS;
	else
		status = EXIT_FAILURE;

	return status;
}

static char *icmp_type_name(int id) {
	switch (id) {
		case ICMP_ECHOREPLY:
			return "echo reply";
		case ICMP_DEST_UNREACH:
			return "destination unreachable";
		case ICMP_SOURCE_QUENCH:
			return "source quench";
		case ICMP_REDIRECT:
			return "redirect (change route)";
		case ICMP_ECHO:
			return "echo request";
		case ICMP_TIME_EXCEEDED:
			return "time exceeded";
		case ICMP_PARAMETERPROB:
			return "parameter problem";
		case ICMP_TIMESTAMP:
			return "timestamp request";
		case ICMP_TIMESTAMPREPLY:
			return "timestamp reply";
		case ICMP_INFO_REQUEST:
			return "information request";
		case ICMP_INFO_REPLY:
			return "information reply";
		case ICMP_ADDRESS:
			return "address mask request";
		case ICMP_ADDRESSREPLY:
			return "address mask reply";
		default:
			return "unknown ICMP type";
	}
}

/* Ping */
int main(int argc, char **argv) {
	struct {
		icmp_ping_t head;
		uint8_t data[56];
	} packet;

	struct sockaddr_in sk;

	in_addr_t ip_to;

	clock_t timeout, elapsed;

	int dupflag;

	int sd;
	int sock_len = sizeof(sk);
	int i, tot_sent;

	int ch;

	while ((ch = getopt(argc, argv, "c:h")) != -1) {
		switch ((char)ch) {
			case 'c':
				pingcount = atoi(optarg);
			break;

			case '?': case 'h':
			default:
				printf("usage: ping [-c n] destination\n");
				exit(EXIT_FAILURE);
			break;
		}
	}
        argc -= optind;
        argv += optind;

	if( argc < 1 ) {
		printf("usage: ping [-c n] destination\n");
		exit(EXIT_FAILURE);
	}

	if (!inet_aton(argv[0], &ip_to)) {
		printf("usage: ping [-c n] destination\n");
		exit(EXIT_FAILURE);
	}

	sd = socket( AF_INET, SOCK_RAW, IPPROTO_ICMP );
	if (sd < 0) {
		printf("cannot create the socket!\n");
		exit(EXIT_FAILURE);
	}

	memset(&sk, 0, sizeof(sk));
	sk.sin_family = AF_INET;
	sk.sin_addr.s_addr = ip_to;

	if(bind(sd, (struct sockaddr *)&sk, sizeof(sk)) < 0) {
		printf("cannot bind the sender socket!\n");
		goto out;
	}

	for (;;) {

		// Fill the packet.
		memset(&packet, 0, sizeof(packet));
		for (i = 0; i < _countof(packet.data); i++)
        		packet.data[i] = 'a' + i % 26;

		// Fill the header of the ping packet.
		packet.head.ping_id = htons(getpid() & 0xFFFF);
		packet.head.ping_seq = htons(ntransmitted); ntransmitted++;
		packet.head.ping_icmp.icmp_type = ICMP_ECHO;
		packet.head.ping_icmp.icmp_code = 0;
		packet.head.ping_icmp.icmp_chk = 0;
		packet.head.ping_icmp.icmp_chk = ip_checksum(&packet, sizeof(packet));

		CLR(ntohs(packet.head.ping_seq) % MAX_DUP_CHK);

		printf("PING: %u(%u) bytes to %u.%u.%u.%u id=%u seq=%u\n",
			sizeof(packet)-sizeof(icmp_ping_t),
			sizeof(packet),
			IP_A(ntohl(ip_to)), IP_B(ntohl(ip_to)), IP_C(ntohl(ip_to)), IP_D(ntohl(ip_to)),
			ntohs(packet.head.ping_id), ntohs(packet.head.ping_seq));

		tot_sent = sendto(sd, &packet, sizeof(packet), 0, (struct sockaddr *)&sk, sock_len);
		if( tot_sent < 0 ) {
			printf("\nHost unreachable!\n", tot_sent);
			goto out;
		}

		// Insert the address into the socket structure.
		sk.sin_family = AF_INET;
		sk.sin_addr.s_addr = ip_to;
		sock_len = sizeof(sk);

		if(bind(sd, (struct sockaddr *)&sk, sizeof(sk)) < 0) {
			printf("cannot bind the receiver socket!\n");
			goto out;
		}

		// Wait for the reply.
		tot_sent = 0;
		timeout = times(NULL) + PING_INTERVAL;
		for (;;) {
			for (;;) {
				elapsed = times(NULL);
				if (elapsed >= timeout) {
					goto ping_loop;
				}

				tot_sent = recvfrom(sd, &packet, sizeof(packet),
					MSG_DONTWAIT, (struct sockaddr *)&sk, &sock_len);
				if (tot_sent != -EAGAIN) {
					break;
				}
			}

			if (tot_sent < 0) {
				printf("\nsocket error %i!\n", tot_sent);
				goto out;
			}

			if (packet.head.ping_icmp.icmp_type == ICMP_ECHOREPLY) {
				++nreceived;

				if (TST(ntohs(packet.head.ping_seq) % MAX_DUP_CHK)) {
					++nrepeats;
					--nreceived;
					dupflag = 1;
				} else {
					SET(ntohs(packet.head.ping_seq) % MAX_DUP_CHK);
					dupflag = 0;
				}

				// Dump packet info.
				printf("PING: %s from %u.%u.%u.%u id=%u seq=%u, time=%u ms %s",
					icmp_type_name(packet.head.ping_icmp.icmp_type),
					IP_A(ntohl(sk.sin_addr.s_addr)), IP_B(ntohl(sk.sin_addr.s_addr)),
					IP_C(ntohl(sk.sin_addr.s_addr)), IP_D(ntohl(sk.sin_addr.s_addr)),
					ntohs(packet.head.ping_id), ntohs(packet.head.ping_seq),
					(elapsed - (timeout - PING_INTERVAL)) * 1000 / 100,
					dupflag ? " (DUP!)\n" : "\n");
			} else {
				if (packet.head.ping_icmp.icmp_type != ICMP_ECHO)
					printf("Warning: Got ICMP %d (%s) from %u.%u.%u.%u\n",
						packet.head.ping_icmp.icmp_type,
						icmp_type_name(packet.head.ping_icmp.icmp_type));
			}
		}
ping_loop:
		if ((pingcount > 0) && (nreceived >= pingcount))
			break;
	}
out:
	closesocket(sd);
	return pingstats();
}
