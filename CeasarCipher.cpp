#include "CeasarCipher.h"

char CeasarCipher::shiftChar(char ch, int shiftVal) const
{
	if (ch < 32 || ch > 126) // make an exeption
	{
		return ch;
	}
	
	//<cctype>
	//std::isalpha

	int result = ch + shiftVal;

	if (result > 126)
	{
		result = 31 + (result - 126);
	}
	else if (result < 32)
	{
		result = 127 - (32 - result);
	}

	return static_cast<char>(result);

}

void CeasarCipher::validateShift(int& shiftVal)
{
	shiftVal = ((shiftVal % 95) + 95) % 95; 
}

CeasarCipher::CeasarCipher(int s) : shift(s)
{
	validateShift(shift);
}

std::string CeasarCipher::encrypt(const std::string& input)
{
    std::string result;
	result.reserve(input.length());
	
	for (char i : input)
	{
		result.push_back(shiftChar(i, shift));
	}
	return result;
}
std::string CeasarCipher::decrypt(const std::string& input)
{
	std::string result;
	result.reserve(input.length());

	for (char c : input)
	{
		result.push_back(shiftChar(c, -shift));
	}
	return result;
}

std::string CeasarCipher::serialize() const
{
	return "Caesar " + std::to_string(shift);
}
Cipher* CeasarCipher::clone() const
{
	return new CeasarCipher(shift);
}
std::string CeasarCipher::getType() const
{
	return "Ceasar";
}
std::string CeasarCipher::getConfig() const
{
	return std::to_string(shift);
}

