# Enigma MDOC
Enigma MDOC is a tool for generating MarkDown documentation from the comments in your code.

# How to use
By default the command tag that comes before every command is `@EMDOC`.
You can put any of the following commands after the command tag

| Name   | Description                                     |
| ------ | ----------------------------------------------- |
| DEF    | Used to define anything                         |
| PARAM  | Used to define the parameters of a function     |
| RETURN | Used to define the return value of a function   |
| NDEF   | Must follow every DEF command                   |

The source code for this project is the example of the EMDOC comments

Before you can use EnigmaMDOC you need to create a configuration file called `EMDOC.json` in the same directory you're running the program in. It should look something like the following
```json
{
    "output_path": "docs",
    "input_path": "src",
    "command_tag": "@EMDOC",
    "create_JSON_int": true,
    "extensions": [
        [".h", "C++"],
        [".hpp", "C++"]
    ]
}
```
output_path
: The directory all generated MarkDown files will be placed in

input_path
: The directory EnigmaMDOC will search thourgh to find source code files

command_tag
: The tag EnigmaMDOC searches for in the source code comments that comes right before the command

create_JSON_int
: Tells EnigmaMDOC to output the intermediate JSON to a file. EnigmaMDOC stores the description information with a JSON tree before generating any MarkDown

extensions
: The file extensions that EnigmaMDOC looks for and the language the file is in
: curently the only supported languages are `C` and `C++`

# TODO:
- [ ] support python
- [ ] support bash
- [ ] support enum / enum classes
- [ ] support macros
- [ ] nested sections
- [x] classes/structs
- [x] variables