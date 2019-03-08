#include <conio.h>
#include <Windows.h>
#include <iostream>
#include <sstream>
#include "ErrorHandling.hpp"
void ThrowLastError(std::string e);
void Block();
#define BUFFER_PIPE_SIZE 5120
int main(int argc, char *argv[])
{
	std::string argstr = " ";
		try
		{
			argstr = argv[1];
			PipeClient pc(argstr);
			PipeClient pc2(argstr);
			pc.Update();
			pc2.Update();
		
		}
		catch (...)
		{
			std::cout << "Unknown exception: ";
			system("pause");
		}
	system("pause");
	return 0;
}
