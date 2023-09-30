// Sam Collier 2023
#include "Core.h"
#include <exception>
#include <iostream>

int main()
{
	goop::Core core;

	try
	{
		core.run();
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
