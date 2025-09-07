#include "MarkDownGenerator.h"
#include "Utils.h"


#include <filesystem>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <iomanip>

#include <iostream>

namespace EMDOC {

	std::string CreateHeader(uint8_t depth)
	{
		std::string outputBuffer;
		uint8_t count = depth + 1;
		outputBuffer.resize(count, '#');
		return outputBuffer;
	}
	std::string CreateTableString(const CLT::Table& table, uint8_t depth)
	{
		// Find what fields have data in them
		std::vector<bool> isFieldUsed;
		isFieldUsed.resize(table.fields.size(), false);
		size_t emptyEntryCount = 0;
		for (size_t i = 0; i < table.entries.size(); ++i) {
			size_t fieldIndex = i % table.fields.size();
			if (!table.entries[i].empty()) isFieldUsed[fieldIndex] = true;
			else emptyEntryCount += 1;
		}

		// Find how many fields aren't being used
		size_t unusedCount = 0;
		size_t lastUsed = 0;
		for (size_t i = 0; i < isFieldUsed.size(); ++i) {
			if (!isFieldUsed[i]) unusedCount += 1;
			else lastUsed = i;
		}

		if (unusedCount == isFieldUsed.size()) return "";

		std::stringstream outputBuffer;
		std::stringstream separaterBuffer;

		// Push the table fields
		outputBuffer << "| ";
		separaterBuffer << "| " << std::setfill('-');
		for (size_t i = 0; i < table.fields.size(); ++i) {
			// Skip field if it doesn't have any data
			if (!isFieldUsed[i]) continue;

			outputBuffer << std::setw(table.fields[i].width) << std::left;
			outputBuffer << table.fields[i].name << " | ";

			// +3 to acount for the 3 separater characters
			uint16_t width = table.fields[i].width + 3;
			separaterBuffer << std::setw(width) << std::right;
			separaterBuffer << " | ";
		}
		outputBuffer << "\n" << separaterBuffer.str();

		// Push the table entries
		outputBuffer << "\n| ";
		size_t numSkiped = 0;
		for (size_t i = 0; i < table.entries.size(); ++i) {

			// Get field of the current entry
			size_t fieldIndex = i % table.fields.size();
			if (!isFieldUsed[fieldIndex]) {
				numSkiped += 1;
				continue;
			}
			const CLT::TableField& field = table.fields[fieldIndex];

			// Push current entry
			outputBuffer << std::setw(field.width) << std::left;
			outputBuffer << table.entries[i] << " | ";

			// Push end line character if this is the last entry on the current line
			if (fieldIndex == lastUsed && i + 2 < table.entries.size()) {
				outputBuffer << "\n| ";
			}
		}

		return outputBuffer.str();
	}

	std::string FindFirstWord(const std::string& str, size_t& offset, const std::string& startSeparater, const std::string& endSeparater)
	{
		size_t start = str.find_first_not_of(startSeparater, offset);
		size_t end = str.find_first_of(endSeparater, start);
		if (end == std::string::npos) return "";
		offset = end + 1;
		return std::string(str.begin() + start, str.begin() + end);
	}
	std::string CommentBlock::FindCommandType(const Config& config) const
	{
		// Find where the type tag starts
		size_t start = text.find(config.commandTag) + config.commandTag.size();
		start = text.find_first_not_of(" \n\t\r\0", start);
		// Find where the type tag ends
		size_t end = text.find_first_of(" \n\t\r\0", start);
		if (end == std::string::npos) end = text.size();
		// Create a new string from the start and end positions
		return std::string(text.begin() + start, text.begin() + end);
	}

