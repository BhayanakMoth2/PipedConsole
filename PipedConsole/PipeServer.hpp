#pragma once
#include<Windows.h>
#include <string>
#include<iostream>
#include<sstream>
#define BUFFERSIZE 1024
enum Header
{
	STRING_INFO,
	STRING_WARN,
	PING,
	PONG,
	EXIT
};
class PipeServer
{
public:
	PipeServer(std::string pipename);
	void OpenNamedPipe(std::string pipename);
	int WritePipe(char * buffer, int size);
	int ReadPipe(char * buffer, size_t size);
	Header ReadHeader();
	void ProcessHeader(Header header);
	void WriteHeader(Header p_Header);
	void operator <<(char * buffer);
	void operator <<(std::string buffer);
	~PipeServer();
private:
	const size_t m_BufferSize = BUFFERSIZE;
	HANDLE hPipe;
	std::string pipeurl = "\\\\.\\pipe\\";
};

