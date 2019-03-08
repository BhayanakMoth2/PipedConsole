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
void Block()
{
	std::cout << "\nPress Any Key To Continue...";
	char a = ' ';
	getchar();
}
void ThrowLastError(std::string e)
{
	int error = GetLastError();
	std::string err;
	try {
		throw ErrorHandling::WinError(GetLastError(), e);
	}
	catch (const ErrorHandling::WinError err)
	{
		std::cout << "RUN TIME EXCEPTION: " <<  err.what();
		Block();
	}
}
class PipeClient
{
public:
	PipeClient(std::string pipename);
	int Connect();
	int ReadPipe(char * buffer, size_t size);
	int WritePipe(char * buffer, size_t size);
	int ConsoleOut(std::string message);
	void Update();
	void ProcessHeader(const Header p_Header)
	{
		switch (p_Header)
		{
			case STRING_INFO:
				{
					char  buffer[BUFFERSIZE];
					ReadPipe(buffer, m_BufferSize);
					buffer[m_BufferSize - 1] = '\0';
					std::cout << "[INFO]:" << buffer;
					break;
				}
			case STRING_WARN:
				{
					char  buffer[BUFFERSIZE];
					ReadPipe(buffer, m_BufferSize);
					buffer[m_BufferSize - 1] = '\0';
					std::cout << "[WARNING]:" << buffer;
					break;
				}
			case PING:
				{
					Header head = Header::PONG;
					WritePipe((char*)&head, sizeof(Header));
				}
		}
	}
	~PipeClient();
private:
	const size_t m_BufferSize = BUFFERSIZE;
	HANDLE hPipe;
	std::string m_PipeName = "\\\\.\\pipe\\";
};
