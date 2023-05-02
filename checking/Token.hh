
#ifndef TOKEN_HH
#define TOKEN_HH

#include <iostream>
#include <vector>
#include <string_view>
#include <iomanip>
#include <string>
#include <memory>
#include <stack>
#include <queue>
#include <charconv>
#include <array>
#include <fstream>

using namespace std;


enum Symbol_Types : int {
    variable = 0,
    formal_variable,
    function,
    global_variable
};

static constexpr std::array<std::string_view, 48> get_type{{
    {"IF", 2},
    {"ELSE", 4},
    {"WHILE", 5},
    {"FOR", 3},
    {"FUNCTION", 8},
    {"RETURN", 6},
    {"BREAK", 5},
    {"CONTINUE", 8},
    {"AND", 3},
    {"NOT", 3},
    {"OR", 2},
    {"LOCAL", 5},
    {"TRUE", 4},
    {"FALSE", 5},
    {"NIL", 3},
    {"ASSIGNMENT", 11},
    {"ADDITION", 8},
    {"SUBTRACTION", 11},
    {"MULTIPLICATION", 14},
    {"DIVISION", 8},
    {"MODULO", 6},
    {"EQUAL", 5},
    {"NOT_EQUAL", 9},
    {"INCREMENT", 10},
    {"DECREMENT", 10},
    {"GREATER", 7},
    {"LESSER", 6},
    {"GREATER_EQUAL", 13},
    {"LESSER_EQUAL", 12},
    {"LEFT_BRACKET", 12},
    {"RIGHT_BRACKET", 13},
    {"LEFT_SUBSCRIPT", 14},
    {"RIGHT_SUBSCRIPT", 15},
    {"LEFT_PARENTHESIS", 16},
    {"RIGHT_PARENTHESIS", 17},
    {"SEMICOLON", 9},
    {"COMMA", 5},
    {"MEMBER_INITIALISER_LIST", 23},
    {"SCOPE_RESOLUTION", 17},
    {"STRUCTURE_REFERENCE", 19},
    {"DOUBLE_FULL_STOP", 16},
    {"ID", 2},
    {"INTCONST", 8},
    {"REALCONST", 9},
    {"STR", 3},
    {"NESTED_COMMENT", 15},
    {"BLOCK_COMMENT", 14},
    {"LINE_COMMENT", 13}
}};

class Token {
private:
    const string lexem;
    const int lexical_token;
    const int line;
    const int no_Tokens;
    int symbol_type;

    void pane(){cout << lexem << endl;}

public:
    Token() : lexem(""), lexical_token(-1), line(-1), no_Tokens(-1), symbol_type(-1) {};
    Token(const string& lexem, const int lexical_token, const int line, const int no_Tokens) : 
    lexem(move(lexem)), lexical_token(lexical_token), line(line), no_Tokens(no_Tokens), symbol_type(-1) {};

    Token(const string& lexem, const int lexical_token, const int line, const int no_Tokens, const int symbol_type) : 
    lexem(lexem), lexical_token(lexical_token), line(line), no_Tokens(no_Tokens), symbol_type(symbol_type) {};

   template <typename T = std::string>
    T getLexem() const;
    
    inline const int getLine() const noexcept {return this->line;}
    
    inline const int getLexicalToken() const noexcept {return this->lexical_token;}

    inline const int getTokensNumber() const noexcept {return this->no_Tokens;}

    inline const int getSymbolType() const noexcept {return this->symbol_type;}

    inline void setSymbolType(const int symbolType) noexcept {this->symbol_type = symbolType;}

    inline Token clone(){return Token(lexem, lexical_token, line, no_Tokens, symbol_type);}

    void print() const {
        std::cout << setw(3) << this->line << ":"<< setw(5) <<"#" << this->no_Tokens << setw(4) << "\"" \
                                    << this->lexem << "\"\t\t" << get_type[this->lexical_token - 256] << "\t\t<-" << "enumerated" << std::endl;
    }

    // Default virtual constructor.
    virtual ~Token() = default;

};

template <>
inline int Token::getLexem<int>() const {
    int lex_int = 0;
    std::from_chars(this->lexem.data(), this->lexem.data() + this->lexem.size(), lex_int);
    return lex_int;
}

template <>
inline double Token::getLexem<double>() const {
    double lex_double = 0.0;
    std::from_chars(this->lexem.data(), this->lexem.data() + this->lexem.size(), lex_double);
    return lex_double;
}

template <>
inline std::string Token::getLexem<std::string>() const {
    return this->lexem;
}



extern std::vector<Token> tokenList;
extern int line;
extern int noTokens;

#endif