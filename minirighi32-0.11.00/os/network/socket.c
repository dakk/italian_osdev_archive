/*!	\file network/socket.c
 *	\brief Standard interface for socket.
 *	\author Andrea Righi <righiandr@users.sf.net>
 *	\date Last update: 2004-04-29\n
 *	\note Copyright (&copy;) 2004 Andrea Righi
 */

#include <const.h>
#include <errno.h>
#include <arch/spinlock.h>
#include <kernel/debug.h>
#include <kernel/queue.h>
#include <kernel/semaphore.h>
#include <net/network.h>
#include <kernel/net/ip.h>
#include <kernel/net/udp.h>
#include <kernel/net/socket.h>

//! Queue of all opened socket.
DECLARE_QUEUE( sock_queue );

//! A semaphore for the mutual exclusion of the socket queue.
DECLARE_MUTEX( sock_mutex );

//! Socket descriptor counter.
int sock_count = 0;

//! Lower numbers are reserved for standard protocols, so let's
//! begin with high values for local port generation.
#define SECURE_LOCAL_PORT	3000

//! Local port numbering.
uint16_t nextport = SECURE_LOCAL_PORT;

// --- LOCAL PORT GENERATION ------------------------------------------ //

DECLARE_SPINLOCK( lport_lock );

//! Get a new local port.
uint16_t new_local_port()
{
	spin_lock( &lport_lock );

	// Assign only high port numbers for local ports.
	if( ++nextport == 0 )
	{
		nextport = SECURE_LOCAL_PORT;
	}

	spin_unlock( &lport_lock );
	return( nextport );
}

// --- STREAM --------------------------------------------------------- //

//! Bind a name to a socket.
int sys_bind( int sockfd, struct sockaddr *my_addr, socklen_t addrlen )
{
	struct sockaddr_in *sin = (struct sockaddr_in *)my_addr;
	queue_t *entry;
	struct sock *sk;
	int n;
	bool found = FALSE;
	int ret = -EBADF;

	DOWN( &sock_mutex );

	// Find the socket structure.
	queue_for_each( entry, n, sock_queue )
	{
		sk = queue_get_entry( entry );
		if( sk->count == sockfd )
		{
			found = TRUE;
			break;
		}
	}
	if( found )
	{
		if( sin->sin_family != sk->domain )
		{
			ret = -EINVAL;
			goto out;
		}
		// Perform the bind.
		sk->ipaddr.s_addr = sin->sin_addr.s_addr;
		sk->port = sin->sin_port;
	}
	// Bind ok.
	ret = 0;
out:
	UP( &sock_mutex );

	return( ret );
}

//! Initiate a connection on a socket.
int sys_connect( int sockfd, const struct sockaddr *serv_addr, socklen_t addrlen )
{
	return( -ENOPROTOOPT );
}

//! Listen for connections on a socket.
int sys_listen( int s, int backlog )
{
	return( -ENOPROTOOPT );
}

//! Accept a connection on a socket.
int sys_accept( int s, struct sockaddr *addr, socklen_t *addrlen )
{
	return( -ENOPROTOOPT );
}

//! Receive a message from a stream socket (connection).
int sys_recv( int s, void *buf, size_t len, int flags )
{
	return( -ENOPROTOOPT );
}

//! Send a message from a stream socket (connection).
int sys_send( int s, const void *msg, size_t len, int flags )
{
	return( -ENOPROTOOPT );
}

// --- DATAGRAM ------------------------------------------------------- //

