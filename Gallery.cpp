#include <iostream>
#include <string>
#include <chrono>
#include "MemoryAccess.h"
#include "sql3DataAccess.h"
#include "AlbumManager.h"
#include <ctime> 

void PrintStart();
int getCommandNumberFromUser()
{
	std::string message("\nPlease enter any command(use number): ");
	std::string numericStr("0123456789");
	
	std::cout << message << std::endl;
	std::string input;
	std::getline(std::cin, input);
	
	while (std::cin.fail() || std::cin.eof() || input.find_first_not_of(numericStr) != std::string::npos) {

		std::cout << "Please enter a number only!" << std::endl;

		if (input.find_first_not_of(numericStr) == std::string::npos) {
			std::cin.clear();
		}

		std::cout << std::endl << message << std::endl;
		std::getline(std::cin, input);
	}
	
	return std::atoi(input.c_str());
}

int main(void)
{
	// initialization data access
	sql3DataAccess dataAccess;

	// initialize album manager
	AlbumManager albumManager(dataAccess);

	PrintStart();
	std::string albumName;
	
	do {
		int commandNumber = getCommandNumberFromUser();
		
		try	{
			albumManager.executeCommand(static_cast<CommandType>(commandNumber));
		} catch (std::exception& e) {	
			std::cout << e.what() << std::endl;
		}
	} 
	while (true);
}


void PrintStart() {
	std::cout << "Welcome to Gallery By Maayan Yogev!" << std::endl;
	auto timenow = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	std::cout << ctime(&timenow) << std::endl;
	std::cout << "===================" << std::endl;
	std::cout << "Type " << HELP << " to a list of all supported commands" << std::endl;

}