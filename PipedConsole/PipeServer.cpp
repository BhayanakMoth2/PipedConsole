#pragma once
#include "PipeServer.hpp"
#include "ErrorHandling.hpp"
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
		std::cout << "RUN TIME EXCEPTION: " << err.what();
		Block();
	}
}
PipeServer::PipeServer(std::string pipename)
{
	OpenNamedPipe(pipename);
}
void PipeServer::OpenNamedPipe(std::string pipename)
{

	pipeurl += pipename;
	hPipe = CreateNamedPipe(
		pipeurl.c_str(),				             // pipe name 
		PIPE_ACCESS_DUPLEX,                          // read/write access 
		PIPE_TYPE_BYTE |							 // Datatype Byte
		PIPE_WAIT,								     // blocking mode 
		1,									         // max. instances  
		m_BufferSize,								 // output buffer size 
		m_BufferSize,						         // input buffer size 
		0,									         // client time-out 
		NULL);							             //default security attribute 

	if (hPipe == INVALID_HANDLE_VALUE)
	{
		try
		{
			ThrowLastError("Failed to open Named Pipe.");
		}
		catch (const std::runtime_error err)
		{
			std::cout << "Runtime Error: " << err.what();
			return;
		}
	}
	int timeout = 100000;


	PROCESS_INFORMATION pi;
	ZeroMemory(&pi, sizeof(pi));
	STARTUPINFO si;
	ZeroMemory(&si, sizeof(si));
	std::string cmd = "Slave.exe " + pipename;
	int retnVal = CreateProcessA("Slave.exe", (LPSTR)cmd.c_str(), NULL, NULL, NULL, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi);
	if (!retnVal)
	{
		retnVal = GetLastError();
	}
	if (!ConnectNamedPipe(hPipe, NULL))
	{
		if (!GetLastError() == ERROR_PIPE_CONNECTED)
		{
			try
			{
				ThrowLastError("Error while connecting to named pipe.");
			}
			catch (std::runtime_error err)
			{
				std::cout << "GLE = " << GetLastError();
				Block();
				return ;
			}
		}
	}

	std::cout << "Connected to pipe.\n";
}
int PipeServer::WritePipe(char * buffer, int size)
{
	DWORD cbWritten;
	BOOL fSuccess = FALSE;
	fSuccess = WriteFile(
		hPipe,
		buffer,
		size,
		&cbWritten,
		NULL
	);
	if (!fSuccess || size != cbWritten)
	{
		ThrowLastError("Error! Can't write to pipe.\n");
		return -1;
	}
	return 0;
}
int PipeServer::ReadPipe(char * buffer, size_t size)
{
	BOOL fSuccess = false;
	DWORD bytesRead = 0;
	while (bytesRead < size)
	{
		fSuccess = ReadFile(
			hPipe,
			buffer,
			size,
			&bytesRead,
			NULL);
		if (!fSuccess || bytesRead == 0 || bytesRead != size)
		{
			try {
				if (GetLastError() == ERROR_BROKEN_PIPE)
				{
					std::cout << "Pipe disconnected...Quitting..";
					return 0;
				}
				else
				{
					ThrowLastError("Read Failed");
				}
			}
			catch (std::runtime_error err)
			{
				std::cout << err.what();
				std::cout << "Quit?(Y/N): ";
				char c;
				std::cin >> c;
				c = tolower(c);
				if (c == 'y')
				{
					delete this;
				}
				return 0;
			}
		}
	}
	return 0;
}
Header PipeServer::ReadHeader()
{
	Header HeaderVal;
	ReadPipe((char*)&HeaderVal, 4);
	return HeaderVal;
}
void PipeServer::ProcessHeader(Header header)
{
	switch (header)
	{
	case PONG: {
		break;
	}
	case PING: {
		Header Pong = Header::PONG;
		WritePipe((char *)&Pong, sizeof(Header));
	}

	}
}
void PipeServer::WriteHeader(Header p_Header)
{
	WritePipe((char*)&p_Header, sizeof(Header));
}
void PipeServer::operator <<(char * buffer)
{
	int size = 0;
	while (buffer[size])
	{
		size++;
	}
	size++;
	WriteHeader(Header::STRING_INFO);
	WritePipe(buffer, size);
}
void PipeServer::operator <<(std::string buffer)
{
	int size = buffer.length();
	WriteHeader(Header::STRING_INFO);
	WritePipe((char*)buffer.c_str(), size + 1);
}
PipeServer::~PipeServer()
{
	DisconnectNamedPipe(hPipe);
}