#include <iostream>
#include <CommandLineTools/ArgParser.h>
#include <CommandLineTools/IOFormat.h>

#include <fstream>
#include <filesystem>

#include "ConfigurationLoader.h"
#include "FileParser.h"
#include "MarkDownGenerator.h"
#include "Utils.h"

using namespace EMDOC;

void ProcessFiles(const std::string& currentPath, const Config& config) {
    std::filesystem::recursive_directory_iterator dirIt(currentPath);
    for (const auto& entry : dirIt) {
        if (entry.is_directory()) {
            ProcessFiles(entry.path().string(), config);
            continue;
        }

        std::string filename = entry.path().string();
        std::vector<CommentBlock> commentBlocks = FindCommentBlocks(filename, config);
        if (commentBlocks.empty()) continue;

        std::string chunk = ParseComments(commentBlocks, filename, config);

        if (chunk.empty() || chunk.find_first_not_of(" \n\t\r\0") == std::string::npos) continue;

        size_t start = config.inputPath.size();
        size_t end = 0;
        std::string relativePath = std::string(currentPath.begin() + start, currentPath.end()) + "\\";

        start = filename.find_last_of("\\/") + 1;
        end = filename.find_last_of(".");
        std::string newFilename = std::string(filename.begin() + start, filename.begin() + end);
        newFilename = config.outputPath + relativePath + newFilename + ".md";

        std::ofstream file;
        file.open(newFilename);
        if (!file.is_open()) {
            WARNING("Failed to open/create file ( " + newFilename + " )");
            continue;
        }

        file << chunk;

        std::cout << "Wrote ( " << filename << " ) to ( " << newFilename << " )" << std::endl;

        file.close();
    }
}

int main(int argc, char** argv) {
    
    std::vector<CLT::Argument> arguments = CLT::ParseArgs(argc, argv);

    // Load Configuration
    Config config = LoadConfigFile();
    CLT::Table configTable = CLT::CreateTable("Configuration", { "Variable", "Value" });
    configTable << "Input Path";
    configTable << config.inputPath.c_str();
    configTable << "Output Path";
    configTable << config.outputPath.c_str();
    CLT::PrintTable(configTable);

    std::cout << std::endl;

    if (!std::filesystem::is_directory(config.outputPath)) {
        try { std::filesystem::create_directories(config.outputPath); }
        catch (const std::filesystem::filesystem_error& err) {
            ERROR(err.what());
        }
    }
    ProcessFiles(config.inputPath, config);

    return 0;
}