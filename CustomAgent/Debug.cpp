#include "Debug.h"

#include "Utils.hpp"

ofstream Debug::m_debugFile = std::ofstream{"DefaultDebug.txt"};

void Debug::Init(int playerTeam)
{
	m_debugFile = std::ofstream{ Utils::FormatString("debug_%i.txt", playerTeam).c_str() };
}

void Debug::Log(const std::string& message)
{
	FormatDebug(message.c_str(), "INFO");
}

void Debug::Log(const char* message)
{
	FormatDebug(message, "INFO");
}

void Debug::LogWarning(const std::string& message)
{
	FormatDebug(message.c_str(), "WAR");
}

void Debug::LogWarning(const char* message)
{
	FormatDebug(message, "WAR");
}

void Debug::LogError(const std::string& message)
{
	FormatDebug(message.c_str(), "ERR");
}

void Debug::LogError(const char* message)
{
	FormatDebug(message, "ERR");
}

void Debug::FormatDebug(const char* message, const char* messageType)
{
	m_debugFile << "[" << messageType << "] " << message << std::endl;
}
