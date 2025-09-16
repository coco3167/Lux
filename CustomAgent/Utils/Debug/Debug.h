#pragma once
#include <string>

#include <iostream>
#include <fstream>

/// <summary>
/// Debug class to log messages in a log file
/// </summary>
class Debug
{
private:
	static std::ofstream m_debugFile;

public:
	static void Init(int playerTeam);
	
	static void Log(const std::string& message);
	static void Log(const char* message);
	
	static void LogWarning(const std::string& message);
	static void LogWarning(const char* message);

	static void LogError(const std::string& message);
	static void LogError(const char* message);

private:
	static void FormatDebug(const char* message, const char* messageType);
};

