# Enigma MDOC
Enigma MDOC is a tool for generating MarkDown documentation from the comments in your code.

# How to use
By default the command tag that comes before every command is `@EMDOC`.
You can put any of the following commands after the command tag

| Name   | Description                                     |
| ------ | ----------------------------------------------- |
| DEF    | Used to define anything                         |
| ELEM   | Used to define variables in structs and classes |
| PARAM  | Used to define the parameters of a function     |
| RETURN | Used to define the return value of a function   |
| NDEF   | Must follow every DEF command                   |
| SEC    | Defines a section in your code                  |
| NSEC   | Ends a section in your code                     |

The source code for this project is the example of the EMDOC comments

# TODO:
- JSON configuration file
- support enum / enum classes
- support macros
- nested sections
- global variables
- using DEF instead of ELEM for member variables