	std::string GetObjectTypeCPP(const std::string& objectDef)
	{
		std::string type;
		size_t start = 0;
		size_t end = 0;
		while ((end = objectDef.find_first_of(" \n\t\r\0", start)) != std::string::npos) {
			std::string str = std::string(objectDef.begin() + start, objectDef.begin() + end);
			start = end + 1;
			type.append(str + " ");
			if (str == "const" || str == "static" || str == "extern") continue;
			type.pop_back();
			break;
		}
		return type;
	}
	void ParseVarDefCPP(CLT::Table& variableTable, const std::vector<CommentBlock> comments, size_t& commentIndex, uint8_t depth, const std::string& fileContent, const Config& config)
	{
		CommentBlock block = comments[commentIndex];

		// Get the variable definition
		size_t start = fileContent.find_last_of("\n", block.start);
		start = fileContent.find_first_not_of(" \n\t\r\0", start);
		std::string variableDef = std::string(fileContent.begin() + start, fileContent.begin() + block.start);

		// Remove white space from the end of the variable definition
		size_t end = variableDef.find_last_not_of(" \n\t\r\0") + 1; // +1 to include the last character
		variableDef = std::string(variableDef.begin(), variableDef.begin() + end);

		// Find the variable type
		std::string variableType = GetObjectTypeCPP(variableDef);

		// Find the variable name
		start = variableType.size();
		std::string variableName = FindFirstWord(variableDef, start, " \n\t\r\0", ";= \n\t\r\0");

		// Find the variables default value if it exists
		std::string variableDefault = "";
		start = variableDef.find_first_of("=");
		if (start != std::string::npos) {
			start += 1;
			variableDefault = FindFirstWord(variableDef, start, " \n\t\r\0", "; \n\t\r\0");
		}

		// Find the variable description
		start = block.text.find(config.commandTag) + config.commandTag.size() + 5; // +5 skips the ELEM tag
		start = block.text.find_first_not_of(" \n\t\r\0", start);
		std::string variableDescription = std::string(block.text.begin() + start, block.text.end());

		// Push the values to the variable table
		variableTable << variableName;
		variableTable << ("`" + variableType + "`");
		variableTable << variableDescription;

		// Push the default variable value if it exists, if not than push an empty string
		if (!variableDefault.empty()) variableTable << ("`" + variableDefault + "`");
		else variableTable << "";
	}
	void ParseFuncDefCPP(Scope& scope, const std::vector<CommentBlock> comments, size_t& commentIndex, uint8_t depth, const std::string& fileContent, const Config& config)
	{
		CommentBlock block = comments[commentIndex];
		std::string commandType = block.FindCommandType(config);
		if (commandType != "DEF") return;

		// Find the function definition blocks
		size_t startIndex = commentIndex;
		size_t paramIndex = std::string::npos; // Where the parameter definitions start
		size_t returnIndex = std::string::npos;
		for (; commentIndex < comments.size(); ++commentIndex) {
			commandType = comments[commentIndex].FindCommandType(config);
			if (commandType == "PARAM" && paramIndex == std::string::npos) paramIndex = commentIndex;
			else if (commandType == "RETURN") returnIndex = commentIndex;
			else if (commandType == "NDEF") break;
		}

		// Get the function definition
		size_t start = comments[commentIndex].end;
		size_t end = 0;
		std::string functionDef = FindFirstWord(fileContent, start, " \n\t\r\0", ";");
		if (functionDef == "") return;

		// Find the function return type
		std::string functionRetType = GetObjectTypeCPP(functionDef);

		// Find the function name
		start = functionRetType.size();
		std::string functionName = FindFirstWord(functionDef, start, " \n\t\r\0", "( \n\t\r\0");

		// Find all of the parameter definitions
		std::vector<std::string> functionParamDefs;
		start = functionDef.find_first_of("(") + 1;
		// Loop through each parameter in side of the perenthesis
		while ((end = functionDef.find_first_of(",", start)) != std::string::npos) {
			start = functionDef.find_first_not_of(" \n\t\r\0", start);
			functionParamDefs.push_back(std::string(functionDef.begin() + start, functionDef.begin() + end));
			start = end + 1;
		}
		// store the last parameter
		if ((end = functionDef.find_last_of(")")) != std::string::npos) {
			start = functionDef.find_first_not_of(" \n\t\r\0", start);
			functionParamDefs.push_back(std::string(functionDef.begin() + start, functionDef.begin() + end));
			start = end + 1;
		}

		// Find the function description
		std::string functionDescription;
		block = comments[startIndex];
		start = block.text.find(config.commandTag) + config.commandTag.size() + 4; // Skip the command tag
		start = block.text.find_first_not_of(" \n\t\r\0", start);
		end = block.text.find_last_not_of(" \n\t\r\0") + 1;
		functionDescription = std::string(block.text.begin() + start, block.text.begin() + end);

		// Find all parameter values and push them to a table
		CLT::Table paramTable = CLT::CreateTable("Parameters", { "Name", "Type", "Description", "Default Value" });
		for (size_t i = 0; i < functionParamDefs.size(); ++i) {
			std::string paramDef = functionParamDefs[i] + " ";

			// Find the variable type
			std::string paramType = GetObjectTypeCPP(paramDef);

			// Find the variable name
			start = paramType.size();
			std::string paramName = FindFirstWord(paramDef, start);

			// Find the variables default value if it exists
			std::string paramDefault = "";
			start = paramDef.find_first_of("=");
			if (start != std::string::npos) {
				start += 1;
				paramDefault = FindFirstWord(paramDef, start, " \n\t\r\0", " \n\t\r\0");
			}

			// Find the parameters description
			std::string paramDescription;
			if (paramIndex != std::string::npos && paramIndex + i < commentIndex && paramIndex + i >= paramIndex) {
				// Get the tag of the comment
				block = comments[paramIndex + i];
				std::string tag = block.FindCommandType(config);

				// Get the description
				if (tag == "PARAM") {
					start = block.text.find(config.commandTag) + config.commandTag.size() + tag.size() + 1;
					start = block.text.find_first_not_of(" \n\t\r\0", start);
					end = block.text.find_last_not_of(" \n\t\r\0") + 1;
					paramDescription = std::string(block.text.begin() + start, block.text.begin() + end);
				}
			}

			// Push the values to the parameter table
			paramTable << paramName;
			paramTable << ("`" + paramType + "`");
			paramTable << paramDescription;

			// Push the default variable value if it exists, if not than push an empty string
			if (!paramDefault.empty()) paramTable << ("`" + paramDefault + "`");
			else paramTable << "";
		}

		std::stringstream outputBuffer;

		outputBuffer << CreateHeader(depth) << " " << functionName << "\n";
		outputBuffer << functionDescription << "\n";
		outputBuffer << "```C++\n" << functionDef << "\n```\n";
		outputBuffer << CreateHeader(depth + 1) << " " << paramTable.name << "\n";
		outputBuffer << CreateTableString(paramTable, depth + 1);

		scope.chunks.push_back({ ChunkType::Function, outputBuffer.str() });
	}
	void ParseDefCommandCPP(Scope& scope, const std::vector<CommentBlock> comments, size_t& commentIndex, uint8_t depth, const std::string& fileContent, const Config& config)
	{
		CommentBlock block = comments[commentIndex];

		// Check if the comment is for a struct or class
		size_t start = block.end;
		size_t end = 0;
		std::string type = FindFirstWord(fileContent, start);
		if (type == "struct" || type == "class") {
			std::string name = FindFirstWord(fileContent, start, " \n\t\r\0", "{ ");

			// Get struct description
			start = block.text.find(config.commandTag) + config.commandTag.size() + 4; // +4 skips the DEF tag
			start = block.text.find_first_not_of(" \n\t\r\0", start);
			std::string description = std::string(block.text.begin() + start, block.text.end());

			Scope functionScope;
			functionScope.name = "Functions";
			CLT::Table variableTable = CLT::CreateTable("Variables", { "Name", "Type", "Description", "Default Value" });

			commentIndex += 1;
			size_t startCommentIndex = commentIndex;
			// Find variables and functions in struct
			for (; commentIndex < comments.size(); ++commentIndex) {
				block = comments[commentIndex];
				std::string commandType = block.FindCommandType(config);
				if (commandType == "NDEF") break;

				// Handle functions
				if (commandType == "DEF") {
					ParseFuncDefCPP(functionScope, comments, commentIndex, depth + 2, fileContent, config);
					continue;
				}
				// Handle variables
				if (commandType == "ELEM") {
					ParseVarDefCPP(variableTable, comments, commentIndex, depth + 1, fileContent, config);
					continue;
				}
			}

			Scope structScope;
			structScope.name = name;
			structScope.separator = true;
			structScope.chunks.push_back({ ChunkType::Description, description });

			Scope variableScope;
			variableScope.name = "Variables";
			variableScope.chunks.push_back({ ChunkType::Variable, CreateTableString(variableTable, depth + 1) });
			if (!variableScope.chunks[0].value.empty()) structScope.scopes.push_back(variableScope);

			if (!functionScope.chunks.empty()) structScope.scopes.push_back(functionScope);

			scope.scopes.push_back(structScope);
			return;
		}

		// Skip to end of def comments
		size_t startIndex = commentIndex;
		for (; commentIndex < comments.size(); ++commentIndex) {
			block = comments[commentIndex];
			if (block.FindCommandType() == "NDEF") break;
		}

		start = block.end;
		std::string objectDef = FindFirstWord(fileContent, start, " \n\t\r\0", ";");

		// Get the type
		start = 0;
		type = FindFirstWord(objectDef, start);

		// Check if the comment is for a function
		if ((end = objectDef.find_first_of("(")) != std::string::npos) {
			commentIndex = startIndex;
			ParseFuncDefCPP(scope, comments, commentIndex, depth, fileContent, config);
			scope.chunks[scope.chunks.size() - 1].separator = true;
		}

		std::string name = FindFirstWord(fileContent, start);
		// Process variables ?

	}
	void ParseCommentsCPP(Scope& scope, const std::vector<CommentBlock> comments, const std::string& filename, const Config& config)
	{
		std::string fileContent = LoadFileStr(filename);

		for (size_t commentIndex = 0; commentIndex < comments.size(); ++commentIndex) {

			CommentBlock block = comments[commentIndex];
			if (block.text.find(config.commandTag) != 0) continue;

			// Get command type
			std::string type = block.FindCommandType(config);

			if (type == "DEF") ParseDefCommandCPP(scope, comments, commentIndex, 0, fileContent, config);
			else if (type == "SEC") {
				size_t start = commentIndex;
				for (; commentIndex < comments.size(); ++commentIndex) {
					block = comments[commentIndex];
					if (block.FindCommandType(config) == "NSEC") break;
				}
				std::vector<CommentBlock> sectionBlocks;
				sectionBlocks = std::vector<CommentBlock>(comments.begin() + start + 1, comments.begin() + commentIndex);
				Scope section;
				block = comments[start];
				section.name = std::string(block.text.begin() + block.text.find("SEC") + 4, block.text.end());
				ParseCommentsCPP(section, sectionBlocks, filename, config);
				scope.scopes.push_back(section);
			}
		}
	}

