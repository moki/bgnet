#include <stdio.h>
#include "client-tcp.h"

int main(int argc, char **arv) {
	printf("client code\n");
	client_connect_tcp();
}
