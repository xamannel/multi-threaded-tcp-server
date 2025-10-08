
/* Class to manage socket resource with RAII pattern */


#ifndef SOCKET_RAII_H
#define SOCKET_RAII_H

#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>


class Socket
{

	constexpr static int BACKLOG = 10; 
	int socket_fd = -1;

	public :

		// Constructor
		
		Socket(int domain, int type, int protocol) noexcept;

		explicit Socket(int fd) noexcept;
		~Socket() noexcept;

		// Delete copy constructor and copy assignment operator to prevent copying

		Socket(const Socket&) = delete;
		Socket& operator=(const Socket&) = delete;
		
		// Move semantics
		
		Socket(Socket&& other) noexcept;
		Socket& operator=(Socket&& other) noexcept;

		// Bind the socket to the local address Ipv4 or Ipv6
		template<typename T>
		void bind(const T* addr, socklen_t addrlen) noexcept;
		void listen(int backlog = BACKLOG) noexcept;
		Socket accept(struct sockaddr* addr, socklen_t* addrlen) noexcept;
		ssize_t recv(void* buffer, size_t length, int flags = 0) noexcept;
		ssize_t send(const void* buffer, size_t length, int flags = 0) noexcept;
		int getSocketFD() const noexcept;
		
		bool isValid() const noexcept;
		void setNonBlocking(bool nonBlocking) noexcept;
};

#endif

