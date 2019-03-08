#pragma once
#include "PipeClient.hpp"

PipeClient::PipeClient(std::string p_PipeName)
{
	m_PipeName = m_PipeName + p_PipeName;
	Connect();
	if (hPipe != INVALID_HANDLE_VALUE || GetLastError() != ERROR_PIPE_BUSY)
	{
		std::cout << "Created Pipe, GLE = " << GetLastError();

	}
	if (hPipe == INVALID_HANDLE_VALUE)
	{
		ThrowLastError("Failed to connect to named pipe.");
	}
}
int PipeClient::Connect()
{
	while (true)
	{
		WaitNamedPipeA(m_PipeName.c_str(), NMPWAIT_WAIT_FOREVER);
		hPipe = CreateFileA(
			m_PipeName.c_str(),
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
			std::cout << "Created Pipe, GLE = " << GetLastError();
			break;
		}
	}
	return 0;
}
int PipeClient::ReadPipe(char * buffer, size_t size)
{
	BOOL fSuccess = false;
	DWORD bytesRead = 0;
	fSuccess = ReadFile(
		hPipe,
		buffer,
		size,
		&bytesRead,
		NULL);
	if (!fSuccess)
	{
		try {
			if (GetLastError() == ERROR_BROKEN_PIPE)
			{
				ThrowLastError("Pipe Disconnected");
				hPipe = NULL;
				return -1;
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
			return -1;
		}
	}
	return bytesRead;
}
int PipeClient::WritePipe(char * buffer, size_t size)
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
int PipeClient::ConsoleOut(std::string message)
{
	char * buffer = new char[message.length()];//<--Destroy this
	WritePipe(buffer, message.length());
	delete buffer;
}
void PipeClient::Update()
{
	std::cout << "\n";
	while (true)
	{
		char buffer[256] = " ";
		int retnVal = ReadPipe(buffer, inputBuffer);
		if (buffer != "")
		{
			std::cout << "\n";
			std::cout << buffer;
		}
		if (retnVal < 0)
			break;
	}
	std::cout << "\nQuitting...";
}
PipeClient::~PipeClient()
{
	BOOL retn = DisconnectNamedPipe(hPipe);
	if (retn == FALSE)
	{
		ThrowLastError("Failed to close pipe.");
	}
}