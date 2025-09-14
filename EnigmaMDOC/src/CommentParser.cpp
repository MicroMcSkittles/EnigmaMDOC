#include "CommentParser.h"
#include "Utils.h"

#include <sstream>
#include <filesystem>

namespace EMDOC {
	std::string ToString(ParserErrorType type)
	{
		switch (type)
		{
		case ParserErrorType::None:       return "No parser error";
		case ParserErrorType::InvalidDef: return "Invalid Def command";
		default:                          return "Unknown parser error";
		}
		return std::string();
	}
	ParserError PackParserError(ParserErrorType type, uint64_t line)
	{
		uint8_t utype = (uint8_t)type;
		uint64_t error = 0;
		// Place the type at the 8 most significant bits
		error |= utype;
		error = error << 56;
		// Place the line number in the remaning bits
		error |= (line & 0x00FFFFFF);
		return error;
	}
	ParserErrorType GetParserErrorType(ParserError error)
	{
		uint8_t type = ((error >> 56) & 0xFF);
		return (ParserErrorType)type;
	}
	uint64_t GetParserErrorLine(ParserError error)
	{
		uint64_t line = error & 0x00FFFFFF;
		return line;
	}
	std::string GetParserErrorMessage(ParserError error)
	{
		std::stringstream outputBuffer;

		ParserErrorType type = GetParserErrorType(error);
		outputBuffer << ToString(type) << "\n";
		outputBuffer << "On line ( " << GetParserErrorLine(error) << " )\n";
		
		return outputBuffer.str();
	}

