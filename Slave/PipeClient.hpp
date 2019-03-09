#pragma once
#include <conio.h>
#include <Windows.h>
#include <iostream>
#include <sstream>
#include "ErrorHandling.hpp"
#define BUFFERSIZE 1024
enum Header
{
	STRING_INFO,
	STRING_WARN,
	PING,
	PONG,
	EXIT
};

class PipeClient
{
public:
	PipeClient(std::string pipename);
	int Connect();
	int ReadPipe(char * buffer, size_t size);
	int WritePipe(char * buffer, size_t size);
	int ConsoleOut(std::string message);
	void Update();
	Header ReadHeader()
	{
		Header header;
		ReadPipe((char*)&header, sizeof(Header));
		return header;
	}
	void ProcessHeader(const Header p_Header)
	{
		switch (p_Header)
		{
			case STRING_INFO:
				{
					char  buffer[BUFFERSIZE];
					int bytesRead = ReadPipe(buffer, m_BufferSize);
					buffer[m_BufferSize - 1] = '\0';
					std::cout << "[INFO]: " << buffer << "\n";
					break;
				}
			case STRING_WARN:
				{
					char  buffer[BUFFERSIZE];
					ReadPipe(buffer, m_BufferSize);
					buffer[m_BufferSize - 1] = '\0';
					std::cout << "[WARNING]: " << buffer << "\n";
					break;
				}
			case PING:
				{
					Header head = Header::PONG;
					WritePipe((char*)&head, sizeof(Header));
				}
			case PONG:
				{
					break;
				}
			case EXIT:
				{
					m_KeepUpdating = false;
					std::cout << "Quitting...\n";
					break;
				}
			default:
				{
					std::cout << "Malformed Packet recieved: " << "\n";
				}
		}
	}
	~PipeClient();
private:
	const size_t m_BufferSize = BUFFERSIZE;
	HANDLE hPipe;
	std::string m_PipeName = "\\\\.\\pipe\\";
	bool m_KeepUpdating = true;
};
