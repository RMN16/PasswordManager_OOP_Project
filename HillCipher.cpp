#include "HillCipher.h"
#include <cmath>
#include <string>
#include <vector>

int HillCipher::mod(int a, int m) const
{
    return ((a % m) + m) % m;
}
int HillCipher::modInverse(int a, int m) const
{
    a = mod(a, m);
    for (int x = 1; x < m; x++)
    {
        if (mod(a * x, m) == 1)
        {
            return x;
        }
    }
    return -1;
}

HillCipher::HillCipher(const Matrix& key) : keyMatrix(key) 
{
    matrixSize = keyMatrix.size();

    for (const auto& row : keyMatrix) 
    {
        if (row.size() != matrixSize) 
        {
            throw std::invalid_argument("Key matrix must be square");
        }
    }

    if (!isValidMatrix()) 
    {
        throw std::invalid_argument("Key matrix is not invertible in Z26");
    }

    inverseMatrix = inverseMatrix26(keyMatrix);
}
HillCipher::HillCipher(const std::string& keyString, int n) 
{
    if (n <= 0) 
    {
        throw std::invalid_argument("Matrix size must be positive");
    }

    keyMatrix = stringToMatrix(keyString, n);
    matrixSize = n;

    if (!isValidMatrix()) 
    {
        throw std::invalid_argument("Key matrix is not invertible in Z26");
    }

    inverseMatrix = inverseMatrix26(keyMatrix);
}

int HillCipher::charToInt(char c) const
{
    if (c >= 'A' && c <= 'Z')
    {
        return c - 'A';
    }
    else if (c >= 'a' && c <= 'z')
    {
        return c - 'a';
    }
    else
    {
        throw std::invalid_argument("Character out of range (A-Z, a-z): " + std::string(1, c));
    }
}
char HillCipher::intToChar(int i) const
{
	if (i < 0 || i >= 26)
	{
		throw std::invalid_argument("Integer out of range for character conversion: " + std::to_string(i));
	}

    i = mod(i, 26);
    return static_cast<char>(i + 'A');
}

int HillCipher::determinant(const Matrix& matrix) const
{
	if (matrix.empty() || matrix[0].empty())
	{
		throw std::invalid_argument("Matrix is empty");
	}
	if (matrix.size() != matrix[0].size())
	{
		throw std::invalid_argument("Matrix must be square");
	}
	if (matrix.size() == 0) return 0;
	if (matrix.size() == 1) return matrix[0][0];

    int n = matrix.size();

    if (n == 1)
    {
        return matrix[0][0];
    }

    if (n == 2)
    {
        return (matrix[0][0] * matrix[1][1] - matrix[0][1] * matrix[1][0]);
    }

    int det = 0;
    for (int i = 0; i < n; i++)
    {
        Matrix submatrix(n - 1, std::vector<int>(n - 1));
        for (int j = 1; j < n; j++)
        {
            int col = 0;
            for (int k = 0; k < n; k++)
            {
                if (k == i) continue;
                submatrix[j - 1][col++] = matrix[j][k];
            }
        }

        det += (i % 2 == 0 ? 1 : -1) * matrix[0][i] * determinant(submatrix);
    }

    return det;
}
HillCipher::Matrix HillCipher::adjugate(const Matrix& matrix) const
{
	if (matrix.empty() || matrix[0].empty())
	{
		throw std::invalid_argument("Matrix is empty");
	}
	if (matrix.size() != matrix[0].size())
	{
		throw std::invalid_argument("Matrix must be square");
	}
	if (matrix.size() == 0) return Matrix();
	if (matrix.size() == 1)
	{
		return Matrix{ {1} };
	}

    int n = matrix.size();
    Matrix adj(n, std::vector<int>(n));

    if (n == 1)
    {
        adj[0][0] = 1;
        return adj;
    }

	// Calculate the cofactor  and transpose it to get the adjugate matrix
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            Matrix submatrix(n - 1, std::vector<int>(n - 1));
            int sub_i = 0;
            for (int k = 0; k < n; k++)
            {
                if (k == i) continue;
                int sub_j = 0;
                for (int l = 0; l < n; l++)
                {
                    if (l == j) continue;
                    submatrix[sub_i][sub_j++] = matrix[k][l];
                }
                sub_i++;
            }

			// Calculate the cofactor and apply the sign based on position
            int cofactor = ((i + j) % 2 == 0 ? 1 : -1) * determinant(submatrix);
            adj[j][i] = mod(cofactor, 26);
        }
    }

    return adj;
}

