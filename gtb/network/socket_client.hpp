#ifndef GTB_SOCKET_CLIENT_INCLUDED
#define GTB_SOCKET_CLIENT_INCLUDED

#include <gtb/common.hpp>

GTB_BEGIN_NAMESPACE

class socket_client {
public:
	socket_client();

	~socket_client();

	bool connect(const char *remote_host, int remote_port);

	int socket();

protected:
	int _socket;
};

GTB_END_NAMESPACE

#endif // GTB_SOCKET_CLIENT_INCLUDED
