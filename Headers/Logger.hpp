#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <sstream>

static constexpr size_t MAX_MESSAGES = 50;

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

	// FOR TIME: std::string(CurrentDateTimeToString())
	template<typename... Args>
	static void Log(Args&&... args)
	{
		Trim();
		std::ostringstream oss;
		(oss << ... << args);

		LogEntry entry;
		entry.type = LogType::LOG_INFO;
		std::string ossString = oss.str();
		std::string msg = std::string("LOG | ") + ossString;
		entry.message = msg;
		messages.push_back(entry);
	};

	template<typename... Args>
	static void Error(Args&&... args)
	{
		Trim();
		std::ostringstream oss;
		(oss << ... << args);

		LogEntry entry;
		entry.type = LogType::LOG_ERROR;
		std::string ossString = oss.str();
		std::string msg = std::string("ERROR | ") + ossString;
		entry.message = msg;
		messages.push_back(entry);
	};

	template<typename... Args>
	static void Warning(Args&&... args)
	{
		Trim();
		std::ostringstream oss;
		(oss << ... << args);

		LogEntry entry;
		entry.type = LogType::LOG_WARNING;
		std::string ossString = oss.str();
		std::string msg = std::string("WARNING | ") + ossString;
		entry.message = msg;
		messages.push_back(entry);
	};

	static const std::vector<LogEntry>& GetMessages() 
	{
		return messages; 
	}

	static void Flush() { messages.clear(); }

	static void Trim() 
	{
		if (messages.size() >= MAX_MESSAGES)
			messages.erase(messages.begin(), messages.begin() + (MAX_MESSAGES / 2));
		;
	}

private:

};

