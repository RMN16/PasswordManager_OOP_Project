#pragma once
#include "Cipher.h"
#include <string>
#include <vector>
#include <memory>

class CipherFactory
{
private:
	static Cipher* createCaesarCipher(const std::vector<std::string>& params);
	static Cipher* createTextCodeCipher(const std::vector<std::string>& params);
	static Cipher* createHillCipher(const std::vector<std::string>& params);

	static std::vector<std::string> splitString(const std::string& str);

public:
	static Cipher* createCipher(const std::string& type, const std::vector<std::string>& params);
	static Cipher* createFromSerialized(const std::string& serializedData);

};

