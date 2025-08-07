#include "precomp.h"
#include "Logger.hpp"

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