#define _POSIX_C_SOURCE (200112L)
#define _XOPEN_SOURCE (1)
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "server-tcp.h"

void listen_tcp(void) {
	int status;
	struct addrinfo hints;
	struct addrinfo *servinfo;
	struct sockaddr_storage their_addr;
	socklen_t addr_size;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	/* assign addresss of my local host to the socket structures,
	 * if node is set to NULL
	 */
	hints.ai_flags = AI_PASSIVE;

	if ((status = getaddrinfo(NULL, "3490", &hints, &servinfo)) !=0) {
		fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
		exit(1);
	}

	printf("server code is ok\n");
	/* servinfo now points to a linked list of 1 or more struct addrinfos */

	int sockfd = socket(servinfo->ai_family,
			    servinfo->ai_socktype,
			    servinfo->ai_protocol);

	bind(sockfd, servinfo->ai_addr, servinfo->ai_addrlen);
	listen(sockfd, 20);
	addr_size = sizeof(their_addr);
	int newfd = accept(sockfd, (struct sockaddr *) &their_addr, &addr_size);

	char *msg = "Hi from moki's server";
	size_t msg_len = strlen(msg);
	ssize_t bytes_sent = send(newfd, msg, msg_len, 0);
	if (bytes_sent < 0) {
		fprintf(stderr, "failed to send msg to client\n");
		exit(1);
	}

	printf("sent back msg :)\n");

	freeaddrinfo(servinfo);
}
