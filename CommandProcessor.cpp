#include "CommandProcessor.h"
#include <iostream>
#include <stdexcept>
#include <fstream>
#include <cstdlib>
#include "CeasarCipher.h"
#include "TextCodeCipher.h" 
#include "HillCipher.h"
#include "CipherFactory.h"

CommandProcessor::CommandProcessor() : passwordManager(nullptr) {} //not much more that we need to do here
CommandProcessor::~CommandProcessor() 
{
	delete passwordManager;
	passwordManager = nullptr;
}

void CommandProcessor::showHelp() const 
{
    std::cout << "\n=== Password Manager Help ===" << std::endl;
    std::cout << "\nFile Operations:" << std::endl;
    std::cout << "  create <filename> <cipher> <password> [cipher-params]" << std::endl;
    std::cout << "    Create a new password file with specified cipher" << std::endl;
    std::cout << "    Ciphers: caesar <shift>, textcode <textfile>, hill <matrix-size>" << std::endl;
    std::cout << "    Example: create mypass.dat caesar mykey123 3" << std::endl;
    std::cout << "\n  open <filename> <password>" << std::endl;
    std::cout << "    Open an existing password file" << std::endl;
    std::cout << "    Example: open mypass.dat mykey123" << std::endl;

    std::cout << "\nPassword Operations: " << std::endl;
    std::cout << "  save <website> <user> <password>" << std::endl;
    std::cout << "    Save a password for a website and user" << std::endl;
    std::cout << "    Example: save gmail.com john@email.com \"my secure pass\"" << std::endl;
    std::cout << "\n  load <website> [<user>]" << std::endl;
    std::cout << "    Load password(s) for a website" << std::endl;
    std::cout << "    Example: load gmail.com john@email.com" << std::endl;
    std::cout << "    Example: load gmail.com (shows all users)" << std::endl;
    std::cout << "\n  update <website> <user> <new-password>" << std::endl;
    std::cout << "    Update an existing password" << std::endl;
    std::cout << "    Example: update gmail.com john@email.com \"new password\"" << std::endl;
    std::cout << "\n  delete <website> [<user>]" << std::endl;
    std::cout << "    Delete password(s) for a website" << std::endl;
    std::cout << "    Example: delete gmail.com john@email.com" << std::endl;
    std::cout << "    Example: delete gmail.com (deletes all users)" << std::endl;

    std::cout << "\nGeneral:" << std::endl;
    std::cout << "  help    - Show this help message" << std::endl;
    std::cout << "  exit    - Exit the application" << std::endl;
    std::cout << "\nNote: Use quotes around passwords/arguments containing spaces" << std::endl;
    std::cout << "================================\n" << std::endl;
}
bool CommandProcessor::hasOpenFile() const {
    return passwordManager && passwordManager->getIsFileOpen();
}

std::vector<std::string> CommandProcessor::parseCommand(const std::string& commandLine) const 
{
	if (commandLine.empty()) {
		return {};
	}

    std::vector<std::string> tokens;

    bool inQuotes = false;
    std::string currentToken;

    for (size_t i = 0; i < commandLine.length(); ++i)
    {
        char c = commandLine[i];

        if (c == '"' && !inQuotes)
        {
            inQuotes = true;
        }
        else if (c == '"' && inQuotes)
        {
            inQuotes = false;
            if (!currentToken.empty())
            {
                tokens.push_back(currentToken);
                currentToken.clear();
            }
        }
        else if (c == ' ' && !inQuotes)
        {
            if (!currentToken.empty())
            {
                tokens.push_back(currentToken);
                currentToken.clear();
            }
        }
        else {
            currentToken += c;
        }
    }

    if (!currentToken.empty())
    {
        tokens.push_back(currentToken);
    }

    return tokens;
}
void CommandProcessor::processCommand(const std::string& commandLine)
{
    if (commandLine.empty())
    {
        return;
    }

    std::vector<std::string> args = parseCommand(commandLine);
    if (args.empty())
    {
        return;
    }

    std::string command = args[0];

    // case-insensitive 
    for (size_t i = 0; i < command.length(); ++i)
    {
        if (command[i] >= 'A' && command[i] <= 'Z')
        {
            command[i] = command[i] + ('a' - 'A');
        }
    }

    try {
        if (command == "create")
        {
            handleCreateCommand(args);
        }
        else if (command == "open")
        {
            handleOpenCommand(args);
        }
        else if (command == "save")
        {
            handleSaveCommand(args);
        }
        else if (command == "load")
        {
            handleLoadCommand(args);
        }
        else if (command == "update")
        {
            handleUpdateCommand(args);
        }
        else if (command == "delete")
        {
            handleDeleteCommand(args);
        }
        else
        {
            throw std::invalid_argument("Unknown command: " + command);
        }
    }
    catch (const std::exception& e)
    {
        throw; // Re-throw for the main
    }
}


