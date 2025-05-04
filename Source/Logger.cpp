#include "Logger.hpp"
#include <iostream>
#include <windows.h>
#include <chrono>
#include <ctime>

std::vector<LogEntry> Logger::messages;

std::string Logger::CurrentDateTimeToString()
{
	std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

	std::string output(30, '\0');

	struct tm timeinfo;

	localtime_s(&timeinfo, &now);

	std::strftime(&output[0], output.size(), "%H:%M:%S", &timeinfo);

	return output;
}

void Logger::Log(const std::string& message)
{
	// Add Log In Entries
	LogEntry entry;
	entry.type = LogType::LOG_INFO;
	entry.message = "LOG | " + CurrentDateTimeToString() + " | " + message;
	messages.push_back(entry);

	// Make Text Green
	HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hStdout, FOREGROUND_GREEN | FOREGROUND_INTENSITY);

	std::cout << "LOG";

	// Back To White Text
	SetConsoleTextAttribute(hStdout, 15);

	std::cout << " | " << CurrentDateTimeToString() << " | " << message << "\n";
}

void Logger::Error(const std::string& message)
{
	// Add Error In Entries
	LogEntry entry;
	entry.type = LogType::LOG_ERROR;
	entry.message = "ERR | " + CurrentDateTimeToString() + " | " + message;
	messages.push_back(entry);

	// Make Text Red
	HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hStdout, FOREGROUND_RED | FOREGROUND_INTENSITY);

	std::cout << "ERR";

	// Back To White Text
	SetConsoleTextAttribute(hStdout, 15);

	std::cout << " | " << CurrentDateTimeToString() << " | " << message << "\n";
}
