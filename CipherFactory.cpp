#include "CipherFactory.h"
#include "CeasarCipher.h"
#include "TextCodeCipher.h"
#include "HillCipher.h"
#include <stdexcept>
#include <cstdlib> 


// Helper function to split a string by spaces
std::vector<std::string> CipherFactory::splitString(const std::string& str)
{
    std::vector<std::string> tokens;
    std::string token;
    bool inToken = false;

    for (char c : str)
    {
        if (c == ' ' || c == '\t' || c == '\n' || c == '\r') {
            if (inToken) {
                tokens.push_back(token);
                token.clear();
                inToken = false;
            }
        }
        else
        {
            token += c;
            inToken = true;
        }
    }

    if (!token.empty())
    {
        tokens.push_back(token);
    }

    return tokens;
}

Cipher* CipherFactory::createCipher(const std::string& type, const std::vector<std::string>& params)
{
	if (type.empty())
    {
		throw std::invalid_argument("Cipher type cannot be empty");
	}
	if (params.empty())
    {
		throw std::invalid_argument("Parameters cannot be empty for cipher type: " + type);
	}

    if (type == "caesar")
    {
        if (params.size() != 1)
        {
            throw std::invalid_argument("Caesar cipher requires exactly one parameter: shift value");
        }
        return createCaesarCipher(params);
    }
    else if (type == "textcode")
    {
        if (type == "TextCode" && params.size() != 1)
        {
            throw std::invalid_argument("TextCode cipher requires exactly one parameter: reference text or file path");
        }
        return createTextCodeCipher(params);
    }
    else if (type == "hill")
    {
        if (params.size() < 2)
        {
            throw std::invalid_argument("Hill cipher requires at least two parameters");
        }
        return createHillCipher(params);
    }
    else
    {
        throw std::invalid_argument("Unknown cipher type: " + type);
    }
}
Cipher* CipherFactory::createFromSerialized(const std::string& serialized)
{
	if (serialized.empty())
	{
		throw std::invalid_argument("Serialized data cannot be empty");
	}

    std::vector<std::string> tokens = splitString(serialized);
    if (tokens.empty())
    {
        throw std::invalid_argument("Empty serialized string");
    }

    std::string type = tokens[0];
    std::vector<std::string> params(tokens.begin() + 1, tokens.end());
    return createCipher(type, params);
}

Cipher* CipherFactory::createCaesarCipher(const std::vector<std::string>& params)
{
    if (params.empty())
    {
        throw std::invalid_argument("Caesar cipher requires a shift parameter");
    }

    char* end;
    long shift = std::strtol(params[0].c_str(), &end, 10);

    if (*end != '\0' || end == params[0].c_str())
    {
        throw std::invalid_argument("Invalid shift parameter for Caesar cipher: " + params[0]);
    }

    return new CeasarCipher(static_cast<int>(shift));
}
Cipher* CipherFactory::createTextCodeCipher(const std::vector<std::string>& params)
{
	if (params.empty())
	{
		throw std::invalid_argument("TextCode cipher requires a reference text or file path");
	}
    if (params[0] == "file" && params.size() > 1) 
    {
        return new TextCodeCipher(params[1], true);
	}
	else if (params[0] == "text" && params.size() > 1)
    {
		return new TextCodeCipher(params[1], false);
	}
	else if (params.size() == 1 && params[0].find("file:") == 0)
    {
		return new TextCodeCipher(params[0].substr(5), true);
	}
	else if (params.size() == 1 && params[0].find("text:") == 0)
    {
		return new TextCodeCipher(params[0].substr(5), false);
	}
    else 
	{		// If no specific type is given, treat it as a text code cipher with the provided text
        return new TextCodeCipher(params.back());
    }

	//return new TextCodeCipher(params[0]);
}
Cipher* CipherFactory::createHillCipher(const std::vector<std::string>& params)
{
	if (params.size() < 2)
	{
		throw std::invalid_argument("Hill cipher requires two parameters: key string and matrix size");
	}
    char* end;
    long matrixSize = std::strtol(params[0].c_str(), &end, 10);
    if (*end != '\0' || end == params[0].c_str() || matrixSize <= 0) {
        throw std::invalid_argument("Invalid matrix size for Hill cipher: " + params[0]);
    }

    if (params.size() == 2) {
        // Case 1: Matrix size + key string (auto-generate matrix)
        return new HillCipher(params[1], static_cast<int>(matrixSize));
    }
    else {
        // Case 2: Matrix size + all matrix elements
        size_t expectedElements = static_cast<size_t>(matrixSize * matrixSize);
        if (params.size() != expectedElements + 1) {
            throw std::invalid_argument("Hill cipher requires exactly " +
                std::to_string(expectedElements) + " matrix elements for " +
                std::to_string(matrixSize) + "x" + std::to_string(matrixSize) + " matrix, got " +
                std::to_string(params.size() - 1));
        }

        // Build matrix from parameters
        std::vector<std::vector<int>> keyMatrix(static_cast<int>(matrixSize),
            std::vector<int>(static_cast<int>(matrixSize)));

        for (int i = 0; i < matrixSize; i++) {
            for (int j = 0; j < matrixSize; j++) {
                const std::string& param = params[1 + i * matrixSize + j];
                char* elemEnd;
                long value = std::strtol(param.c_str(), &elemEnd, 10);
                if (*elemEnd != '\0' || elemEnd == param.c_str()) {
                    throw std::invalid_argument("Invalid matrix element for Hill cipher: " + param);
                }
                keyMatrix[i][j] = static_cast<int>(value);
            }
        }

        return new HillCipher(keyMatrix);
    }
}
