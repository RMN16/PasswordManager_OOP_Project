#pragma once
#include <string>
#include <vector>
#include "Cipher.h"
#include "PasswordEntry.h"

class PasswordManager
{
private:
	std::string filename; //name of the file that contains the passwords
	std::string masterPassword; //password used to encrypt/decrypt the file
	Cipher* fileCipher; //cipher used to encrypt/decrypt the passwords
	std::vector<PasswordEntry> passwords; //list of passwords stored in the file
	bool isFileOpen; //flag to indicate if a file is currently open

	

public:
	PasswordManager();
	~PasswordManager();

	bool getIsFileOpen() const { return isFileOpen; }
	Cipher* getFileCipher() const { return fileCipher; }
	void setFileCipher(Cipher* cipher);
	
	void saveToFile() const;
	void loadFromFile();
	std::vector<PasswordEntry> loadAllUsers(const std::string& website) const;

	void createFile(const std::string& filename, Cipher* cipher, const std::string& masterPassword);
	void openFile(const std::string& filename, const std::string& masterPassword);

	void addPassword(const std::string& website, const std::string& username, const std::string& password);
	PasswordEntry* findPassword(const std::string& website, const std::string& username);
	std::vector<PasswordEntry*> findPasswordsByWebsite(const std::string& website);
	bool updatePassword(const std::string& website, const std::string& username, const std::string& newPassword);
	bool deletePassword(const std::string& website, const std::string& username);
	int deletePasswordsByWebsite(const std::string& website);
	bool isOpen() const;

};

