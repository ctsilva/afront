#include <gtb/gtb.hpp>
#ifndef WIN32
#include <gtb/network/socket_client.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <netinet/in.h>
#endif // WIN32


GTB_BEGIN_NAMESPACE


socket_client::socket_client()
{
}


bool socket_client::connect(const char *remote_host, int remote_port)
{
	// create the socket
	_socket = ::socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (-1 == _socket) {
		perror("socket");
		//exit(EXIT_FAILURE);
		return false;
	}

	// Find the host address.  First, try a hostname.  If it
	// fails, try an IP address.
	struct hostent *host_ptr = gethostbyname(remote_host);
	if (0 == host_ptr) {
		host_ptr = gethostbyaddr(remote_host, strlen(remote_host),
					 AF_INET);
		if (0 == host_ptr) {
			perror("Error resolving server address");
			//exit(EXIT_FAILURE);
			return false;
		}
	}

	// build the address of the remote structure, and attempt to
	// connect to it
	struct sockaddr_in server_name;
	server_name.sin_family = AF_INET;
	server_name.sin_port = htons(remote_port);
	memcpy(&server_name.sin_addr, host_ptr->h_addr, host_ptr->h_length);

	int status = ::connect(_socket, (struct sockaddr *) &server_name,
			       sizeof(server_name));
	if (-1 == status) {
		perror("connect");
		//exit(EXIT_FAILURE);
		return false;
	}
	return true;
}


socket_client::~socket_client()
{
	close(_socket);
}


int socket_client::socket()
{
	return _socket;
}


GTB_END_NAMESPACE
