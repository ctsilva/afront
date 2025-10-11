#ifndef GTB_SOCKET_SERVER_INCLUDED
#define GTB_SOCKET_SERVER_INCLUDED

#include <gtb/common.hpp>

GTB_BEGIN_NAMESPACE

class socket_server {
public:
	socket_server();

	~socket_server();

	bool connect(int port);

	// returns the client socket
	int accept();

protected:
	int _server_socket;
};

GTB_END_NAMESPACE

#endif // GTB_SOCKET_SERVER_INCLUDED
