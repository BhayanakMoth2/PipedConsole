#include<Windows.h>
#include<time.h>
#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<io.h>
#include<direct.h>
#include<iostream>
#include<sstream>
#define BUFFERSIZE 1024
void Throw_Last_Error(std::string str);
void Block()
{
	std::cout << "Press Any Key To Continue...";
	getchar();
}
enum Header
{
	STRING_INFO,
	STRING_WARN,
	PING,
	PONG,
	EXIT
};
class ConsoleServer
{
public:
	ConsoleServer(std::string pipename)
	{
		OpenNamedPipe(pipename);
	}
	int OpenNamedPipe(std::string pipename)
	{

		pipeurl += pipename;
		hPipe = CreateNamedPipe(
			pipeurl .c_str(),				             // pipe name 
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
				Throw_Last_Error("Failed to open Named Pipe.");
			}
			catch (const std::runtime_error err)
			{
				std::cout << "Runtime Error: " << err.what();
				return 0;
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
					Throw_Last_Error("Error while connecting to named pipe.");
				}
				catch (std::runtime_error err)
				{
					std::cout << "GLE = " << GetLastError();
					Block();
					return 0;
				}
			}
		}

		std::cout << "Connected to pipe.\n";
	}
	int WritePipe(char * buffer, int size)
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
			Throw_Last_Error("Error! Can't write to pipe.\n");
			return -1;
		}
		return 0;
	}
	int ReadPipe(char * buffer, size_t size)
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
						Throw_Last_Error("pipe disconnected");
					}
					else
					{
						Throw_Last_Error("read failed");
					}
				}
				catch (std::runtime_error err)
				{
					std::cout << err.what();
					std::cout << "\nQuitting...";
					return 0;
				}
			}
		}
		return 0;

	}
	Header ReadHeader()
	{
		Header HeaderVal;
		char * buffer = new char[4];
		ReadPipe(buffer, 4);

	}
	void ProcessHeader(Header header)
	{
		switch (header)
		{
		case PONG: {
			break;
		}
		case PING:{
				Header Pong = Header::PONG;
				WritePipe((char *)&Pong, sizeof(Header));
			}
	
		}
	}
	void WriteHeader(Header p_Header)
	{
		WritePipe((char*)&p_Header, sizeof(Header));
	}
	void operator <<(char * buffer)
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
	void operator <<(std::string buffer)
	{
		int size = buffer.length();
		WriteHeader(Header::STRING_INFO);
		WritePipe((char*)buffer.c_str(), size+1);
	}
private:
	const size_t m_BufferSize = BUFFERSIZE;
	HANDLE hPipe;
	std::string pipeurl = "\\\\.\\pipe\\";
};

void Throw_Last_Error(std::string e)
{
	int error = GetLastError();
	std::string err;
	err = e + ", GLE = " + std::to_string(error);
	try {
		throw std::runtime_error(err);
	}
	catch (const std::runtime_error e)
	{
		std::cout << "RUN TIME EXCEPTION: " << std::hex << e.what()<<"\n";
		Block();
	}
}

int main()
{
	ConsoleServer server = ConsoleServer("namedpipe");
	char buffer[100] = "Mic Check Mic Check!";
	server << buffer;
	std::string strbuffer = " ";
	while (true)
	{
		std::cout << "Enter: ";
		std::getline(std::cin, strbuffer);
		server << strbuffer;
	}
	return 0;
}