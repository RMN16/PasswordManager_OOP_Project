#pragma once
#include "Cipher.h"
#include <stdexcept>

class TextCodeCipher : public Cipher
{
private:
	std::string referenceText;
    std::vector<std::pair<char, int>> charToPosition; 
    std::vector<std::pair<int, char>> positionToChar; 

    void initializeMappings(); 
    int findCharPosition(char c) const;
    char findPositionChar(int pos) const;
    static std::string trimWhitespace(const std::string& str);

public:
    TextCodeCipher(const std::string& text);
    TextCodeCipher(const std::string& filePath, bool isFile);

    std::string encrypt(const std::string& input) override;
    std::string decrypt(const std::string& input) override;
    
    std::string serialize() const override;
    Cipher* clone() const override;
    std::string getType() const override;
	virtual std::string getConfig() const override;

    static std::string readTextFromFile(const std::string& filePath);
};

