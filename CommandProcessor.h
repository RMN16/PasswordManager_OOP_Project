#pragma once
#include <string>
#include <vector>
#include "PasswordManager.h"
#include "Cipher.h"

class CommandProcessor
{
private:
    PasswordManager* passwordManager;

    std::vector<std::string> parseCommand(const std::string& commandLine) const;
    void validateArguments(const std::vector<std::string>& args, size_t expectedMin, size_t expectedMax = SIZE_MAX) const;

    void handleCreateCommand(const std::vector<std::string>& args);
    void handleOpenCommand(const std::vector<std::string>& args);
    void handleSaveCommand(const std::vector<std::string>& args);
    void handleLoadCommand(const std::vector<std::string>& args);
    void handleUpdateCommand(const std::vector<std::string>& args);
    void handleDeleteCommand(const std::vector<std::string>& args);

    bool isValidCipherType(const std::string& cipherType) const;
    void validateFileAccess(const std::string& filename) const;

	
    /*int parsePositiveInteger(const std::string& str, const std::string& paramName, int minVal, int maxVal) const;
    void validateFilePath(const std::string& filePath, const std::string& cipherName) const;*/

    Cipher* createCipher(const std::string& cipherType, const std::vector<std::string>& cipherParams) const;
  

public:
    CommandProcessor();
    ~CommandProcessor();

    void processCommand(const std::string& commandLine);
    void showHelp() const;
    bool hasOpenFile() const;
};

