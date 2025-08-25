#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>

int const PORT = 5000;


// RAII object for socket management

class Socket
{

	int socket_fd;
	public: 
		Socket(int domain, int type, int protocol)
		{
			socket_fd = ::socket(domain, type, protocol);
			if (socket_fd < 0)
			{
				throw std::runtime_error("Failed to create socket");
			}
		}

		// Constructor for an existing socket file descriptor


		explicit Socket(int fd) : socket_fd(fd) 
		
		{    
		if (fd < 0) throw std::runtime_error("Invalid fd");
		}


		~Socket()
		{
			close(socket_fd);
		}


		// Bind the socket to the local address Ipv4 or Ipv6
		
		template <typename SockAddrT>
		void bind(const SockAddrT& addr)
		{
		    if (::bind(socket_fd,
			       reinterpret_cast<const sockaddr*>(&addr),
			       sizeof(SockAddrT)) < 0)
		    {
			throw std::system_error(errno, std::generic_category(), "bind failed");
		    }

		    std::cout << "Socket successfully bound\n";
		}

		// Listen for incoming connections on the socket with backlog the maximum number of pending connections
		void listen(int backlog = 5)
		{
			if (::listen(socket_fd, backlog) < 0)
			{
				throw std::system_error(errno, std::generic_category(), "listen failed");
			}
			std::cout << "Socket is now listening\n";
		}

		Socket accept(sockaddr_in& clientAddr)
		{
			socklen_t clientAddrLen = sizeof(clientAddr);
			int clientSocketFD = ::accept(socket_fd, reinterpret_cast<sockaddr*>(&clientAddr), &clientAddrLen);
			if (clientSocketFD < 0)
			{
				throw std::system_error(errno, std::generic_category(), "accept failed");
			}
			std::cout << "Accepted a new connection\n";
			return Socket(clientSocketFD);
		}

		// Receive message from the socket
		void recv(void* buffer, size_t length, int flags = 0)
		{
			ssize_t bytesReceived = ::recv(socket_fd, buffer, length, flags);
			if (bytesReceived < 0)
			{
				throw std::system_error(errno, std::generic_category(), "recv failed");
			}
			std::cout << "Received " << bytesReceived << " bytes: " << std::string(static_cast<char*>(buffer), bytesReceived) << "\n";
		}

		// Send message to the socket
		void send(const void* buffer, size_t length, int flags = 0)
		{
			ssize_t bytesSent = ::send(socket_fd, buffer, length, flags);
			if (bytesSent < 0)
			{
				throw std::system_error(errno, std::generic_category(), "send failed");
			}
			std::cout << "Sent " << bytesSent << " bytes\n";
		}
		int getSocketFD() const
		{
			return socket_fd;
		}

};
int main()
{
	int socketListen;
	struct sockaddr_in socketAddr_IPV4{};

	Socket socket_server(AF_INET, SOCK_STREAM, 0); /* 0 means we use the default protocol of SOCK_STREAM (tcp) */

	
	char buffer[1024];
	char broadcastMessage[] = "Hello from server";

	// Prepare the adress for the local binding

	socketAddr_IPV4.sin_family = AF_INET;
	socketAddr_IPV4.sin_addr.s_addr = htonl(INADDR_ANY); 
	socketAddr_IPV4.sin_port = htons(PORT);	

	// Bind the socket 
	
	socket_server.bind(socketAddr_IPV4);

	// Listen for incoming connections
	
	socket_server.listen();
	
	// Loop for incoming connections 
	
	bool running = true;
	while (running)
	{
		std::cout << "Waiting for incoming connections...\n";
		sockaddr_in clientAddr{};
		Socket socket_communication = socket_server.accept(clientAddr);
		socket_communication.recv(buffer, sizeof(buffer));
		socket_communication.send(broadcastMessage, sizeof(broadcastMessage));
		std::cout << "Client IP: " << inet_ntoa(clientAddr.sin_addr) << ", Port: " << ntohs(clientAddr.sin_port) << "\n";
        }

	return 0;

}
