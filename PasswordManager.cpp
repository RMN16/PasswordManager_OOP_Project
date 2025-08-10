#include "PasswordManager.h"
#include "CeasarCipher.h"
#include "TextCodeCipher.h"
#include "HillCipher.h"
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <cstring>

PasswordManager::PasswordManager() : fileCipher(nullptr), isFileOpen(false) {}
PasswordManager::~PasswordManager()
{
	if (isFileOpen)
	{
		saveToFile();
	}
	delete fileCipher;
	fileCipher = nullptr;
}


void PasswordManager::setFileCipher(Cipher* cipher)
{
	if (cipher == nullptr)
	{
		throw std::invalid_argument("Cipher cannot be null.");
	}
	delete fileCipher; // Delete the old cipher to avoid memory leak

	fileCipher = cipher->clone(); // Clone the cipher to ensure we have a separate instance
	
}


void PasswordManager::createFile(const std::string& filename, Cipher* cipher, const std::string& masterPassword)
{
	if (isFileOpen) 
	{
		throw std::runtime_error("A file is already open.");
	}
	if (!cipher)
	{
		throw std::invalid_argument("Cipher cannot be null.");
	}
	if (filename.empty() || masterPassword.empty())
	{
		throw std::invalid_argument("Filename and master password cannot be empty.");
	}

	this->filename = filename;
	this->masterPassword = masterPassword;

	delete fileCipher; // Ensure any previous cipher is deleted
	this->fileCipher = cipher;
	passwords.clear(); // Start with an empty password list
	this->isFileOpen = true;
	
	saveToFile();

	std::cout << "File created successfully: " << filename << std::endl;
}
void PasswordManager::openFile(const std::string& filename, const std::string& masterPassword)
{
	if (isFileOpen)
	{
		throw std::runtime_error("A file is already open.");
	}
	if (filename.empty() || masterPassword.empty())
	{
		throw std::invalid_argument("Filename and master password cannot be empty.");
	}
	/*if (!fileCipher)
	{
		throw std::runtime_error("Cipher must be set before opening a file.");
	}*/ // loadFromFile will handle this


	this->filename = filename;
	this->masterPassword = masterPassword;
	this->isFileOpen = true;
	
	try
	{
		loadFromFile();
		std::cout << "File opened successfully: " << filename << std::endl;
	}
	catch (const std::exception& e)
	{
		isFileOpen = false;
		this->filename.clear();
		this->masterPassword.clear();
		delete fileCipher;
		fileCipher = nullptr;
		passwords.clear();
		throw std::runtime_error("Failed to open file: " + std::string(e.what()));
	}
}

void PasswordManager::addPassword(const std::string& website, const std::string& username, const std::string& password)
{
	if (!isFileOpen)
	{
		throw std::runtime_error("No file is open.");
	}
	if (website.empty() || username.empty() || password.empty())
	{
		throw std::invalid_argument("Website, username, and password cannot be empty.");
	}
	if (findPassword(website, username) != nullptr)
	{
		throw std::runtime_error("Password for this website and username already exists.");
	}

	std::string encryptedPassword;
	try 
	{
		encryptedPassword = fileCipher->encrypt(password);
	}
	catch (const std::exception& e) 
	{
		throw;
	}

	PasswordEntry newEntry(website, username, encryptedPassword);
	passwords.push_back(newEntry);
	saveToFile();
	std::cout << "Password added for website: " << website << "(user: " << username << ")" << std::endl;
}
PasswordEntry* PasswordManager::findPassword(const std::string& website, const std::string& username)
{
	if (!isFileOpen)
	{
		throw std::runtime_error("No file is open.");
	}
	if (website.empty() || username.empty())
	{
		throw std::invalid_argument("Website and username cannot be empty.");
	}
	for (auto& entry : passwords)
	{
		if (entry.getWebsite() == website && entry.getUsername() == username)
		{
			return &entry;
		}
	}
	return nullptr;
}
std::vector<PasswordEntry*> PasswordManager::findPasswordsByWebsite(const std::string& website)
{
	if (!isFileOpen)
	{
		throw std::runtime_error("No file is open.");
	}
	if (website.empty())
	{
		throw std::invalid_argument("Website cannot be empty.");
	}
	std::vector<PasswordEntry*> results;
	for (auto& entry : passwords)
	{
		if (entry.getWebsite() == website)
		{
			results.push_back(&entry);
		}
	}
	return results;
}
bool PasswordManager::updatePassword(const std::string& website, const std::string& username, const std::string& newPassword)
{
	if (!isFileOpen)
	{
		throw std::runtime_error("No file is open.");
	}
	if (website.empty() || username.empty() || newPassword.empty())
	{
		throw std::invalid_argument("Website, username, and new password cannot be empty.");
	}
	PasswordEntry* entry = findPassword(website, username);
	if (!entry)
	{
		return false;
	}

	std::string encryptedOldPassword = entry->getPassword();
	std::string decryptedOldPassword = fileCipher->decrypt(encryptedOldPassword);

	if (decryptedOldPassword == newPassword)
	{
		throw std::runtime_error("New password is the same as the old one");
	}

	//Encrypt the new password before setting it
	std::string encryptedNewPassword = fileCipher->encrypt(newPassword);
	entry->setPassword(encryptedNewPassword);

	saveToFile();
	std::cout << "Password updated for website: " << website << " (user: " << username << ")" << std::endl;
	return true;
}
bool PasswordManager::deletePassword(const std::string& website, const std::string& username)
{
	if (!isFileOpen)
	{
		throw std::runtime_error("No file is open.");
	}
	if (website.empty() || username.empty())
	{
		throw std::invalid_argument("Website and username cannot be empty.");
	}

	for (auto it = passwords.begin(); it != passwords.end(); ++it)
	{
		if (it->getWebsite() == website && it->getUsername() == username)
		{
			passwords.erase(it);
			saveToFile();
			std::cout << "Password deleted for website: " << website << "(user: " << username << ")" << std::endl;
			return true;
		}
	}
	return false;
}
int PasswordManager::deletePasswordsByWebsite(const std::string& website)
{
	if (!isFileOpen)
	{
		throw std::runtime_error("No file is open.");
	}
	if (website.empty())
	{
		throw std::invalid_argument("Website cannot be empty.");
	}
	int deletedCount = 0;
	auto it = passwords.begin();

	while (it != passwords.end())
	{
		if (it->getWebsite() == website)
		{
			it = passwords.erase(it);
			deletedCount++;
		}
		else
		{
			++it;
		}
	}

	if (deletedCount > 0)
	{
		saveToFile();
		std::cout << "Deleted " << deletedCount << " entries for website: " << website << std::endl;
	}

	return deletedCount;
}

