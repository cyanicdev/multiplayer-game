#include <iostream>
#include <thread>
#include "asio.hpp"

using asio::ip::udp;
#define MAX_PACKET 4096

int runserver(char* port)
{
	try
	{
		asio::io_context io_context;

		udp::socket socket(io_context, udp::endpoint(udp::v4(), atoi(port)));

		for (;;)
		{
			std::array<char, MAX_PACKET> recv_buf;
			udp::endpoint remote_endpoint;
			asio::error_code error;
			socket.receive_from(asio::buffer(recv_buf), remote_endpoint);

			printf("Received message from %s on port %s: %s\n", remote_endpoint.address().to_string().c_str(), std::to_string(remote_endpoint.port()).c_str(), recv_buf.data());

			std::string message = "You successfully sent '" + std::string(recv_buf.data()) + "' from " + remote_endpoint.address().to_string() + " on port " + std::to_string(remote_endpoint.port()) + "\n" + make_daytime_string();

			asio::error_code ignored_error;
			socket.send_to(asio::buffer(message), remote_endpoint, 0, ignored_error);
		}
	}
	catch (std::exception & e)
	{
		std::cerr << e.what() << std::endl;
		return 1;
	}
	return 0;
}

int runclient(char* server, char* port)
{
	try
	{
		asio::io_context io_context;

		udp::resolver resolver(io_context);
		udp::endpoint receiver_endpoint = *resolver.resolve(udp::v4(), server, port).begin();

		udp::socket socket(io_context);
		socket.open(udp::v4());

		std::array<char, MAX_PACKET> send_buf = { 0 };

		printf("Text to send: ");
		std::cin.getline(send_buf.data(), MAX_PACKET);

		socket.send_to(asio::buffer(send_buf), receiver_endpoint);

		std::array<char, MAX_PACKET> recv_buf;
		udp::endpoint sender_endpoint;
		size_t len = socket.receive_from(asio::buffer(recv_buf), sender_endpoint);

		std::cout.write(recv_buf.data(), len);
	}
	catch (std::exception & e)
	{
		std::cerr << e.what() << std::endl;
		return 1;
	}
	return 0;
}

std::string make_daytime_string()
{
	using namespace std; // For time_t, time and ctime;
	time_t now = time(0);
	return ctime(&now);
}

int main(int argc, char* argv[])
{
	if (argc > 1 && std::string(argv[1]) == "-server")
	{
		// Run a server
		if (argc != 3)
		{
			printf("specify a port: %s -server port", argv[0]);
			return 1;
		}

		return runserver(argv[2]);
	}
	else
	{
		// Run a client
		if (argc != 3)
		{
			printf("specify a server to connect: %s server port", argv[0]);
			return 1;
		}

		return runclient(argv[1], argv[2]);
	}
}