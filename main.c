#include <stdio.h>
#include "5-system_call-or-bust/server-tcp/server-tcp.h"
#include "5-system_call-or-bust/client-tcp/client-tcp.h"

int main(int argc, char **argv) {
	printf("hello\n");
	listen_tcp();
	connect_tcp();
}
