#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <sstream>

enum class LogType 
{
	LOG_INFO,
	LOG_WARNING,
	LOG_ERROR,
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

	template<typename... Args>
	static void Log(Args&&... args)
	{
		std::ostringstream oss;
		(oss << ... << args);

		LogEntry entry;
		entry.type = LogType::LOG_INFO;
		std::string msg = "LOG | " + CurrentDateTimeToString() + " | " + oss.str();
		entry.message = oss.str();
		messages.push_back(entry);
	};

	template<typename... Args>
	static void Error(Args&&... args)
	{
		std::ostringstream oss;
		(oss << ... << args);

		LogEntry entry;
		entry.type = LogType::LOG_ERROR;
		std::string msg = "ERROR | " + CurrentDateTimeToString() + " | " + oss.str();
		entry.message = oss.str();
		messages.push_back(entry);
	};

	template<typename... Args>
	static void Warning(Args&&... args)
	{
		std::ostringstream oss;
		(oss << ... << args);

		LogEntry entry;
		entry.type = LogType::LOG_WARNING;
		std::string msg = "WARNING | " + CurrentDateTimeToString() + " | " + oss.str();
		entry.message = oss.str();
		messages.push_back(entry);
	};

	static const std::vector<LogEntry>& GetMessages() 
	{
		//if (messages.size() > 20) Flush();
		//else 
		return messages; 
	}

	static void Flush() { messages.clear(); }

private:

};

