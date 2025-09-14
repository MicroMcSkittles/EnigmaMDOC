# CommentBlock <sub>*struct*</sub>
Stores the information needed to parse comments
## Variables
| Name  | Type          | Description                                 | Default Value     |
|-------|---------------|---------------------------------------------|-------------------|
| text  | `std::string` | All of the text in a comment                | ""                |
| start | `size_t`      | Where the command starts in the source code | std::string::npos |
| line  | `size_t`      | The line the command is on in the file      | std::string::npos |
| end   | `size_t`      | Where the command ends in the source code   | std::string::npos |
## FindCommandType <sub>*function*</sub>
Returns the type tag that follows the command tag 
```cpp
std::string FindCommandType(const Config& config) const
```
### Parameters
| Name   | Type            | Description                                    |
|--------|-----------------|------------------------------------------------|
| config | `const Config&` | The configuration that was loaded in from file |


---

# FindCommentBlocks <sub>*function*</sub>
Looks through a file and finds comments that contain the command tag
```cpp
std::vector<CommentBlock> FindCommentBlocks(const std::string& filename, const Config& config)
```
## Parameters
| Name     | Type                 | Description                                    |
|----------|----------------------|------------------------------------------------|
| config   | `const Config&`      | The configuration that was loaded in from file |
| filename | `const std::string&` | The specific file that is being parsed         |

Return
: Returns a list of the comment blocks that contain command tags
