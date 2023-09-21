#include <thread>
#include <iostream>
#include "rust_lib/rustlib.h"

int main()
{
	std::cout << "Calling rust function...\n";
	std::thread t(say_hello);
	std::cout << "Doing cpp stuff..\n";
	t.join();
	return 0;
}