bool PasswordManager::isOpen() const
{
	return isFileOpen;
}

std::vector<PasswordEntry> PasswordManager::loadAllUsers(const std::string& website) const
{
	if (!isFileOpen) 
	{
		throw std::runtime_error("No password file is currently open");
	}

	if (website.empty()) 
	{
		throw std::invalid_argument("Website cannot be empty");
	}

	std::vector<PasswordEntry> userEntries;
	userEntries.reserve(passwords.size());

	for (const auto& entry : passwords) 
	{
		if (entry.getWebsite() == website) 
		{
			userEntries.push_back(entry);
		}
	}

	return userEntries;
}

//helper functions
int stringToInt(const std::string& str)
{
	int result = 0;
	bool negative = false;
	size_t i = 0;

	if (str[0] == '-')
	{
		negative = true;
		i = 1;
	}

	for (; i < str.length(); ++i)
	{
		if (str[i] >= '0' && str[i] <= '9')
		{
			result = result * 10 + (str[i] - '0');
		}
		else
		{
			throw std::invalid_argument("Invalid integer string");
		}
	}

	return negative ? -result : result;
}
std::string intToString(int value)
{
	if (value == 0) return "0";

	std::string result;
	bool negative = false;

	if (value < 0)
	{
		negative = true;
		value = -value;
	}

	while (value > 0)
	{
		result = char('0' + (value % 10)) + result;
		value /= 10;
	}

	if (negative)
	{
		result = "-" + result;
	}

	return result;
}
std::vector<std::string> splitString(const std::string& str, char delimiter)
{
	std::vector<std::string> result;
	std::string current;

	for (char c : str)
	{
		if (c == delimiter)
		{
			if (!current.empty())
			{
				result.push_back(current);
				current.clear();
			}
		}
		else
		{
			current += c;
		}
	}

	if (!current.empty())
	{
		result.push_back(current);
	}

	return result;
}
std::string simpleEncryptDecrypt(const std::string& text, const std::string& key) {
	std::string result = text;
	for (size_t i = 0; i < result.length(); ++i) {
		result[i] ^= key[i % key.length()];
	}
	return result;
}


void PasswordManager::saveToFile() const 
{
	if (!isFileOpen)
	{
		throw std::runtime_error("No file is open.");
	}

	// Create content to save
	std::string content = "CIPHER_TYPE:" + fileCipher->getType() + "\n";
	content += "CIPHER_CONFIG:" + fileCipher->getConfig() + "\n";
	content += "ENTRIES:\n";

	for (const auto& entry : passwords) 
	{
		content += entry.getWebsite() + "|" + entry.getUsername() + "|" + entry.getPassword() + "\n";
	}

	// Encrypt content using simple XOR with master password
	std::string encryptedContent = simpleEncryptDecrypt(content, masterPassword);

	// Write to file
	std::ofstream file(filename.c_str());
	if (!file.is_open()) 
	{
		throw std::runtime_error("Cannot write to file: " + filename);
	}

	file << encryptedContent;
	file.close();
}

