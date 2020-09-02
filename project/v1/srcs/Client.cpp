#include "../includes/Headers.hpp"

/*
** constructors / destructors
*/

Client::Client(int accept_fd):
    _accept_fd(accept_fd)
{
    ;
}

Client::~Client()
{
	// free(_buffer);
	// _buffer = NULL;
	// if (_accept_fd != -1)
	// {
		FD_CLR(_accept_fd, &g_conf._readfds);
		FD_CLR(_accept_fd, &g_conf._save_readfds);
		close(_accept_fd);
        _accept_fd = -1;
	// }
	std::cout << "connection closed" << std::endl;

}

/*
** other class methods
*/

int Client::parse_request(void)
{
    _request.init();
    return (1);
}