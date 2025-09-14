#pragma once
#include <vector>
#include <string>

#include "ConfigurationLoader.h"

namespace EMDOC {
	// @EMDOC DEF Stores the information needed to parse comments
	struct CommentBlock {
		std::string text = "";                // @EMDOC DEF All of the text in a comment
		size_t start     = std::string::npos; // @EMDOC DEF Where the command starts in the source code
		size_t end       = std::string::npos; // @EMDOC DEF Where the command ends in the source code
		size_t commentStart = std::string::npos;
		size_t commentEnd = std::string::npos;

		size_t line = std::string::npos; // @EMDOC DEF The line the command is on in the file

		/*
		@EMDOC DEF Returns the type tag that follows the command tag 
		@EMDOC PARAM The configuration that was loaded in from file
		@EMDOC NDEF
		*/
		std::string FindCommandType(const Config& config) const;
	};
	// @EMDOC NDEF

	// C/CPP Functions
	void SplitCommentBlockCPP(std::vector<CommentBlock>& blocks, const std::string& fileContent, const Config& config);
	std::vector<CommentBlock> FindCommentBlocksCPP(const std::string& filename, const Config& config);

	/*
	@EMDOC DEF 
	Looks through a file and finds comments that contain the command tag
	@EMDOC PARAM The specific file that is being parsed
	@EMDOC PARAM The configuration that was loaded in from file
	@EMDOC RETURN Returns a list of the comment blocks that contain command tags
	@EMDOC NDEF
	*/
	std::vector<CommentBlock> FindCommentBlocks(const std::string& filename, const Config& config);
}