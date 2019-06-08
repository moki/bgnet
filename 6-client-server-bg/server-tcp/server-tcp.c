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

static void sigchild_handler(int signal);

void server_listen_tcp(void) {
	int sock, _sock;
	struct addrinfo hints;
	struct addrinfo *servinfo;
	struct addrinfo *cursor;
	struct sockaddr_storage client_address;
	socklen_t sin_size;
	struct sigaction sa;
	char s[INET6_ADDRSTRLEN];
	int rv;
	int reuse_address = 1;
	char *ok_msg = "200: ok\n";

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	sa.sa_handler = sigchild_handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if (sigaction(SIGCHLD, &sa, NULL) == -1) {
		perror("sigaction");
		exit(1);
	}

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

		if (setsockopt(sock,
			       SOL_SOCKET,
			       SO_REUSEADDR,
			       &reuse_address,
			       sizeof(int)) == -1) {
			perror("setsockopt");
			exit(1);
		}

		if (bind(sock,
			 cursor->ai_addr,
			 cursor->ai_addrlen) == -1) {
			close(sock);
			perror("server:bind");
			continue;
		}

		break;
	}

	freeaddrinfo(servinfo);

	if (!cursor) {
		fprintf(stderr, "server: failed to bind\n");
		exit(1);
	}

	printf("server: listening on 3490\n");

	if (listen(sock, 20) == -1) {
		perror("server:listen");
		exit(1);
	}

	while (1) {
		sin_size = sizeof(client_address);
		_sock = accept(sock, (struct sockaddr *) &client_address, &sin_size);
		if (_sock == -1) {
			perror("accept");
			continue;
		}

		void *in_addr;
		if (((struct sockaddr *) &client_address)->sa_family == AF_INET) {
			in_addr = &(((struct sockaddr_in *) &client_address)->sin_addr);
		} else {
			in_addr = &(((struct sockaddr_in6 *) &client_address)->sin6_addr);
		}

		inet_ntop(client_address.ss_family, in_addr, s, sizeof(s));
		printf("server: inc. connection from %s\n", s);

		if (!fork()) {
			close(sock);
			if (send(_sock, ok_msg, 8, 0) == -1) {
				perror("server:send");
			}
			close(_sock);
			exit(0);
		}

		close(_sock);
	}
}

static void sigchild_handler(int signal) {
	int saved_errno = errno;
	while(waitpid(-1, NULL, WNOHANG) > 0);
	errno = saved_errno;
}
