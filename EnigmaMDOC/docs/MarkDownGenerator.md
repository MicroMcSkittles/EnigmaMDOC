# Variables
| Name              | Type                                   | Description                                                    |
|-------------------|----------------------------------------|----------------------------------------------------------------|
| BranchPriorityMap | `extern std::map<std::string, size_t>` | Used for organizing the differant branches based on there type |
# GenerateMarkdown <sub>*function*</sub>
Generates MarkDown source code from a JSON tree
```cpp
void GenerateMarkdown(const CLT::DataBranch& docTree, const Config& config)
```
## Parameters
| Name    | Type                     | Description                                    |
|---------|--------------------------|------------------------------------------------|
| config  | `const Config&`          | The configuration that was loaded in from file |
| docTree | `const CLT::DataBranch&` | The JSON tree                                  |

