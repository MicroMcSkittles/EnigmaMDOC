#pragma once
#include <cstdio>
#include <stdlib.h>
#include <string>

#define ERROR(msg) fprintf(stderr, "An error occurred: %s\n", std::string(msg).c_str()); exit(-1)
#define ASSERT(condition, msg) if (!(condition)) { ERROR(msg); }

#define WARNING(msg) fprintf(stderr, "Warning: %s\n", std::string(msg).c_str())
#define ASSERT_WARN(condition, msg) if(!(condition)) { WARNING(msg); }

namespace EMDOC {
	std::string LoadFileStr(const std::string& filename);
	std::string RemoveWhiteSpace(const std::string& str);

	std::string FindFirstWord(const std::string& str, size_t& offset, const std::string& startSeparater = " \n\t\r\0", const std::string& endSeparater = " \n\t\r\0");
}