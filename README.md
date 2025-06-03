# MiniLang Compiler: Lexical and Syntax Analyzer in C++

## 🧠 Overview

This project implements the frontend of a compiler — including a **Lexical Analyzer (Lexer)** and a **Syntax Analyzer (Parser)** — for a simplified programming language called **MiniLang**. Written in C++, it showcases core compiler design principles, including tokenization, symbol table construction, and recursive-descent parsing.

## 👨‍💻 Author
Aryan Shah

## 📚 Features

- **Lexical Analysis**: Identifies and tokenizes reserved keywords, identifiers, literals, operators, and symbols.
- **Recursive-Descent Parser**: Parses input code using a top-down strategy according to MiniLang grammar rules.
- **Symbol Table Support**: Tracks variable names, types, and scopes across nested blocks.
- **Error Handling**: Basic syntax and lexical error reporting to ensure meaningful compiler feedback.

## 🧪 Supported Language Tokens

Includes over 30 token types such as:
- **Keywords**: `int`, `real`, `bool`, `true`, `false`, `if`, `while`, `switch`, `case`, `public`, `private`
- **Operators**: `+`, `-`, `*`, `/`, `=`, `==`, `!=`, `>=`, `<=`, `>`, `<`
- **Symbols**: `(`, `)`, `{`, `}`, `;`, `,`, `:` 
- **Identifiers**: Variable/function names
- **Literals**: Integers and floating-point numbers

## 🚀 How to Compile and Run

### Prerequisites

- C++ compiler (e.g., g++, clang++)
- C++11 or higher standard

### Compile

```bash
g++ -std=c++11 inputbuf.cc lexer.cc parser.cc -o minilang
```

### Run
```bash
./minilang < input.txt
```
Replace `input.txt` with your source code file written in MiniLang syntax.

## 🧠 Key Concepts Demonstrated
- Input buffering with unget support
- Token classification using state-driven logic
- Recursive-descent parsing for grammar rule validation
- Symbol table structure for scope-aware variable handling
