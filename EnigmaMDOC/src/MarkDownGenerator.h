#pragma once
#include "FileParser.h"
#include <CommandLineTools/IOFormat.h>
#include <string>
#include <vector>

namespace EMDOC {

	enum class ChunkType {
		None = 0,
		Description,
		Function,
		Variable,
		Last = Variable
	};

	// @EMDOC DEF Stores a piece of MarkDown source code so that the final file can be ordered properly
	struct Chunk {
		ChunkType type;         // @EMDOC ELEM The kind of chunk, certen types are placed higher then others in the file
		std::string value;      // @EMDOC ELEM The MarkDown source code for the chunk
		bool separator = false; // @EMDOC ELEM Places a separator at the end of the chunk if true
	};
	// @EMDOC NDEF

	// @EMDOC DEF Used to organize the differant MarkDown chunks
	struct Scope {
		std::string name;          // @EMDOC ELEM The name that will be displayed in the scope header
		std::vector<Chunk> chunks; // @EMDOC ELEM Every chunk in the scope
		std::vector<Scope> scopes; // @EMDOC ELEM Every child scope
		bool separator = false;    // @EMDOC ELEM Places a separator at the end of the chunk if true

		/*
		@EMDOC DEF
		Iterates through each child scope and chunk to generate the correctly layed out MarkDown source code
		@EMDOC PARAM How nested is the scope
		@EMDOC RETURN Returns the formated MarkDown source code
		@EMDOC NDEF
		*/
		std::string GenerateMarkDown(uint8_t depth);
	};
	// @EMDOC NDEF

	std::string FindFirstWord(const std::string& str, size_t& offset, const std::string& startSeparater = " \n\t\r\0", const std::string& endSeparater = " \n\t\r\0");

	// @EMDOC SEC Mark Down Functions

	/*
	@EMDOC DEF Creates a MarkDown header from a depth value
	@EMDOC PARAM The depth, the higher the number the smaller the header
	@EMDOC RETURN Returns a string of hashes
	@EMDOC NDEF
	*/
	std::string CreateHeader(uint8_t depth);

	/*
	@EMDOC DEF Creates a MarkDown formated table from the Command Line Tools Table
	@EMDOC PARAM The Command Line Tools table
	@EMDOC PARAM The configuration that was loaded in from file
	@EMDOC RETURN Returns a string that contains the MarkDown formated table
	@EMDOC NDEF
	*/
	std::string CreateTableString(const CLT::Table& table, uint8_t depth);

	// @EMDOC NSEC

	// C/CPP functions
	std::string GetObjectTypeCPP(const std::string& objectDef);
	void ParseVarDefCPP(CLT::Table& variableTable, const std::vector<CommentBlock> comments, size_t& commentIndex, uint8_t depth, const std::string& fileContent, const Config& config);
	void ParseFuncDefCPP(Scope& scope, const std::vector<CommentBlock> comments, size_t& commentIndex, uint8_t depth, const std::string& fileContent, const Config& config);
	void ParseDefCommandCPP(Scope& scope, const std::vector<CommentBlock> comments, size_t& commentIndex, uint8_t depth, const std::string& fileContent, const Config& config);
	void ParseCommentsCPP(Scope& scope, const std::vector<CommentBlock> comments, const std::string& filename, const Config& config);

	/*
	@EMDOC DEF
	Looks through the comments for the command tag, than interprates
	the commands to generate a MarkDown formated documentation file
	@EMDOC PARAM The list of comment blocks from the specified file
	@EMDOC PARAM The specific file that is being parsed
	@EMDOC PARAM The configuration that was loaded in from file
	@EMDOC RETURN Returns the list off MarkDown sections
	@EMDOC NDEF
	*/
	std::string ParseComments(const std::vector<CommentBlock> comments, const std::string& filename, const Config& config);

}