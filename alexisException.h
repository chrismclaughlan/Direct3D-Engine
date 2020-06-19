// Credit: https://www.youtube.com/watch?v=QYGLXhulvVQ

#pragma once
#include <exception>
#include <string>

#include "common_types.h"

class AlexisException :public std::exception
{
public:
	AlexisException(int32 line, const char* file) noexcept;
	const char* what() const noexcept override;
	virtual const char* GetType() const noexcept;
	int32 GetLine() const noexcept;
	const std::string& GetFile() const noexcept;
	std::string GetOriginString() const noexcept;
private:
	int32 line;
	std::string file;
protected:
	mutable std::string whatBuffer;
};