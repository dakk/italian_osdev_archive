/*!	\file apps/src/socket/socket.c
 *	\brief Socket test.
 *	\author Andrea Righi <righiandr@users.sf.net>
 *	\date Last update: 2004-30-04\n
 */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/sys.h>
#include <net/ip.h>
#include <net/network.h>
#include <net/socket.h>

#define PROTOPORT	8080

int main( int argc, char **argv )
{
	int sin_len;
	char message[ 256 ];
	int sd;
	int port = 0;
	struct sockaddr_in sin;

	if( argc > 1 )
	{
		port = atoi( argv[ 1 ] );
	}
	if( port == 0 )
	{
		port = PROTOPORT;
	}

	printf( "Hi! This is a simple UDP chat server to test user-space sockets.\n"
		"\n"
		"This program simply wait for incoming UDP packets on port '%u'\n"
		"and prints the content of every packet on the screen...\n"
		"\n"
		"You can stop the server sending the string 'stop' from any client\n"
		"or killing manually this process (if you ran it in foreground go\n"
		"to another console and use the 'kill' command).\n"
		"\n"
		"Enjoy!\n"
		"  -Andrea.\n"
		"\n", PROTOPORT );

	memset( &sin, 0, sizeof(sin) );
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = htonl( INADDR_ANY );
	sin.sin_port = htons( port );

	printf( "Creating an UDP socket ...\n" );
	sd = socket( AF_INET, SOCK_DGRAM, 0 );
	if( sd < 0 )
	{
		printf( "Cannot create the socket! error %d\n", errno );
		exit( EXIT_FAILURE );
	}
	if( bind(sd, (struct sockaddr *)&sin, sizeof(sin)) < 0 )
	{
		printf( "Cannot perform the binding! error %d\n", errno );
		exit( EXIT_FAILURE );
	}

	printf( "Waiting for UDP packets...\n" );
	for( ; ; )
	{
		sin_len = sizeof(sin);
		if( recvfrom(sd, message, sizeof(message), 0, (struct sockaddr *)&sin, &sin_len) < 0 )
		{
			printf( "error %u during RECVFROM operation!\n", errno );
			exit( EXIT_FAILURE );
		}
		printf( "%u, %u\n", ntohs(sin.sin_port), ntohl(sin.sin_addr.s_addr) );
		printf( "Reply from server:\n\n%s\n", message );
		if( strncmp(message, "stop", 4) == 0 ) break;
	}
	closesocket( sd );

	return( EXIT_SUCCESS );
}