// Helper function to parse Hill cipher 
std::string parseHillMatrixString(const std::string& config)
{
	size_t matrixPos = config.find("Key matrix: ");
	if (matrixPos == std::string::npos)
	{
		throw std::runtime_error("Invalid Hill cipher config format");
	}

	return config.substr(matrixPos + 12); // 12 is Length of "Key matrix: "
}
int parseHillMatrixSize(const std::string& config)
{
	size_t sizePos = config.find("Matrix size: ");
	if (sizePos == std::string::npos)
	{
		throw std::runtime_error("Invalid Hill cipher config format");
	}

	size_t startPos = sizePos + 13; // Length of "Matrix size: "
	size_t endPos = config.find(",", startPos);
	if (endPos == std::string::npos)
	{
		throw std::runtime_error("Invalid Hill cipher config format");
	}

	std::string sizeStr = config.substr(startPos, endPos - startPos);
	return stringToInt(sizeStr);
}

std::vector<std::vector<int>> parseHillMatrix(const std::string& matrixStr, int matrixSize) {
	std::vector<std::vector<int>> matrix(matrixSize, std::vector<int>(matrixSize));

	// Parse format "1 2; 3 4" or "1 2 3; 4 5 6; 7 8 9"
	std::vector<std::string> rows = splitString(matrixStr, ';');
	if (rows.size() != matrixSize) {
		throw std::runtime_error("Invalid matrix format: incorrect number of rows");
	}

	for (int i = 0; i < matrixSize; i++) {
		std::string rowStr = rows[i];
		// Trim whitespace
		size_t start = rowStr.find_first_not_of(" \t");
		size_t end = rowStr.find_last_not_of(" \t");
		if (start != std::string::npos && end != std::string::npos) {
			rowStr = rowStr.substr(start, end - start + 1);
		}

		std::vector<std::string> elements = splitString(rowStr, ' ');
		if (elements.size() != matrixSize) {
			throw std::runtime_error("Invalid matrix format: incorrect number of columns in row " + std::to_string(i));
		}

		for (int j = 0; j < matrixSize; j++) {
			matrix[i][j] = stringToInt(elements[j]);
		}
	}

	return matrix;
}


void PasswordManager::loadFromFile()
{
	std::ifstream file(filename.c_str());
	if (!file.is_open())
	{
		throw std::runtime_error("Cannot open file: " + filename);
	}

	// Read encrypted content
	std::string encryptedContent;
	std::string line;
	while (std::getline(file, line))
	{
		encryptedContent += line + "\n";
	}
	file.close();

	if (encryptedContent.empty())
	{
		throw std::runtime_error("File is empty or corrupted: " + filename);
	}

	// Decrypt content using simple XOR with master password
	std::string decryptedContent = simpleEncryptDecrypt(encryptedContent, masterPassword);

	// Parse decrypted content
	passwords.clear();
	delete fileCipher;
	fileCipher = nullptr;

	std::vector<std::string> lines = splitString(decryptedContent, '\n');
	std::string cipherType, cipherConfig;
	bool readingEntries = false;

	for (const std::string& currentLine : lines)
	{
		if (currentLine.empty()) continue;

		if (currentLine.find("CIPHER_TYPE:") == 0)
		{
			cipherType = currentLine.substr(12);
		}
		else if (currentLine.find("CIPHER_CONFIG:") == 0)
		{
			cipherConfig = currentLine.substr(14);
		}
		else if (currentLine == "ENTRIES:")
		{
			readingEntries = true;

			// Create cipher based on type and config
			if (cipherType == "Ceasar")
			{
				int shift = stringToInt(cipherConfig);
				fileCipher = new CeasarCipher(shift);
			}
			else if (cipherType == "TextCode")
			{
				fileCipher = new TextCodeCipher(cipherConfig);
			}
			else if (cipherType == "Hill")
			{
				// Parse Hill cipher config: "Matrix size: 2, Key matrix: 1 2; 3 4"
				int matrixSize = parseHillMatrixSize(cipherConfig);
				std::string matrixStr = parseHillMatrixString(cipherConfig);

				std::vector<std::vector<int>> keyMatrix = parseHillMatrix(matrixStr, matrixSize);
				fileCipher = new HillCipher(keyMatrix);
			}
			else
			{
				throw std::runtime_error("Unknown cipher type: " + cipherType);
			}
		}
		else if (readingEntries)
		{
			// Parse entry line: website|username|encrypted_password
			std::vector<std::string> parts = splitString(currentLine, '|');
			if (parts.size() == 3)
			{
				passwords.push_back(PasswordEntry(parts[0], parts[1], parts[2]));
			}
		}
	}

	if (!fileCipher)
	{
		throw std::runtime_error("Failed to create cipher from file data");
	}
}