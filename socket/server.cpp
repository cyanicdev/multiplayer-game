#include <ctime>
#include <iostream>
#include <string>
#include <boost/array.hpp>
#include <boost/asio.hpp>

using boost::asio::ip::udp;

std::string make_daytime_string()
{
	using namespace std; // For time_t, time and ctime;
	time_t now = time(0);
	return ctime(&now);
}

int main()
{
	try
	{
		boost::asio::io_context io_context;

		udp::socket socket(io_context, udp::endpoint(udp::v4(), 13));

		for (;;)
		{
			boost::array<char, 1024> recv_buf = { {0} };
			udp::endpoint remote_endpoint;
			boost::system::error_code error;
			socket.receive_from(boost::asio::buffer(recv_buf),
				remote_endpoint, 0, error);

			if (error && error != boost::asio::error::message_size)
				throw boost::system::system_error(error);

			std::cout << std::string(recv_buf.begin(), recv_buf.end()) << std::endl;

			std::string message = make_daytime_string() + "\nYou connected boi";

			boost::system::error_code ignored_error;
			socket.send_to(boost::asio::buffer(message), remote_endpoint, 0, ignored_error);
		}
	}
	catch (std::exception & e)
	{
		std::cerr << e.what() << std::endl;
	}

	return 0;
}