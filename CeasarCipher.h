#pragma once
#include "Cipher.h"
class CeasarCipher : public Cipher
{
private:
	int shift;

	char shiftChar(char ch, int shiftVal) const; //this is what does the shifting

public:
	CeasarCipher(int s);

	std::string encrypt(const std::string& input) override;
	virtual std::string decrypt(const std::string& input) override;

	std::string serialize() const override;
	Cipher* clone() const override;
	std::string getType() const override;
	virtual std::string getConfig() const override;

	static void validateShift(int& shiftVal);

};