HillCipher::Matrix HillCipher::multiplyMatrices(const Matrix& a, const Matrix& b) const
{
	if (a.empty() || b.empty() || a[0].empty() || b[0].empty()) 
    {
		throw std::invalid_argument("Matrices must not be empty");
	}
	if (a[0].size() != b.size()) 
    {
		throw std::invalid_argument("Matrix dimensions do not match for multiplication");
	}
	if (a.size() == 0 || b.size() == 0) return Matrix();
	if (a.size() == 1 && b.size() == 1) {
		return Matrix{ {mod(a[0][0] * b[0][0], 26)} };
	}
	
    int n = a.size();
    Matrix result(n, std::vector<int>(n, 0));

    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++) 
        {
            for (int k = 0; k < n; k++) 
            {
                result[i][j] += a[i][k] * b[k][j];
            }
            result[i][j] = mod(result[i][j], 26);
        }
    }

    return result;
}
HillCipher::Matrix HillCipher::inverseMatrix26(const Matrix& matrix) const
{
	if (matrix.empty() || matrix[0].empty()) 
    {
		throw std::invalid_argument("Matrix is empty");
	}

    int n = matrix.size();
    int det = mod(determinant(matrix), 26);
    int detInv = modInverse(det, 26);

    if (detInv == -1) 
    {
        throw std::invalid_argument("Matrix determinant has no multiplicative inverse in Z26");
    }

    Matrix adj = adjugate(matrix);
    Matrix result(n, std::vector<int>(n));

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            result[i][j] = mod(adj[i][j] * detInv, 26);
        }
    }

    return result;
}
bool HillCipher::isValidMatrix() const 
{
    int det = mod(determinant(keyMatrix), 26);
    return modInverse(det, 26) != -1;
}

std::string HillCipher::padText(const std::string& text) const
{
	if (text.empty()) {
		throw std::invalid_argument("Text to pad cannot be empty");
	}
	if (matrixSize <= 0) {
		throw std::invalid_argument("Matrix size must be positive");
	}
	if (matrixSize == 1) {
		return text; // No padding needed for 1x1 matrix
	}
	if (matrixSize < 1) {
		throw std::invalid_argument("Matrix size must be at least 1");
	}

    std::string result = text;
    int remainder = result.length() % matrixSize;

    if (remainder != 0)
    {
        result.append(matrixSize - remainder, 'X');
    }

    return result;
}

HillCipher::Matrix HillCipher::stringToMatrix(const std::string& keyString, int n)
{
	if (n <= 0)
	{
		throw std::invalid_argument("Matrix size must be positive");
	}
	if (keyString.empty())
	{
		throw std::invalid_argument("Key string cannot be empty");
	}
	if (keyString.length() % n != 0)
	{
		throw std::invalid_argument("Key string length must be a multiple of matrix size");
	}

    Matrix matrix(n, std::vector<int>(n));

    if (keyString.length() < n * n)
    {
        throw std::invalid_argument("Key string too short for specified matrix size");
    }

    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            char c = keyString[i * n + j];
            if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'))
            {
                matrix[i][j] = (c >= 'A' && c <= 'Z') ? (c - 'A') : (c - 'a');
            }
            else
            {
                throw std::invalid_argument("Invalid character in key string: " + std::string(1, c));
            }
        }
    }

    return matrix;
}
std::string HillCipher::matrixToString(const Matrix& matrix)
{
	if (matrix.empty() || matrix[0].empty())
	{
		throw std::invalid_argument("Matrix is empty");
	}
	if (matrix.size() != matrix[0].size())
	{
		throw std::invalid_argument("Matrix must be square");
	}

	std::string result;
	
    for (size_t i = 0; i < matrix.size(); i++)
    {
        const auto& row = matrix[i];
        for (size_t j = 0; j < row.size(); ++j)
        {
            std::string numStr = std::to_string(row[j]);
            result += numStr;

            if (j < row.size() - 1) {
                result += ' ';
            }
        }

        if (i < matrix.size() - 1) {
            result += "; ";
        }
    }

	return result; // format "1 2 3; 4 5 6; 7 8 9"
}