void CommandProcessor::validateArguments(const std::vector<std::string>& args, size_t expectedMin, size_t expectedMax) const 
{
    if (args.size() < expectedMin) 
    {
        throw std::invalid_argument("Insufficient arguments for command");
    }
    if (expectedMax != SIZE_MAX && args.size() > expectedMax)
    {
        throw std::invalid_argument("Too many arguments for command");
    }
}
bool CommandProcessor::isValidCipherType(const std::string& cipherType) const
{
    return (cipherType == "caesar" || cipherType == "textcode" || cipherType == "hill");
}

//int CommandProcessor::parsePositiveInteger(const std::string& str, const std::string& paramName, int minVal, int maxVal) const
//{
//    if (str.empty())
//    {
//        throw std::invalid_argument(paramName + " cannot be empty");
//    }
//
//    if (str[0] == '-' || str[0] == '+')
//    {
//        throw std::invalid_argument(paramName + " must be a positive number without sign");
//    }
//
//    if (str.length() > 1 && str[0] == '0')
//    {
//        throw std::invalid_argument(paramName + " cannot have leading zeros");
//    }
//
//    long long result = 0;
//    for (char c : str) {
//        if (c < '0' || c > '9') 
//        {
//            throw std::invalid_argument(paramName + " must contain only digits");
//        }
//
//        long long newResult = result * 10 + (c - '0');
//        if (newResult > INT_MAX) 
//        {
//            throw std::invalid_argument(paramName + " is too large");
//        }
//        result = newResult;
//    }
//
//    int value = static_cast<int>(result);
//    if (value < minVal || value > maxVal)
//    {
//        throw std::invalid_argument(paramName + " must be between " +
//            std::to_string(minVal) + " and " + std::to_string(maxVal));
//    }
//
//    return value;
//}
//void CommandProcessor::validateFilePath(const std::string& filePath, const std::string& cipherName) const
//{
//    if (filePath.empty()) 
//    {
//        throw std::invalid_argument(cipherName + " requires a non-empty file path");
//    }
//
//    std::ifstream testFile(filePath);
//    if (!testFile.is_open()) 
//    {
//        throw std::runtime_error("Cannot open file for " + cipherName + ": " + filePath);
//    }
//    testFile.close();
//}

Cipher* CommandProcessor::createCipher(const std::string& cipherType, const std::vector<std::string>& cipherParams) const
{
    /*if (cipherType == "Caesar") 
    {
        return createCaesarCipher(cipherParams);
    }
    else if (cipherType == "Textcode")
    {
        return createTextCodeCipher(cipherParams);
    }
    else if (cipherType == "Hill")
    {
        return createHillCipher(cipherParams);
    }
    else
    {
        throw std::invalid_argument("Unknown cipher type: " + cipherType);
    }*/

    Cipher* newCipher = CipherFactory::createCipher(cipherType, cipherParams);

    if (passwordManager != nullptr && newCipher != nullptr)
    {
        Cipher* oldCipher = passwordManager->getFileCipher();

        passwordManager->setFileCipher(newCipher);

		// delete the old cipher to avoid memory leaks
        if (oldCipher != nullptr)
        {
            delete oldCipher;
            oldCipher = nullptr;
        }
    }

    return newCipher;
}

void CommandProcessor::validateFileAccess(const std::string& filename) const
{
    if (filename.empty()) {
        throw std::invalid_argument("Filename cannot be empty");
    }

    const std::string invalidChars = "<>:\"|?*";
    for (char c : invalidChars)
    {
        if (filename.find(c) != std::string::npos)
        {
            throw std::invalid_argument("Invalid character in filename: " + std::string(1, c));
        }
    }
}

