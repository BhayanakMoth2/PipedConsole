#include "PipeServer.hpp"
int main()
{
	PipeServer server = PipeServer("namedpipe");
	char buffer[100] = "Mic Check Mic Check!";
	server << buffer;
	std::string strbuffer = " ";
	server.WriteHeader(Header::EXIT);
	while (true)
	{
		std::cout << "Enter: ";
		std::getline(std::cin, strbuffer);
		server << strbuffer;
	}
	return 0;
}