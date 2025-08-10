#pragma once
#include <string>
#include <vector>
#include <iostream>


class Cipher
{
private:


public:
	virtual ~Cipher() = default;

	virtual std::string encrypt(const std::string& input) = 0;
	virtual std::string decrypt(const std::string& input) = 0;

	virtual std::string serialize() const = 0; //create a string that represents the cipher, for saving to a file or sending over a network
	virtual Cipher* clone() const = 0; 
	virtual std::string getType() const = 0;
	virtual std::string getConfig() const = 0;
};

