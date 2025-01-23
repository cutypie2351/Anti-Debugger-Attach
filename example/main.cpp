#include "anti_attach.hpp"
#include <iostream>


int main(int argc, char* argv[])
{
	Anti_Attaching* anti_attach = new Anti_Attaching();
	anti_attach->Initialize(argc, argv);

	std::cout << "process started!" << std::endl;

	// your code here:
	while (true)
	{
		Sleep(1000);
	}
	std::cout << "process ended!" << std::endl;
	delete anti_attach;
	return 0;
}