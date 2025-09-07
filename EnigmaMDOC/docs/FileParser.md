 
# FindCommentBlocks
Looks through a file and finds comments that contain the command tag
```C++
std::vector<CommentBlock> FindCommentBlocks(const std::string& filename, const Config& config)
```
## Parameters
| Name     | Type                 | Description                                    | 
| -------- | -------------------- | ---------------------------------------------- | 
| filename | `const std::string&` | The specific file that is being parsed         | 
| config   | `const Config&`      | The configuration that was loaded in from file | 

---
# CommentBlock
Stores the information needed to parse comments
## Variables
| Name  | Type          | Description                                 | Default Value       | 
| ----- | ------------- | ------------------------------------------- | ------------------- | 
| text  | `std::string` | All of the text in a comment                | `""`                | 
| start | `size_t`      | Where the comment starts in the source code | `std::string::npos` | 
| end   | `size_t`      | Where the comment ends in the source code   | `std::string::npos` | 

## Functions
### FindCommandType
Returns the type tag that follows the command tag
```C++
std::string FindCommandType(const Config& config = Config()) const
```
#### Parameters
| Name   | Type            | Description                                    | Default Value | 
| ------ | --------------- | ---------------------------------------------- | ------------- | 
| config | `const Config&` | The configuration that was loaded in from file | `Config()`    | 



---
