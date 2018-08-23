#include<conio.h>
#include <Windows.h>
#include <iostream>
#include <sstream>
void ThrowLastError(std::string e);
void Block();
#define BUFFER_PIPE_SIZE 5120
void ThrowLastError(std::string e)
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
	std::cout << "\nPress Any Key To Continue...";
	char a = ' ';
	getchar();
}
class PipeClient
{
public:
	PipeClient()
	{
		Connect();
		if (hPipe != INVALID_HANDLE_VALUE || GetLastError() != ERROR_PIPE_BUSY)
		{
			std::cout << "Created Pipe, GLE=" << GetLastError();
		
		}
		if (hPipe == INVALID_HANDLE_VALUE)
		{
			ThrowLastError("Failed to connect to named pipe.");
		}
	}
	int Connect()
	{
		while (true)
		{
			WaitNamedPipeA(pipename, NMPWAIT_WAIT_FOREVER);
			hPipe = CreateFileA(
				pipename,
				GENERIC_READ |
				GENERIC_WRITE,
				0,
				NULL,
				OPEN_EXISTING,
				0,
				NULL
			);
			if (hPipe != INVALID_HANDLE_VALUE || GetLastError() != ERROR_PIPE_BUSY)
			{
				std::cout << "Created Pipe, GLE=" << GetLastError();
				break;
			}
		}
		return 0;
	}
	int ReadPipe(char * buffer, size_t size)
	{
		BOOL fSuccess = false;
		DWORD bytesRead = 0;
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
					ThrowLastError("pipe disconnected");
				}
				else
				{
					ThrowLastError("read failed");
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
	int WritePipe(char * buffer, size_t size)
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
private:
	DWORD inputBuffer = 256;
	DWORD outputBuffer = 256;
	HANDLE hPipe;
	char * pipename = "\\\\.\\pipe\\namedpipe";
};


int main()
{
	PipeClient pc;

	return 0;
}
