#pragma once
#include <string>
#include <stdexcept>

class PasswordEntry
{
private:
	std::string website;
	std::string username;
	std::string password;

public:
	PasswordEntry(const std::string& site, const std::string& user, const std::string& pass)
		: website(site), username(user), password(pass) {}

	const std::string& getWebsite() const {
		return website;
	}
	const std::string& getUsername() const {
		return username;
	}
	const std::string& getPassword() const {
		return password;
	}


	void setPassword(const std::string& newPassword) 
	{
		if (newPassword.empty())
		{
			throw std::invalid_argument("Password cannot be empty.");
		}
		
		//Asuming that the password is encrypted before being set
		password = newPassword; 
	}

	std::string serialize() const
	{
		return website + "|" + username + "|" + password + "|"; // format: "website|username|password|"
	}
	static PasswordEntry deserialize(const std::string& data)
	{
		size_t pos1 = data.find('|');
		size_t pos2 = data.find('|', pos1 + 1);
		if (pos1 == std::string::npos || pos2 == std::string::npos)
		{
			throw std::invalid_argument("Invalid serialized data format");
		}
		std::string site = data.substr(0, pos1);
		std::string user = data.substr(pos1 + 1, pos2 - pos1 - 1);
		std::string pass = data.substr(pos2 + 1, data.size() - pos2 - 2); // remove trailing '|'
		return PasswordEntry(site, user, pass);

	}

};