	std::string ParseComments(const std::vector<CommentBlock> comments, const std::string& filename, const Config& config)
	{
		// Find the file's extension, skip it if it isn't one of the specified extensions
		std::string extension = std::filesystem::path(filename).extension().string();
		if (std::find(config.extensions.begin(), config.extensions.end(), extension) == config.extensions.end()) {
			return std::string();
		}

		Scope scope;

		// Call the correct find comment blocks function based on the language the extension specifies
		if (extension == ".h" || extension == ".c" || extension == ".hpp" || extension == ".cpp") {
			ParseCommentsCPP(scope, comments, filename, config);
		}

		return scope.GenerateMarkDown(-1);
	}

	std::string Scope::GenerateMarkDown(uint8_t depth)
	{
		std::stringstream outputBuffer;
		outputBuffer << CreateHeader(depth) << " " << name << "\n";

		std::vector<size_t> startPositions;
		startPositions.resize((size_t)ChunkType::Last, 0);
		std::vector<Chunk> sortedChunks;

		// Sort each chunk based on its type
		for (Chunk& c : chunks) {
			sortedChunks.insert(sortedChunks.begin() + startPositions[(size_t)c.type - 1], c);
			for (size_t i = (size_t)c.type; i < startPositions.size(); ++i) {
				startPositions[i] += 1;
			}
		}

		// Push each chunk in the correct order
		for (Chunk& c : sortedChunks) {
			outputBuffer << c.value << "\n";
			if (c.separator) outputBuffer << "\n---\n";
		}

		for (size_t i = 0; i < scopes.size(); ++i) {
			outputBuffer << scopes[i].GenerateMarkDown(depth + 1) << "\n";
			if (scopes[i].separator) outputBuffer << "\n---\n";
		}

		return outputBuffer.str();
	}
}