	std::string GetObjectTypeCPP(const std::string& def)
	{
		std::string type;
		size_t start = 0;
		size_t end = 0;
		size_t bracketCount = 0;
		// Loop through each word in def
		while ((end = def.find_first_of(" \n\t\r\0", start)) != std::string::npos) {
			std::string str = std::string(def.begin() + start, def.begin() + end);
			start = end + 1;
			end = 0;
			while ((end = str.find_first_of("<>", end)) != std::string::npos) {
				if (str[end] == '<') bracketCount += 1;
				else if (str[end] == '>') bracketCount -= 1;
				end += 1;
			}
			// Push word to the type string
			type.append(str + " ");
			// If the word is a C/C++ key word then continue pushing back words
			if (str == "const" || str == "static" || str == "extern" || bracketCount != 0) continue;
			type.pop_back();
			break;
		}
		return type;
	}
	bool IsVariableCPP(const std::string& def)
	{
		if (def.empty()) return false;
		// Get the variable type
		std::string type = GetObjectTypeCPP(def);
		size_t start = type.size();
		// Get the variable name
		std::string name = FindFirstWord(def, start, " \n\t\r\0", ";= \n\t\r\0");
		// If the variable is actualy a variable then the next character will be either a ; or =
		start = def.find_first_not_of(" \n\t\r\0", start - 1);
		if (start == std::string::npos) return false;

		if (def[start] == ';' || def[start] == '=') return true;
		return false;
	}
	bool IsFunctionCPP(const std::string& def)
	{
		if (def.empty()) return false;
		// Get the function type
		std::string type = GetObjectTypeCPP(def);
		size_t start = type.size();
		// Get the function name
		std::string name = FindFirstWord(def, start, " \n\t\r\0", "( \n\t\r\0");
		// If the function is actualy a function then the next character will be (
		start = def.find_first_not_of(" \n\t\r\0", start - 1);
		if (start == std::string::npos) return false;

		if (def[start] == '(') return true;
		return false;
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

	ParserError ParseClassStructDefCPP(CLT::DataBranch& docTree, const std::string& fileContent, size_t& commentIndex, const std::vector<CommentBlock>& comments, const Config& config)
	{
		CommentBlock block = comments[commentIndex];
		size_t start = block.commentEnd;
		size_t end = 0;

		// Get the class/struct type and name
		std::string type = FindFirstWord(fileContent, start);
		std::string name = FindFirstWord(fileContent, start, " \n\r\t\0", "{ \n\r\t\0");

		// Get the description
		start = block.text.find(config.commandTag) + config.commandTag.size() + 4;
		start = block.text.find_first_not_of(" \n\r\t\0", start);
		std::string description = std::string(block.text.begin() + start, block.text.end());
		
		// Create a data branch for the class/struct
		CLT::DataBranch classBranch = CLT::DataBranchType::Object;
		classBranch["name"]        = name;
		classBranch["branch_type"] = type;
		classBranch["description"] = description;

		// Find all of the things inside the class/struct
		CLT::DataBranch children = CLT::DataBranchType::Object;
		commentIndex += 1;
		for (; commentIndex < comments.size(); ++commentIndex) {
			// Break out of the loop when we get to the NDEF command
			std::string commentType = comments[commentIndex].FindCommandType(config);
			if (commentType == "NDEF") break;

			ParserError error = 0;
			if (commentType == "DEF") {
				error = ParseDefCommandCPP(children, fileContent, commentIndex, comments, config);
			}
			if (error) return error;
		}

		if (comments[commentIndex].FindCommandType(config) != "NDEF") {
			return PackParserError(ParserErrorType::ExpectedNDef, block.line);
		}

		classBranch["children"] = children;
		docTree[name] = classBranch;
		return 0;
	}
	ParserError ParseVariableDefCPP(CLT::DataBranch& docTree, const std::string& variableDef, const std::string& fileContent, size_t& commentIndex, const std::vector<CommentBlock>& comments, const Config& config)
	{
		CommentBlock block = comments[commentIndex];

		// Get the variable type and name
		std::string type = GetObjectTypeCPP(variableDef);
		size_t start = type.size();
		std::string name = FindFirstWord(variableDef, start, " \n\r\t\0", ";,)= \n\r\t\0");
		
		// Get the variables default value if it has one
		start = variableDef.find_first_not_of(" \n\t\r\0", start - 1);
		std::string defaultValue;
		if (variableDef[start] == '=') {
			start += 1;
			defaultValue = FindFirstWord(variableDef, start, " \n\r\t\0", ";,)");
		}

		start = block.text.find(config.commandTag) + config.commandTag.size();
		FindFirstWord(block.text, start);
		start = block.text.find_first_not_of(" \n\t\0\r", start);
		std::string description = std::string(block.text.begin() + start, block.text.end());

		// Create a data branch for the variable
		CLT::DataBranch variableBranch = CLT::DataBranchType::Object;
		variableBranch["name"]                = name;
		variableBranch["type"]                = type;
		variableBranch["branch_type"]         = std::string("variable");
		variableBranch["description"]         = description;
		if (!defaultValue.empty()) variableBranch["default_value"] = defaultValue;
		else variableBranch["default_value"] = CLT::DataBranch();

		docTree[name] = variableBranch;
		return 0;
	}
	ParserError ParseFunctionDefCPP(CLT::DataBranch& docTree, const std::string& functionDef, const std::string& fileContent, size_t& commentIndex, const std::vector<CommentBlock>& comments, const Config& config)
	{
		CommentBlock block = comments[commentIndex];

		// Get the function return type and name
		std::string type = GetObjectTypeCPP(functionDef);
		size_t start = type.size();
		std::string name = FindFirstWord(functionDef, start, " \n\r\t\0", "( \n\r\t\0");

		// Get the function description
		start = block.text.find(config.commandTag) + config.commandTag.size();
		FindFirstWord(block.text, start);
		start = block.text.find_first_not_of(" \n\t\0\r", start);
		std::string description = std::string(block.text.begin() + start, block.text.end());

		std::vector<std::string> paramDefs;
		start = functionDef.find_first_of("(") + 1;
		size_t end = 0;
		while ((end = functionDef.find_first_of(",)", start)) != std::string::npos)
		{
			std::string paramDef = std::string(functionDef.begin() + start, functionDef.begin() + end + 1);
			paramDefs.push_back(paramDef);
			start = end + 2;
		}

		size_t paramIndex = 0;
		std::string returnDescription;
		CLT::DataBranch parameters = CLT::DataBranchType::Object;
		commentIndex += 1;
		for (; commentIndex < comments.size(); ++commentIndex) {
			std::string commentType = comments[commentIndex].FindCommandType(config);
			if (commentType == "NDEF") break;

			start = comments[commentIndex].text.find(config.commandTag) + config.commandTag.size();
			FindFirstWord(comments[commentIndex].text, start);
			start = comments[commentIndex].text.find_first_not_of(" \n\t\0\r", start);

			if (commentType == "RETURN") {
				returnDescription = std::string(comments[commentIndex].text.begin() + start, comments[commentIndex].text.end());
			}
			else if (commentType == "PARAM") {
				if (paramIndex == paramDefs.size()) continue;
				ParseVariableDefCPP(parameters, paramDefs[paramIndex++], fileContent, commentIndex, comments, config);
			}
		}

		CLT::DataBranch functionBranch = CLT::DataBranchType::Object;
		functionBranch["name"]        = name;
		functionBranch["return_type"] = type;
		functionBranch["parameters"]  = parameters;
		functionBranch["branch_type"] = std::string("function");
		functionBranch["description"] = description;
		functionBranch["function_definition"] = functionDef;
		if (!returnDescription.empty()) functionBranch["return_description"] = returnDescription;
		else functionBranch["return_description"] = CLT::DataBranch();

		docTree[name] = functionBranch;
		return 0;
	}
	ParserError ParseDefCommandCPP(CLT::DataBranch& docTree, const std::string& fileContent, size_t& commentIndex, const std::vector<CommentBlock>& comments, const Config& config)
	{
		CommentBlock block = comments[commentIndex];

		// Check if the comment is for a struct or class
		size_t start = block.commentEnd;
		size_t end = 0;
		std::string type = FindFirstWord(fileContent, start);

		if (type == "struct" || type == "class") {
			return ParseClassStructDefCPP(docTree, fileContent, commentIndex, comments, config);
		}


		// Check if the comment is for a variable behind the comment block
		end   = fileContent.find_last_not_of("/* \t\r\0", block.start - 1);
		start = fileContent.find_last_of("\n", end);
		if (start != end) {
			start = fileContent.find_first_not_of(" \n\t\r\0", start);
			type = std::string(fileContent.begin() + start, fileContent.begin() + end + 1);
		}
		else type = "";
		if (IsVariableCPP(type)) {
			return ParseVariableDefCPP(docTree, type, fileContent, commentIndex, comments, config);
		}

		// Check if the comment is for a variable or function ahead of the comment block
		start = fileContent.find_first_not_of(" \n\t\r\0", block.commentEnd);
		end   = fileContent.find_first_of("\n", start);
		end   = fileContent.find_last_not_of(" \n\t\r\0", end);
		type = std::string(fileContent.begin() + start, fileContent.begin() + end);
		if (IsVariableCPP(type)) {
			return ParseVariableDefCPP(docTree, type, fileContent, commentIndex, comments, config);
		}
		if (IsFunctionCPP(type)) {
			return ParseFunctionDefCPP(docTree, type, fileContent, commentIndex, comments, config);
		}


		return PackParserError(ParserErrorType::InvalidDef, block.line);
	}

	ParserError ParseCommentsCPP(CLT::DataBranch& docTree, const std::string& filename, const std::vector<CommentBlock>& comments, const Config& config)
	{
		std::string fileContent = LoadFileStr(filename);
		docTree = CLT::DataBranchType::Object;

		for (size_t commentIndex = 0; commentIndex < comments.size(); ++commentIndex) {

			CommentBlock block = comments[commentIndex];
			if (block.text.find(config.commandTag) != 0) continue;

			// Get command type
			std::string type = block.FindCommandType(config);

			ParserError error = 0;
			if (type == "DEF") error = ParseDefCommandCPP(docTree, fileContent, commentIndex, comments, config);
			if (error) return error;
		}
		return 0;
	}
	ParserError ParseComments(CLT::DataBranch& docTree, const std::string& filename, const std::vector<CommentBlock>& comments, const Config& config)
	{
		// Find the file's extension, skip it if it isn't one of the specified extensions
		std::string ext= std::filesystem::path(filename).extension().string();
		if (config.extensions.find(ext) == config.extensions.end()) return 0;
		FileType type = config.extensions.at(ext);

		// Call the correct find comment blocks function based on the language the extension specifies
		if (type == FileType::CnCPP) {
			return ParseCommentsCPP(docTree, filename, comments, config);
		}

		return 0;
	}
}