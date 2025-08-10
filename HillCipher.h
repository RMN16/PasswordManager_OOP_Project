#pragma once
#include "Cipher.h"

class HillCipher : public Cipher
{
private:
	typedef std::vector<std::vector<int>> Matrix;
		
	Matrix keyMatrix;
	Matrix inverseMatrix;
	int matrixSize;

	//matrix function helpers
	int determinant(const Matrix& matrix) const;
	int modInverse(int a, int m) const;
	Matrix adjugate(const Matrix& matrix) const;
	Matrix multiplyMatrices(const Matrix& a, const Matrix& b) const;
	Matrix inverseMatrix26(const Matrix& matrix) const;

	int charToInt(char c) const;
	char intToChar(int i) const;

	bool isValidMatrix() const;
	std::string padText(const std::string& text) const; //fill the matrix // padding

	int mod(int a, int m) const;
	static char toUpper(char c);

public:


	HillCipher(const Matrix& key);
    HillCipher(const std::string& keyString, int n);

    std::string encrypt(const std::string& plainText) override;
    std::string decrypt(const std::string& cipherText) override;

	std::string serialize() const override;
    Cipher* clone() const override;
    std::string getType() const override;
	virtual std::string getConfig() const override;

    static Matrix stringToMatrix(const std::string& keyString, int n);
    static std::string matrixToString(const Matrix& matrix);

};

