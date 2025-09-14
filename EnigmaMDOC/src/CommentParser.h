#pragma once
#include "FileParser.h"

#include <CommandLineTools/JSONparser.h>

namespace EMDOC {

	// the 8 most significate bits store the type, the rest store the line number
	typedef uint64_t ParserError;
	enum class ParserErrorType {
		None = 0,
		InvalidDef,
		ExpectedNDef,
	};
	std::string     ToString(ParserErrorType type);
	ParserError     PackParserError(ParserErrorType type, uint64_t line);
	ParserErrorType GetParserErrorType(ParserError error);
	uint64_t        GetParserErrorLine(ParserError error);
	std::string     GetParserErrorMessage(ParserError error);

	std::string GetObjectTypeCPP(const std::string& def);
	bool IsVariableCPP(const std::string& def);
	bool IsFunctionCPP(const std::string& def);

	ParserError ParseClassStructDefCPP(CLT::DataBranch& docTree, const std::string& fileContent, size_t& commentIndex, const std::vector<CommentBlock>& comments, const Config& config);
	ParserError ParseVariableDefCPP(CLT::DataBranch& docTree, const std::string& variableDef, const std::string& fileContent, size_t& commentIndex, const std::vector<CommentBlock>& comments, const Config& config);
	ParserError ParseFunctionDefCPP(CLT::DataBranch& docTree, const std::string& functionDef, const std::string& fileContent, size_t& commentIndex, const std::vector<CommentBlock>& comments, const Config& config);
	ParserError ParseDefCommandCPP(CLT::DataBranch& docTree, const std::string& fileContent, size_t& commentIndex, const std::vector<CommentBlock>& comments, const Config& config);
	ParserError ParseCommentsCPP(CLT::DataBranch& docTree, const std::string& filename, const std::vector<CommentBlock>& comments, const Config& config);
	
	ParserError ParseComments(CLT::DataBranch& docTree, const std::string& filename, const std::vector<CommentBlock>& comments, const Config& config);
}