//! Receive a datagram packet.
int sys_recvfrom( int s, void *buf, size_t len, int flags, struct sockaddr *from, socklen_t *fromlen )
{
	struct sockaddr_in *sin = (struct sockaddr_in *)from;
	queue_t *entry;
	struct sock *sk = NULL;
	int n;
	bool found = FALSE;
	int ret;

	if( from == NULL )
	{
		// Equivalent to a connection oriented receive.
		ret = sys_recv( s, buf, len, flags );
		return( ret );
	}

	DOWN( &sock_mutex );

	// Find the socket structure.
	queue_for_each( entry, n, sock_queue )
	{
		sk = queue_get_entry( entry );
		if( sk->count == s )
		{
			found = TRUE;
			break;
		}
	}

	UP( &sock_mutex );

	if( !found )
	{
		// Invalid socket descriptor.
		ret = -EBADF;
		goto out;
	}

	// Get the socket informations.
	sin->sin_family = sk->domain;
	sin->sin_addr.s_addr = sk->ipaddr.s_addr;
	sin->sin_port = sk->port;

	// Perform the recvfrom operation.
	switch( sk->type )
	{
		case SOCK_RAW:
			// Raw IP packet.
			ret = ip_recvfrom(sk, buf, len, flags, sin, fromlen);
		break;

		case SOCK_DGRAM:
			// UDP packet.
			ret = udp_recvfrom(sk, buf, len, flags, sin, fromlen);
		break;

		case SOCK_STREAM:
			// TODO: implement the connection-oriented recvfrom.
			ret = -ENOPROTOOPT;
		break;

		default:
			ret = -EINVAL;
		break;
	}
out:
	return( ret );
}

//! Send a datagram packet.
int sys_sendto( int s, const void *msg, size_t len, int flags, const struct sockaddr *to, socklen_t tolen )
{
	struct sockaddr_in *sin = (struct sockaddr_in *)to;
	queue_t *entry;
	struct sock *sk = NULL;
	int n;
	bool found = FALSE;
	int ret;

	if( to == NULL )
	{
		// Equivalent to a connection oriented send.
		ret = sys_send( s, msg, len, flags );
		return( ret );
	}

	DOWN( &sock_mutex );

	// Find the socket structure.
	queue_for_each( entry, n, sock_queue )
	{
		sk = queue_get_entry( entry );
		if( sk->count == s )
		{
			found = TRUE;
			break;
		}
	}

	UP( &sock_mutex );

	if( !found )
	{
		// Invalid socket descriptor.
		ret = -EBADF;
		goto out;
	}
	if( sin->sin_family != sk->domain )
	{
		// Invalid socket address family.
		ret = -EINVAL;
		goto out;
	}

	// Perform the sendto operation.
	switch( sk->type )
	{
		case SOCK_RAW:
			// Raw IP packet.
			ret = ip_sendto(sk, msg, len, flags, sin, tolen);
		break;

		case SOCK_DGRAM:
			ret = udp_sendto(sk, msg, len, flags, sin, tolen);
		break;

		case SOCK_STREAM:
			// TODO: implement the connection-oriented sendto.
			ret = -ENOPROTOOPT;
		break;

		default:
			ret = -EINVAL;
		break;
	}
out:
	return( ret );
}

// --- COMMON --------------------------------------------------------- //

//! Create a socket.
int sys_socket( int domain, int type, int protocol )
{
	struct sock *sk;

	if (domain != AF_INET)
		return -EINVAL;

	// AF_INET allows socket datagram, socket stream and raw socket.
	if ((type != SOCK_DGRAM) && (type != SOCK_STREAM) && (type != SOCK_RAW))
		return -EINVAL;

	// Allocate the socket structure.
	sk = kcalloc( 1, sizeof(struct sock), GFP_KERNEL );
	if( sk == NULL )
	{
		printk( KERN_ERR
			"%s(): out of kernel memory!\n",
			__FUNCTION__ );
		return( -ENOMEM );
	}

	// Assign the socket credentials.
	sk->domain = domain;
	sk->type = type;
	sk->protocol = protocol;

	DOWN( &sock_mutex );

	// Get a new socket counter.
	sk->count = ++sock_count;

	// Add the socket to the queue of opened sockets.
	add_queue( &sock_queue, sk );

	UP( &sock_mutex );

	// Return the socket descriptor.
	return( sk->count );
}

//! Close a socket.
int sys_closesocket( int sockfd )
{
	queue_t *entry;
	struct sock *sk = NULL;
	int n;
	bool found = FALSE;

	DOWN( &sock_mutex );

	queue_for_each( entry, n, sock_queue )
	{
		sk = queue_get_entry( entry );
		if( sk->count == sockfd )
		{
			found = TRUE;
			break;
		}
	}
	if( found )
	{
		// Socket found.
		queue_del_entry( &sock_queue, entry );
		// Free unused memory.
		kfree( sk );
	}

	UP( &sock_mutex );

	return( found ? 0 : -EBADF );
}
