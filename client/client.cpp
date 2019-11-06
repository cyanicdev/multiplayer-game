#include<cstdio>
#include<cstring>
#define _WIN32_WINNT 0x600

#ifdef _WIN32
#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include<unistd.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netdb.h>
#include<arpa/inet.h>
#endif

#define MAXDATASIZE 100

void* get_in_addr(struct sockaddr* sa)
{
	if (sa->sa_family == AF_INET)
	{
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(int argc, char* argv[])
{
	if (argc != 3) {
		fprintf(stderr, "usage: client.exe hostname port\n");
		exit(1);
	}

	int status;
	struct addrinfo hints, * res;
	int sock;
	char s[INET6_ADDRSTRLEN];
	char buf[MAXDATASIZE];

#ifdef _WIN32
	// Initialize Winsock.
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != NO_ERROR) {
		wprintf(L"WSAStartup failed with error: %ld\n", iResult);
		return 1;
	}
#endif

	// Before using hint you have to make sure that the data structure is empty 
	memset(&hints, 0, sizeof hints);
	// Set the attribute for hint
	hints.ai_family = AF_UNSPEC; // We don't care V4 AF_INET or 6 AF_INET6
	hints.ai_socktype = SOCK_STREAM; // TCP Socket SOCK_DGRAM 

	// Fill the res data structure and make sure that the results make sense. 
	status = getaddrinfo(argv[1], argv[2], &hints, &res);
	if (status != 0)
	{
		fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
	}

	// Create Socket and check if error occured afterwards
	sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if (sock < 0)
	{
		fprintf(stderr, "socket error: %s\n", gai_strerror(status));
	}

	// Bind the socket to the address of my local machine and port number 
	status = connect(sock, res->ai_addr, res->ai_addrlen);
	if (status < 0)
	{
		fprintf(stderr, "bind: %s\n", gai_strerror(status));
	}

	// Free the res linked list after we are done with it	

	inet_ntop(res->ai_family, get_in_addr((struct sockaddr*) res->ai_addr), s, sizeof s);
	printf("connecting to %s\n", s);

	freeaddrinfo(res);

	status = recv(sock, buf, MAXDATASIZE - 1, 0);
	if (status == -1)
	{
		fprintf(stderr, "recv: %s\n", gai_strerror(status));
#ifdef _WIN32
		closesocket(sock);
		WSACleanup();
#else
		close(sock);
#endif
		_exit(4);
	}

	buf[status] = '\0';
	printf("client: received '%s'\n", buf);

	// Close the socket before we finish 
#ifdef _WIN32
	closesocket(sock);
	WSACleanup();
#else
	close(sock);
#endif

	return 0;
}