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

#define BUFFER_SIZE (100)

void client_connect_tcp(void) {
	struct addrinfo *servinfo;
	struct addrinfo *cursor;
	struct addrinfo hints;

	char *host = "localhost";
	char s[INET6_ADDRSTRLEN];
	char buf[BUFFER_SIZE];

	int nbytes;
	int sock;
	int rv;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

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

		if (connect(sock, cursor->ai_addr, cursor->ai_addrlen) == -1) {
			close(sock);
			perror("client:connect");
			continue;
		}

		break;
	}

	if (!cursor) {
		fprintf(stderr, "client:connection failed");
		exit(1);
	}

	void *in_addr;
	if (((struct sockaddr *) &cursor->ai_addr)->sa_family == AF_INET) {
		in_addr = &(((struct sockaddr_in *) &cursor->ai_addr)->sin_addr);
	} else {
		in_addr = &(((struct sockaddr_in6 *) &cursor->ai_addr)->sin6_addr);
	}

	inet_ntop(cursor->ai_family, in_addr, s, sizeof(s));
	printf("client:connecting to %s\n", s);
	freeaddrinfo(servinfo);

	if ((nbytes = recv(sock, buf, BUFFER_SIZE - 1, 0)) == -1) {
		perror("recv");
		exit(1);
	}

	buf[nbytes] = '\0';
	printf("client:received:\n%s\n", buf);
	close(sock);
}
