#include <iostream>
#include <CommandLineTools/ArgParser.h>
#include <CommandLineTools/IOFormat.h>

#include <fstream>
#include <filesystem>

#include "ConfigurationLoader.h"
#include "FileParser.h"
#include "MarkDownGenerator.h"
#include "CommentParser.h"
#include "FileIterator.h"
#include "Utils.h"

using namespace EMDOC;

int main(int argc, char** argv) {
    
    std::vector<CLT::Argument> arguments = CLT::ParseArgs(argc, argv);

    // Load Configuration
    Config config = LoadConfigFile();
    //PrintConfig(config);

    // Get the files that need to be parsed
    VarifyDirectories(config);
    std::vector<std::string> files = GetFilesToProcess(config.inputPath, config);

    // Process the files
    CLT::DataBranch docTree = CLT::DataBranchType::Object;
    ProcessFiles(files, docTree, config);

    if (config.createJSONInt) {
        CLT::JSONError jsonError = CLT::JSON::SaveJSON(docTree, "EMDOC_temp.json");
        ASSERT(!jsonError, CLT::JSONErrorMessage(jsonError));
    }

    GenerateMarkdown(docTree, config);
    return 0;
}