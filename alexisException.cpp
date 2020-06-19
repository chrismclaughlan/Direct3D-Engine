#include <sstream>
#include "common_windows.h"
#include "common_types.h"
#include "alexisException.h"

AlexisException::AlexisException(int32 line, const char* file) noexcept
	: line(line), file(file) {}

const char* AlexisException::what() const noexcept
{
	std::stringstream oss;
	oss << GetType() << std::endl
		<< GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char* AlexisException::GetType() const noexcept
{
	return "Alexis Exception";
}

int32 AlexisException::GetLine() const noexcept
{
	return line;
}

const std::string& AlexisException::GetFile() const noexcept
{
	return file;
}

std::string AlexisException::GetOriginString() const noexcept
{
	std::ostringstream oss;
	oss << "[File] " << file << std::endl
		<< "[Line] " << line;
	return oss.str();
}