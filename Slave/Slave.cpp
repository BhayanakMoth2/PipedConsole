#include <conio.h>
#include <Windows.h>
#include <iostream>
#include <sstream>
#include "ErrorHandling.hpp"
#include "PipeClient.hpp"
int main(int argc, char *argv[])
{
	std::string argstr = argv[1];
		try
		{
			PipeClient pc(argstr);
			pc.Update();
		}
		catch (std::runtime_error err)
		{
			std::cout << "Unknown exception: "<<err.what();
			system("pause");
		}
	system("pause");
	return 0;
}
