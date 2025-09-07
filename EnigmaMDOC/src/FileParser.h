#pragma once
#include <vector>
#include <string>

#include "ConfigurationLoader.h"

namespace EMDOC {
	// @EMDOC DEF Stores the information needed to parse comments
	struct CommentBlock{
		std::string text = "";                // @EMDOC ELEM All of the text in a comment
		size_t start     = std::string::npos; // @EMDOC ELEM Where the comment starts in the source code
		size_t end       = std::string::npos; // @EMDOC ELEM Where the comment ends in the source code
		/*
		@EMDOC DEF Returns the type tag that follows the command tag 
		@EMDOC PARAM The configuration that was loaded in from file
		@EMDOC NDEF
		*/
		std::string FindCommandType(const Config& config = Config()) const;
	};
	// @EMDOC NDEF

	// C/CPP Functions
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