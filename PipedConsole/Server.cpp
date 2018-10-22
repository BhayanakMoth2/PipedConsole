#include<Windows.h>
#include<time.h>
#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<io.h>
#include<direct.h>
#include<iostream>
#include<sstream>
#define BUFFER_PIPE_SIZE 5120
void Throw_Last_Error(std::string str);
void Block()
{
	std::cout << "Press Any Key To Continue...";
}
class Server
{
public:
	Server(std::string pipename)
	{
		OpenNamedPipe(pipename);
	}
	int OpenNamedPipe(std::string pipename)
	{

		pipeurl += pipename;
		hPipe = CreateNamedPipe(
			pipeurl .c_str(),				             // pipe name 
			PIPE_ACCESS_DUPLEX,   // read/write access 
			PIPE_TYPE_BYTE |							 // Datatype Byte
			PIPE_WAIT,								     // blocking mode 
			1,									         // max. instances  
			outputBuffer,								 // output buffer size 
			inputBuffer,						         // input buffer size 
			0,									// client time-out 
			NULL);							   // default security attribute 

		if (hPipe == INVALID_HANDLE_VALUE)
		{
			try
			{
				Throw_Last_Error("CreateNamedPipe failed");
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
					std::cout << "GLE= " << GetLastError();
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
	void operator <<(char * buffer)
	{
		int size = 0;
		while (buffer[size])
		{
			size++;
		}
		size++;
		WritePipe(buffer, size);
	}
	void operator <<(std::string buffer)
	{
		int size = buffer.length();
		WritePipe((char*)buffer.c_str(), size);
	}
private:
	DWORD inputBuffer = 256;
	DWORD outputBuffer = 256;
	HANDLE hPipe;
	std::string pipeurl = "\\\\.\\pipe\\";
};



int WritePipe(char * buff, int size, HANDLE hPipe);
int ReadPipe(char * buffer, size_t size, HANDLE hPipe);
int WritePipeVoid(void * buff, int size, HANDLE hPipe);
int WritePipeVoid(void * buff, int size, HANDLE hPipe)
{
	DWORD cbWritten;
	BOOL fSuccess = FALSE;
	fSuccess = WriteFile(
		hPipe,
		(LPCVOID)buff,
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
int WritePipe(char * buffer, int size, HANDLE hPipe)
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
int ReadPipe(char * buffer, size_t size, HANDLE hPipe)
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
void Throw_Last_Error(std::string e)
{
	int error = GetLastError();
	std::string err;
	err = e + ", GLE=" + std::to_string(error);
	try {
		throw std::runtime_error(err);
	}
	catch (const std::runtime_error e)
	{
		std::cout << "RUN TIME EXCEPTION: " << std::hex << e.what();
		Block();
	}
}

int main()
{
	Server server =Server("namedpipe");
	char buffer[100] = "Mic Check Mic Check!";
	server.WritePipe(buffer, 100);
	std::string strbuffer = " ";
	while (true)
	{
		std::cout << "Enter: ";
		std::cin >> strbuffer;
		server << strbuffer;
	}
	return 0;
}