# VarifyDirectories <sub>*function*</sub>
Makes sure the output folders exist 
```cpp
void VarifyDirectories(const Config& config)
```
# ProcessFiles <sub>*function*</sub>
Creates a JSON tree with the documentation info from the files 
```cpp
void ProcessFiles(const std::vector<std::string>& files, CLT::DataBranch& docTree, const Config& config)
```
# GetFilesToProcess <sub>*function*</sub>
Searches the input folder for files with the specified extinsions 
```cpp
std::vector<std::string> GetFilesToProcess(const std::string& currentPath, const Config& config)
```
Return
: Returns a list of paths 
