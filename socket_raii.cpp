#include <unistd.h>
#include <fcntl.h>
#include <stdexcept>
#include <iostream>
#include <arpa/inet.h>
#include "socket_raii.h"

Socket::Socket(int domain, int type, int protocol) noexcept
{
    socket_fd = ::socket(domain, type, protocol);
    if (socket_fd == -1) {
	std::cerr << "Socket creation failed: " << strerror(errno) << std::endl;
    }
}


Socket::Socket(int fd) noexcept : socket_fd(fd) {}



Socket::~Socket() noexcept
{
	::close(socket_fd);
    	socket_fd = -1;
}

Socket::Socket(Socket&& other) noexcept : socket_fd(other.socket_fd)
{
    other.socket_fd = -1;
}

Socket& Socket::operator=(Socket&& other) noexcept
{
    if (this != &other) {
	::close(socket_fd);
	socket_fd = other.socket_fd;
	other.socket_fd = -1;
    }
    return *this;
}



template<typename T>
void Socket::bind(const T* addr, socklen_t addrlen) noexcept
{
    if (::bind(socket_fd, reinterpret_cast<const struct sockaddr*>(addr), addrlen) == -1) {
	std::cerr << "Bind failed: " << strerror(errno) << std::endl;
    }
}

void Socket::listen(int backlog) noexcept
{
    if (::listen(socket_fd, backlog) == -1) {
	std::cerr << "Listen failed: " << strerror(errno) << std::endl;
    }
}

Socket Socket::accept(struct sockaddr* addr, socklen_t* addrlen) noexcept
{
    int client_fd = ::accept(socket_fd, addr, addrlen);
    if (client_fd == -1) {
	std::cerr << "Accept failed: " << strerror(errno) << std::endl;
	return Socket(-1); // Return an invalid socket
    }
    return Socket(client_fd);
}

ssize_t Socket::recv(void* buffer, size_t length, int flags) noexcept
{
    ssize_t bytes_received = ::recv(socket_fd, buffer, length, flags);
    if (bytes_received == -1) {
	std::cerr << "Receive failed: " << strerror(errno) << std::endl;
    }
    return bytes_received;
}

ssize_t Socket::send(const void* buffer, size_t length, int flags) noexcept
{
    ssize_t bytes_sent = ::send(socket_fd, buffer, length, flags);
    if (bytes_sent == -1) {
	std::cerr << "Send failed: " << strerror(errno) << std::endl;
    }
    return bytes_sent;
}

int Socket::getSocketFD() const noexcept
{
    return socket_fd;
}

/* void Socket::setNonBlocking(bool nonBlocking) noexcept
{
    int flags = fcntl(socket_fd, F_GETFL, 0);
    if (flags == -1) {
	std::cerr << "fcntl F_GETFL failed: " << strerror(errno) << std::endl;
	return;
    }
    if (nonBlocking) {
	flags |= O_NONBLOCK;
    } else {
	flags &= ~O_NONBLOCK;
    }
    if (fcntl(socket_fd, F_SETFL, flags) == -1) {
	std::cerr << "fcntl F_SETFL failed: " << strerror(errno) << std::endl;
    }
}

*/

