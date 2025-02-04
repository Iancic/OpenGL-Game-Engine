#pragma once
#include <string>
#include <vector>

enum class LogType 
{
	LOG_INFO,
	LOG_WARNING,
	LOG_ERROR,
	LOG_CRITICAL
};

struct LogEntry 
{
	LogType type = LogType::LOG_INFO;
	std::string message = "Default Message";
};

class Logger
{
public:

	static std::vector<LogEntry> messages;

	static std::string CurrentDateTimeToString();
	static void Log(const std::string& message);
	static void Error(const std::string& message);

private:
};

