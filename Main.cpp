#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
#include "PasswordManager.h"
#include "CommandProcessor.h"

//fileCipher constructor - add validations for null/invalid data
//encapsulation - validation for set, add const
//йерархия за командите
//работа на блокове
//ооп принципи виж пак
//работа на паметта
//lifecycle management
//make encrypt dectypt const
//simple encriptor for the whole file, like the simpleEncryptDecrypt


int main()
{
    try 
    {
        CommandProcessor commandProcessor;

        std::cout << "Password Manager - Enter 'help' for available commands or 'exit' to quit" << std::endl;

        std::string commandLine;
        while (true) 
        {
            std::cout << "\n" << ">> ";
            std::getline(std::cin, commandLine);

            if (commandLine == "exit") 
            {
                break;
            }
            else if (commandLine == "help") 
            {
                commandProcessor.showHelp();
            }
            else {
                try {
                    commandProcessor.processCommand(commandLine);
                }
                catch (const std::exception& e)
                {
                    std::cerr << "Error: " << e.what() << std::endl;
                }
            }
        }

        return 0;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }
}