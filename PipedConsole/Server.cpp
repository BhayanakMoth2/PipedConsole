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

class Server
{
	Server()
	{
		OpenNamedPipe();
	}
	int OpenNamedPipe()
	{
		hPipe = CreateNamedPipe(
			"\\\\.\\pipe\\namedpipe",             // pipe name 
			PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED,       // read/write access 
			PIPE_TYPE_BYTE |
			PIPE_WAIT,  // blocking mode 
			1, // max. instances  
			outputBuffer,                  // output buffer size 
			inputBuffer,                  // input buffer size 
			0,                        // client time-out 
			NULL);                    // default security attribute 

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
private:
	DWORD inputBuffer = 256;
	DWORD outputBuffer = 256;
	HANDLE hPipe;
	char * pipename = "\\\\.\\pipe\\namedpipe";
};



void Throw_Last_Error(std::string str);
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
void Block()
{
	std::cout << "Press Any Key To Continue...";
}
int main()
{
	DWORD inputBuffer = 256;
	DWORD outputBuffer = 256;
	HANDLE _hPipe = CreateNamedPipe(
		"\\\\.\\pipe\\namedpipe",             // pipe name 
		PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED,       // read/write access 
		PIPE_TYPE_BYTE |
		PIPE_WAIT,  // blocking mode 
		1, // max. instances  
		outputBuffer,                  // output buffer size 
		inputBuffer,                  // input buffer size 
		0,                        // client time-out 
		NULL);                    // default security attribute 

	if (_hPipe == INVALID_HANDLE_VALUE)
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
	if (!ConnectNamedPipe(_hPipe, NULL))
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
	std::cout << "Writing to pipe...\n";

	DWORD cbWritten;
	BOOL fSuccess = FALSE;
	char  buffer[256] = "Test 123";
	size_t size = sizeof(buffer);
	std::cout << buffer;
	Block();
	return 0;
}