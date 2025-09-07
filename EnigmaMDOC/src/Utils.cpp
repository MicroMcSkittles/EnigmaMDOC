#include "Utils.h"

#include <string>
#include <sstream>
#include <fstream>

namespace EMDOC {

	std::string LoadFileStr(const std::string& filename)
	{
		// Open file
		std::ifstream file;
		file.open(filename);

		// Make sure file openend properly
		ASSERT(file.is_open(), "Failed to open file ( " + filename + " )");

		// Copy file content to string stream to return the file content as a string
		std::stringstream fileBuffer;
		fileBuffer << file.rdbuf();
		file.close();
		return fileBuffer.str();
	}

	std::string RemoveWhiteSpace(const std::string& str)
	{
		std::stringstream strBuffer(str);
		std::stringstream outputBuffer;

		// Loop through each line, store that line to a stringstream, then pull every thing in
		// the stringstream with the >> operator which skips spaces, then push a end line character
		// so each line is seporated from each other
		std::string line;
		while (std::getline(strBuffer, line)) {
			std::stringstream lineBuffer(line);
			bool wsCheck = false;
			while (lineBuffer >> line) {
				outputBuffer << line;
				wsCheck = true;
			}
			if (wsCheck) outputBuffer << "\n";
		}

		return outputBuffer.str();
	}

}