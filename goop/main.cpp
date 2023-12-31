// Sam Collier 2023
#include "Core.h"
#include <exception>
#include <iostream>

int main(int argc, char** argv)
{
	goop::Core core(argc, argv);

	core.run();

	return EXIT_SUCCESS;
}