void CommandProcessor::handleCreateCommand(const std::vector<std::string>& args)
{
    // create <filename> <cipher> <password> [cipher-params...]
    validateArguments(args, 4, SIZE_MAX);
	//args[0] is the command, args[1] is the filename, args[2] is the cipher type, args[3] is the file password
    std::string filename = args[1];
    std::string cipherType = args[2];
    std::string filePassword = args[3];

    validateFileAccess(filename);

    if (!isValidCipherType(cipherType))
    {
        throw std::invalid_argument("Invalid cipher type. Supported: caesar, textcode, hill");
    }

	// Additional parameters for the cipher
    std::vector<std::string> cipherParams;
    for (size_t i = 4; i < args.size(); ++i) 
    {
        cipherParams.push_back(args[i]);
    }

	// Creating the cipher based on the type and parameters
    Cipher* cipher = nullptr;
    try
    {
        cipher = createCipher(cipherType, cipherParams);
    }
    catch (const std::exception& e) 
    {
        delete cipher; // Cleanup
        throw;
    }

	// Create a new PasswordManager instance
    delete passwordManager;
    passwordManager = new PasswordManager();

    try 
    {
        passwordManager->createFile(filename, cipher, filePassword);
    }
    catch (const std::exception& e)
    {
        delete cipher; // Cleanup 
        delete passwordManager;
        passwordManager = nullptr;
        throw;
    }

    std::cout << "Password file '" << filename << "' created successfully with " << cipherType << " cipher." << std::endl;
}
void CommandProcessor::handleOpenCommand(const std::vector<std::string>& args)
{
    // open <filename> <password>
    validateArguments(args, 3, 3);
    std::string filename = args[1];
    std::string filePassword = args[2];
    validateFileAccess(filename);

    std::ifstream testFile(filename);
    if (!testFile.is_open())
    {
        throw std::runtime_error("Cannot open password file: " + filename);
    }
	

    testFile.close();
 

    delete passwordManager; 
    passwordManager = new PasswordManager();
    passwordManager->openFile(filename, filePassword);

    std::cout << "Password file '" << filename << "' opened successfully." << std::endl;
}
void CommandProcessor::handleSaveCommand(const std::vector<std::string>& args)
{
    // save <website> <user> <password>
    validateArguments(args, 4, 4);

    if (!passwordManager || !passwordManager->getIsFileOpen()) 
    {
        throw std::runtime_error("No password file is currently open. Use 'create' or 'open' command first.");
    }

    std::string website = args[1];
    std::string user = args[2];
    std::string password = args[3];

    if (website.empty() || user.empty() || password.empty())
    {
        throw std::invalid_argument("Website, user, and password cannot be empty");
    }

    passwordManager->addPassword(website, user, password);
    std::cout << "Password saved successfully for " << user << "@" << website << std::endl;
}
void CommandProcessor::handleLoadCommand(const std::vector<std::string>& args)
{
    // load <website> [<user>]
    validateArguments(args, 2, 3);

    if (!passwordManager || !passwordManager->getIsFileOpen()) 
    {
        throw std::runtime_error("No password file is currently open. Use 'create' or 'open' command first.");
    }

    std::string website = args[1];
    if (website.empty())
    {
        throw std::invalid_argument("Website cannot be empty");
    }

    if (args.size() == 3)
    {
		// Specific user 
        std::string user = args[2];
        if (user.empty()) 
        {
            throw std::invalid_argument("User cannot be empty");
        }

		PasswordEntry* entry = passwordManager->findPassword(website, user);

        std::string decryptedPassword = passwordManager->getFileCipher()->decrypt(entry->getPassword());
		std::cout << "Password for " << user << "@" << website << ": " << decryptedPassword << std::endl; // this shows the decrypted password

		//std::cout << "Password for " << user << "@" << website << ": " << entry->getPassword() << std::endl; //this shows the encrypted password
    }
    else
    {
		// Everty user for the website
        auto users = passwordManager->loadAllUsers(website);
        if (users.empty()) 
        {
            std::cout << "No passwords found for website: " << website << std::endl;
        }
        else 
        {
            std::cout << "Passwords for " << website << ":" << std::endl;
            for (const auto& userPass : users) 
            {
                std::string decryptedPassword = passwordManager->getFileCipher()->decrypt(userPass.getPassword());
				std::cout << "  " << userPass.getUsername() << ": " << decryptedPassword << std::endl; // this shows the decrypted password

				//std::cout << "  " << userPass.getUsername() << ": " << userPass.getPassword() << std::endl; // this shows the encrypted password
            }
        }
    }
}
void CommandProcessor::handleUpdateCommand(const std::vector<std::string>& args)
{
	// update <website> <user> <new-password>
	validateArguments(args, 4, 4);
	if (!passwordManager || !passwordManager->getIsFileOpen())
	{
		throw std::runtime_error("No password file is currently open. Use 'create' or 'open' command first.");
	}
	std::string website = args[1];
	std::string user = args[2];
	std::string newPassword = args[3];
	if (website.empty() || user.empty() || newPassword.empty())
	{
		throw std::invalid_argument("Website, user, and new password cannot be empty");
	}
	if (!passwordManager->updatePassword(website, user, newPassword))
	{
		throw std::runtime_error("Password not found for " + user + "@" + website);
	}
	std::cout << "Password updated successfully for " << user << "@" << website << std::endl;
}
void CommandProcessor::handleDeleteCommand(const std::vector<std::string>& args)
{
    // delete <website> [<user>]
    validateArguments(args, 2, 3);

    if (!passwordManager || !passwordManager->getIsFileOpen())
    {
        throw std::runtime_error("No password file is currently open. Use 'create' or 'open' command first.");
    }

    std::string website = args[1];
    if (website.empty())
    {
        throw std::invalid_argument("Website cannot be empty");
    }

    if (args.size() == 3) 
    {
     
        std::string user = args[2];
        if (user.empty()) {
            throw std::invalid_argument("User cannot be empty");
        }

        passwordManager->deletePassword(website, user);
        std::cout << "Password deleted successfully for " << user << "@" << website << std::endl;
    }
    else 
    {
        int deletedCount = passwordManager->deletePasswordsByWebsite(website);
        std::cout << "Deleted " << deletedCount << " password(s) for website: " << website << std::endl;
    }
}