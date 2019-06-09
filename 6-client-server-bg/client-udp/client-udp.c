#define _POSIX_C_SOURCE (200809L)
#define _XOPEN_SOURCE (1)
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include "client-udp.h"

void client_connect_udp(void) {
	struct addrinfo *servinfo;
	struct addrinfo *cursor;
	struct addrinfo hints;
	char *host = "0.0.0.0";
	char *msg = "Hello there, its client\n";

	int nbytes;
	int sock;
	int rv;

	memset(&hints, 0, sizeof(hints));
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_family = AF_UNSPEC;

	if ((rv = getaddrinfo(host, "3490", &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		exit(1);
	}

	for (cursor = servinfo; cursor != NULL; cursor = cursor->ai_next) {
		if ((sock = socket(cursor->ai_family,
				   cursor->ai_socktype,
				   cursor->ai_protocol)) == -1) {
			perror("client:socket");
			continue;
		}

		break;
	}

	if (!cursor) {
		fprintf(stderr, "client:failed to create socket\n");
		exit(1);
	}

	if ((nbytes = sendto(sock,
			     msg,
			     strlen(msg),
			     0,
			     cursor->ai_addr,
			     cursor->ai_addrlen)) == -1) {
		perror("client: sendto");
		exit(1);
	}

	freeaddrinfo(servinfo);

	fprintf(stdout, "client: sent %d bytes to %s\n", nbytes, host);
	close(sock);
}
