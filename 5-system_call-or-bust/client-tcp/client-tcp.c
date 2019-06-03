#define _POSIX_C_SOURCE (200112L)
#define _XOPEN_SOURCE (1)
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "client-tcp.h"

void connect_tcp(void) {
	int status;
	struct addrinfo hints;
	struct addrinfo *servinfo;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if ((status = getaddrinfo("www.google.com", "3490", &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
		exit(1);
	}

	printf("client code is ok\n");

	/* display servifno linked list */
	printf("ip addresses for google.com\n");
	char ipstr[INET6_ADDRSTRLEN];
	struct addrinfo *cursor;
	for (cursor = servinfo; cursor != NULL; cursor = cursor->ai_next) {
		void *addr;
		char *ipver;
		/* ipv4 */
		if (cursor->ai_family == AF_INET) {
			struct sockaddr_in *ipv4 =
			(struct sockaddr_in *) cursor->ai_addr;
			addr = &(ipv4->sin_addr);
			ipver = "ipv4";
		} else {
			struct sockaddr_in6 *ipv6 =
			(struct sockaddr_in6 *) cursor->ai_addr;
			addr = &(ipv6->sin6_addr);
			ipver = "ipv6";
		}

		inet_ntop(cursor->ai_family, addr, ipstr, sizeof(ipstr));
		printf("%s:%s\n", ipver, ipstr);
	}

	freeaddrinfo(servinfo);
}
