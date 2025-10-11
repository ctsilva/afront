#include <gtb/gtb.hpp>
#ifndef WIN32
#include <gtb/network/network.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#endif // WIN32
using namespace gtb;


void work(int client_socket)
{
	char buffer[256] = "";
	int status;
	while (0 < (status = read(client_socket,
				  buffer, sizeof(buffer) - 1))) {
		buffer[status] = '\0';
		printf("socket client: received %d bytes - \"%s\"\n",
		       status, buffer);
	}
	if (-1 == status) {
		perror("read");
	}
}


int main(int argc, char *argv[])
{
	if (3 != argc) {
		fprintf(stderr, "Usage: %s <remote_host> <remote_port>\n",
			argv[0]);
		exit(EXIT_FAILURE);
	}

	char *remote_host = argv[1];
	int remote_port = atoi(argv[2]);

	socket_client client;
	client.connect(remote_host, remote_port);
	work(client.socket());

	return 0;
}
