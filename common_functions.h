#pragma once
#include "common_windows.h"
#include "common_types.h"
#include <string>

// Returns pointer to new wchar_t -> must delete data after use
// TODO make less error prone
//static wchar_t* charToWchar(const char* data)
//{
//	wchar_t* convertedData;
//	int32 wchars_num = MultiByteToWideChar(0, 0, data, -1, NULL, 0);
//	convertedData = new wchar_t[wchars_num];
//	MultiByteToWideChar(0, 0, data, -1, convertedData, wchars_num);
//	return convertedData;
//}

inline const std::wstring CharToWString(const char* c)
{
	const std::string str(c);
	const std::wstring wstr = std::wstring(str.begin(), str.end());
	return wstr;
}