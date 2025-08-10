#include "TextCodeCipher.h"
#include <fstream>
#include <cstdlib>

TextCodeCipher::TextCodeCipher(const std::string& text) : referenceText(text) 
{
    initializeMappings();
}
TextCodeCipher::TextCodeCipher(const std::string& filePath, bool isFile) : referenceText("")
{
    if (isFile) 
    {
        referenceText = readTextFromFile(filePath);
    }
    else 
    {
        referenceText = filePath;
    }
    initializeMappings();
}

void TextCodeCipher::initializeMappings() 
{
    charToPosition.clear();
    positionToChar.clear();

    for (size_t i = 0; i < referenceText.length(); ++i)
    {
        char currentChar = referenceText[i];
        bool charExists = false;

        // Check if character already exists in charToPosition
        for (size_t j = 0; j < charToPosition.size(); ++j)
        {
            if (charToPosition[j].first == currentChar)
            {
                charExists = true;
                break;
            }
        }

        if (!charExists)
        {
            charToPosition.push_back(std::make_pair(currentChar, static_cast<int>(i)));
            positionToChar.push_back(std::make_pair(static_cast<int>(i), currentChar));
        }
    }

    if (charToPosition.empty())
    {
        throw std::runtime_error("Reference text doesn't contain any characters!");
    }
}
int TextCodeCipher::findCharPosition(char c) const
{
    for (size_t i = 0; i < charToPosition.size(); ++i)
    {
        if (charToPosition[i].first == c)
        {
            return charToPosition[i].second;
        }
    }
    return -1; // Not found
}
char TextCodeCipher::findPositionChar(int pos) const
{
    for (size_t i = 0; i < positionToChar.size(); ++i)
    {
        if (positionToChar[i].first == pos)
        {
            return positionToChar[i].second;
        }
    }
    return '\0'; // Not found
}
std::string TextCodeCipher::trimWhitespace(const std::string& str)
{
    if (str.empty()) return str;

    size_t first = 0;
    while (first < str.size() &&
        (str[first] == ' ' || str[first] == '\t' ||
            str[first] == '\n' || str[first] == '\r' ||
            str[first] == '\v' || str[first] == '\f'))
    {
        ++first;
    }

    if (first == str.size())
    {
        return "";
    }

    size_t last = str.size() - 1;
    while (last > first &&
        (str[last] == ' ' || str[last] == '\t' ||
            str[last] == '\n' || str[last] == '\r' ||
            str[last] == '\v' || str[last] == '\f'))
    {
        --last;
    }

    return str.substr(first, last - first + 1);
}

std::string TextCodeCipher::readTextFromFile(const std::string& filePath) 
{
    std::ifstream file(filePath);
    if (!file.is_open()) 
    {
        throw std::runtime_error("Could not open file: " + filePath);
    }

    std::string content;
    char ch;
    while (file.get(ch))
    {
        content += ch;
    }
    return content;
}
std::string TextCodeCipher::encrypt(const std::string& plainText) 
{
    std::vector<int> positions;

    for (size_t i = 0; i < plainText.size(); ++i)
    {
        int pos = findCharPosition(plainText[i]);
        if (pos == -1)
        {
            throw std::runtime_error("Character not found in reference text: " + std::string(1, plainText[i]));
        }
        positions.push_back(pos);
    }

    std::string result = "{";
    for (size_t i = 0; i < positions.size(); ++i)
    {
        result += std::to_string(positions[i]);
        if (i < positions.size() - 1)
        {
            result += ", ";
        }
    }
    result += "}";

    return result;
}
std::string TextCodeCipher::decrypt(const std::string& cipherText) 
{
    std::string result;
    std::string numbers = cipherText;

    if (!numbers.empty() && numbers[0] == '{') 
    {
        numbers = numbers.substr(1);
    }
    if (!numbers.empty() && numbers[numbers.length() - 1] == '}') 
    {
        numbers = numbers.substr(0, numbers.length() - 1);
    }

    std::vector<std::string> numberStrings;
    std::string currentNumber;

    for (size_t i = 0; i < numbers.size(); ++i)
    {
        char ch = numbers[i];
        if (ch == ',')
        {
            std::string trimmed = trimWhitespace(currentNumber);
            if (!trimmed.empty())
            {
                numberStrings.push_back(trimmed);
            }
            currentNumber.clear();
        }
        else
        {
            currentNumber += ch;
        }
    }

    // Add the last number if there's anything left
    if (!currentNumber.empty())
    {
        std::string trimmed = trimWhitespace(currentNumber);
        if (!trimmed.empty())
        {
            numberStrings.push_back(trimmed);
        }
    }

    for (size_t i = 0; i < numberStrings.size(); ++i)
    {
        const std::string& numStr = numberStrings[i];
        if (!numStr.empty())
        {
            try
            {
                char* end;
                long position = std::strtol(numStr.c_str(), &end, 10);

                // Check if the entire string was converted
                if (*end != '\0')
                {
                    throw std::runtime_error("Invalid number format: " + numStr);
                }

                // Check for overflow
                if (position < INT_MIN || position > INT_MAX)
                {
                    throw std::runtime_error("Position out of range: " + numStr);
                }

                char c = findPositionChar(static_cast<int>(position));
                if (c == '\0')
                {
                    throw std::runtime_error("Position not found in reference text: " + numStr);
                }
                result.push_back(c);
            }
            catch (...)
            {
                throw std::runtime_error("Invalid number format: " + numStr);
            }
        }
    }

    return result;
}

std::string TextCodeCipher::serialize() const 
{
    return "TextCode " + referenceText;
}
Cipher* TextCodeCipher::clone() const 
{
    return new TextCodeCipher(referenceText);
}
std::string TextCodeCipher::getType() const 
{
    return "TextCode";
}
std::string TextCodeCipher::getConfig() const
{
	return referenceText;
}