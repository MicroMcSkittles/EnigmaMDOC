 
# Chunk
Stores a piece of MarkDown source code so that the final file can be ordered properly
## Variables
| Name      | Type          | Description                                                               | Default Value | 
| --------- | ------------- | ------------------------------------------------------------------------- | ------------- | 
| type      | `ChunkType`   | The kind of chunk, certen types are placed higher then others in the file |               | 
| value     | `std::string` | The MarkDown source code for the chunk                                    |               | 
| separator | `bool`        | Places a separator at the end of the chunk if true                        | `false`       | 



---
# Scope
Used to organize the differant MarkDown chunks
## Variables
| Name      | Type                 | Description                                         | Default Value | 
| --------- | -------------------- | --------------------------------------------------- | ------------- | 
| name      | `std::string`        | The name that will be displayed in the scope header |               | 
| chunks    | `std::vector<Chunk>` | Every chunk in the scope                            |               | 
| scopes    | `std::vector<Scope>` | Every child scope                                   |               | 
| separator | `bool`               | Places a separator at the end of the chunk if true  | `false`       | 



---
# Mark Down Functions
# CreateTableString
Creates a MarkDown formated table from the Command Line Tools Table
```C++
std::string CreateTableString(const CLT::Table& table, uint8_t depth)
```
## Parameters
| Name  | Type                | Description                                    | 
| ----- | ------------------- | ---------------------------------------------- | 
| table | `const CLT::Table&` | The Command Line Tools table                   | 
| depth | `uint8_t`           | The configuration that was loaded in from file | 

---
# CreateHeader
Creates a MarkDown header from a depth value
```C++
std::string CreateHeader(uint8_t depth)
```
## Parameters
| Name  | Type      | Description                                             | 
| ----- | --------- | ------------------------------------------------------- | 
| depth | `uint8_t` | The depth, the higher the number the smaller the header | 

---

