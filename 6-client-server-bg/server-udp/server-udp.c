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

#define BUFFER_SIZE 100

void server_listen_udp(void) {

	struct sockaddr_storage client_addr;
	struct addrinfo *servinfo;
	struct addrinfo *cursor;
	struct addrinfo hints;
	socklen_t addr_len;

	char buf[BUFFER_SIZE];
	char s[INET6_ADDRSTRLEN];

	int nbytes;
	int sock;
	int rv;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = AI_PASSIVE;

	if ((rv = getaddrinfo(NULL, "3490", &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		exit(1);
	}

	for (cursor = servinfo; cursor != NULL; cursor = cursor->ai_next) {
		if ((sock = socket(cursor->ai_family,
				   cursor->ai_socktype,
				   cursor->ai_protocol)) == -1) {
			perror("server:socket");
			continue;
		}

		if (bind(sock, cursor->ai_addr, cursor->ai_addrlen) == -1) {
			close(sock);
			perror("server:bind");
			continue;
		}

		break;
	}

	if (!cursor) {
		fprintf(stderr, "server:failed to bind socket\n");
		exit(1);
	}

	freeaddrinfo(servinfo);

	fprintf(stdout, "listening on localhost:3490");

	addr_len = sizeof(client_addr);
	if ((nbytes = recvfrom(sock,
			       buf,
			       BUFFER_SIZE - 1,
			       0,
			       (struct sockaddr *) &client_addr,
			       &addr_len)) == -1) {
		perror("recvfrom");
		exit(1);
	}

	void *in_addr;
	if (((struct sockaddr *) &client_addr)->sa_family == AF_INET) {
		in_addr = &(((struct sockaddr_in *) &client_addr)->sin_addr);
	} else {
		in_addr = &(((struct sockaddr_in6 *) &client_addr)->sin6_addr);
	}

	fprintf(stdout,
		"server:req from %s\n",
		inet_ntop(client_addr.ss_family, in_addr, s, sizeof(s)));

	buf[nbytes] = '\0';
	fprintf(stdout, "server:packet length: %d bytes\n", nbytes);
	fprintf(stdout, "server:packet:\n%s\n", buf);

	close(sock);
}