char HillCipher::toUpper(char c) 
{
    if (c >= 'a' && c <= 'z') 
    {
        return c - 'a' + 'A';
    }
    return c;
}

std::string HillCipher::encrypt(const std::string& plainText) 
{
	if (plainText.empty())
	{
		throw std::invalid_argument("Plain text cannot be empty");
	}

    std::string processedText;
    for (char c : plainText) 
    {
        if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z')) 
        {
            processedText.push_back(toUpper(c));
        }
    }

    processedText = padText(processedText);
    std::string result;

    for (size_t i = 0; i < processedText.length(); i += matrixSize)
    {
        std::vector<int> textVector(matrixSize);
        for (int j = 0; j < matrixSize; j++)
        {
            textVector[j] = charToInt(processedText[i + j]);
        }

        std::vector<int> resultVector(matrixSize, 0);
        for (int row = 0; row < matrixSize; row++)
        {
            for (int col = 0; col < matrixSize; col++)
            {
                resultVector[row] += keyMatrix[row][col] * textVector[col];
            }
            resultVector[row] = mod(resultVector[row], 26);
        }

        for (int j = 0; j < matrixSize; j++)
        {
            result.push_back(intToChar(resultVector[j]));
        }
    }

    return result;
}
std::string HillCipher::decrypt(const std::string& cipherText) 
{
	if (cipherText.empty())
	{
		throw std::invalid_argument("Cipher text cannot be empty");
	}
	if (inverseMatrix.empty())
	{
		throw std::invalid_argument("Key matrix is not invertible, decryption not possible");
	}

    std::string processedText;
    for (char c : cipherText)
    {
        if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'))
        {
            processedText.push_back(toUpper(c));
        }
    }

    processedText = padText(processedText);
    std::string result;

    for (size_t i = 0; i < processedText.length(); i += matrixSize)
    {
        std::vector<int> textVector(matrixSize);
        for (int j = 0; j < matrixSize; j++)
        {
            textVector[j] = charToInt(processedText[i + j]);
        }

        std::vector<int> resultVector(matrixSize, 0);
        for (int row = 0; row < matrixSize; row++)
        {
            for (int col = 0; col < matrixSize; col++)
            {
                resultVector[row] += inverseMatrix[row][col] * textVector[col];
            }
            resultVector[row] = mod(resultVector[row], 26);
        }

        for (int j = 0; j < matrixSize; j++)
        {
            result.push_back(intToChar(resultVector[j]));
        }
    }

    return result;
}

std::string HillCipher::serialize() const
{
	if (keyMatrix.empty() || keyMatrix[0].empty())
	{
		throw std::invalid_argument("Key matrix is empty");
	}
	if (matrixSize <= 0)
	{
		throw std::invalid_argument("Matrix size must be positive");
	}

    std::string result = "Hill " + std::to_string(matrixSize) + " ";

    for (const auto& row : keyMatrix)
    {
        for (int value : row)
        {
            result += std::to_string(value) + " ";
        }
    }

    return result;
}
Cipher* HillCipher::clone() const
{
    return new HillCipher(keyMatrix);
}
std::string HillCipher::getType() const
{
    return "Hill";
}
std::string HillCipher::getConfig() const
{
	return "Matrix size: " + std::to_string(matrixSize) + ", Key matrix: " + matrixToString(keyMatrix);
}