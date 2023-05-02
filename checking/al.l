%option c++
%option yyclass="yy::Lexer"
%option debug
%option nodefault
%option noyywrap

%{
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
#include "Token.hh"
#include "al.hh"

#undef  YY_DECL
#define YY_DECL int yy::Lexer::yylex( yy::parser::semantic_type * const lval, yy::parser::location_type *location )
#define yyterminate() return 0

using namespace std;
using token = yy::parser::token;


int line = 1;
int value = 256;
int noTokens = 0;
int type;
int nestedComments = 0;
bool cont = true;
char* text;
constexpr const char* lineCommentText = "";

// Holds a string's content.
string buffer;



//===============================================================================================

Token* dummy = nullptr; // Dummy variable.

// Holds the program's tokens.
vector<Token> tokenList;

// Auxiliary data structure for block-comment directive.
queue<shared_ptr<string>> q;
stack<shared_ptr<string>> s;


//===============================================================================================


// This signature has been mandated by the proffessor.
const int alpha_yylex(void* ylval);
const bool isCommentsEnd(void);
void clearTemp(void);
void pushInQueue(void);
void addTokenComment(void);


//===============================================================================================


%}

identifier [a-zA-Z]([a-zA-Z_]|[0-9])* 
integerconstant [0-9]+
realconstant [0-9]+\.[0-9]+
whitespace [ \t\a\b\f\v\r]+
newline [\n] 

%x STRINGS

%%

{newline} {++line; loc->lines(yyleng); loc->step();} // \n increments the line.
{whitespace} {loc->step();} // Ignores white spaces.


if {text=yytext; type=token::IF; ++noTokens; return alpha_yylex(dummy);}
else {text=yytext; type=token::ELSE; ++noTokens; return alpha_yylex(dummy);}
while {text=yytext; type=token::WHILE; ++noTokens; return alpha_yylex(dummy);}
for {text=yytext; type=token::FOR; ++noTokens; return alpha_yylex(dummy);}
function {text=yytext; type=token::FUNCTION; ++noTokens; return alpha_yylex(dummy);}
return {text=yytext; type=token::RETURN; ++noTokens; return alpha_yylex(dummy);}
break {text=yytext; type=token::BREAK; ++noTokens; return alpha_yylex(dummy);}
continue {text=yytext; type=token::CONTINUE; ++noTokens; return alpha_yylex(dummy);}
and {text=yytext; type=token::AND; ++noTokens; return alpha_yylex(dummy);}
not {text=yytext; type=token::NOT; ++noTokens; return alpha_yylex(dummy);}
or {text=yytext; type=token::OR; ++noTokens; return alpha_yylex(dummy);}
local {text=yytext; type=token::LOCAL; ++noTokens; return alpha_yylex(dummy);}
true {text=yytext; type=token::TRUE; ++noTokens; return alpha_yylex(dummy);}
false {text=yytext; type=token::FALSE; ++noTokens; return alpha_yylex(dummy);}
nil {text=yytext; type=token::NIL; ++noTokens; return alpha_yylex(dummy);}


{identifier}  {text=yytext; type=token::ID; ++noTokens; return alpha_yylex(dummy);}
{integerconstant} {text=yytext; type=token::INTCONST; ++noTokens; return alpha_yylex(dummy);}
{realconstant} {text=yytext; type=token::REALCONST; ++noTokens; return alpha_yylex(dummy);}


"=" {text=yytext; type=token::ASSIGNMENT; ++noTokens; return alpha_yylex(dummy);}
"+" {text=yytext; type=token::ADDITION; ++noTokens; return alpha_yylex(dummy);}
"-" {text=yytext; type=token::SUBTRACTION; ++noTokens; return alpha_yylex(dummy);}
"*" {text=yytext; type=token::MULTIPLICATION; ++noTokens; return alpha_yylex(dummy);}
"/" {text=yytext; type=token::DIVISION; ++noTokens; return alpha_yylex(dummy);}
"%" {text=yytext; type=token::MODULO; ++noTokens; return alpha_yylex(dummy);}
"==" {text=yytext; type=token::EQUAL; ++noTokens; return alpha_yylex(dummy);}
"!=" {text=yytext; type=token::NOT_EQUAL; ++noTokens; return alpha_yylex(dummy);}
"++" {text=yytext; type=token::INCREMENT; ++noTokens; return alpha_yylex(dummy);}
"--" {text=yytext; type=token::DECREMENT; ++noTokens; return alpha_yylex(dummy);}
">" {text=yytext; type=token::GREATER; ++noTokens; return alpha_yylex(dummy);}
"<" {text=yytext; type=token::LESSER; ++noTokens; return alpha_yylex(dummy);}
">=" {text=yytext; type=token::GREATER_EQUAL; ++noTokens; return alpha_yylex(dummy);}
"<=" {text=yytext; type=token::LESSER_EQUAL; ++noTokens; return alpha_yylex(dummy);}


"(" {text=yytext; type=token::LEFT_PARENTHESIS; ++noTokens; return alpha_yylex(dummy);}
")" {text=yytext; type=token::RIGHT_PARENTHESIS; ++noTokens; return alpha_yylex(dummy);}
"{" {text=yytext; type=token::LEFT_BRACKET; ++noTokens; return alpha_yylex(dummy);}
"}" {text=yytext; type=token::RIGHT_BRACKET; ++noTokens; return alpha_yylex(dummy);}
"[" {text=yytext; type=token::LEFT_SUBSCRIPT; ++noTokens; return alpha_yylex(dummy);}
"]" {text=yytext; type=token::RIGHT_SUBSCRIPT; ++noTokens; return alpha_yylex(dummy);}
";" {text=yytext; type=token::SEMICOLON; ++noTokens; return alpha_yylex(dummy);}
"," {text=yytext; type=token::COMMA; ++noTokens; return alpha_yylex(dummy);}
":" {text=yytext; type=token::MEMBER_INITIALISER_LIST; ++noTokens; return alpha_yylex(dummy);}
"::" {text=yytext; type=token::SCOPE_RESOLUTION; ++noTokens; return alpha_yylex(dummy);}
"." {text=yytext; type=token::STRUCTURE_REFERENCE; ++noTokens; return alpha_yylex(dummy);}
".." {text=yytext; type=token::DOUBLE_FULL_STOP; ++noTokens; return alpha_yylex(dummy);}



\" {BEGIN(STRINGS); type=token::STR; buffer.clear();} /* Upon encountering a quotation mark, review the string's format validity.*/
<STRINGS>\\\" {buffer.push_back('\"');}
<STRINGS>\\n {buffer.push_back('\n');}
<STRINGS>\\t {buffer.push_back('\t');}
<STRINGS>\\a {buffer.push_back('\a');}
<STRINGS>\\b {buffer.push_back('\b');}
<STRINGS>\\f {buffer.push_back('\f');}
<STRINGS>\\r {buffer.push_back('\r');}
<STRINGS>\\v {buffer.push_back('\v');}
<STRINGS>\\' {buffer.push_back('\'');}
<STRINGS>\\? {buffer.push_back('\?');}
<STRINGS>\\. {buffer.push_back('\\');}
<STRINGS>\n {cerr << "String in line: " << line << " never closes. ABORT." << endl; exit(1);}
<STRINGS><<EOF>> {cerr << "String in line: " << line << " never closes. ABORT." << endl; exit(1);}
<STRINGS>\" {BEGIN(INITIAL); ++noTokens; text=buffer.data(); return alpha_yylex(dummy);} // Terminal condition.
<STRINGS>. {buffer.push_back(*yytext);}

. {cerr<< "Error: identifier: \'"<< yytext << "\', in line: "<< line << " is invalid." << endl; cont = false; exit(0);} // If an invalid identifier is encountered, report the mistake and exit the lexical analyser.


%%

// This signature has been mandated by the proffessor.
const int alpha_yylex(void* ylval){

    if(type == token::ID || type == token::STR || type == token::INTCONST || type == token::REALCONST) 
        tokenList.emplace_back(Token(text, type, line, noTokens));
    
    else --noTokens;

    return type;

}

// Observer that checks if the comment has ended.
// It also notes this comment's ending line, since it's evoked
// only upon encountering */ .
const bool isCommentsEnd(){
    auto tmp = s.top();

    to_chars(tmp->data() + tmp->size(), tmp->data() + tmp->capacity(), -line);
    s.pop();

    return (nestedComments == 0) ? true : false;
}


// Pushes a new element in stack.
void pushInQueue(){
    // Reserves 15 positions, turns the line to string,
    // and then resizes the string to its actual current size.
    // Resize won't reallocate memory, it will simply update its size. 
    auto tmp = make_shared<string>(15, 0);

    auto [ends, er] = to_chars(tmp->data(), tmp->data() + tmp->size(), line);
    tmp->resize(ends - tmp->data());

    q.emplace(tmp);
    s.push(tmp);
}


// Adds a new token and appends the comment's ending line.
void addTokenComment(){
    auto element = q.front();
    auto beginning = element->data();

    int tmpLine = line;
    text = element->data();

    from_chars(beginning, beginning + element->capacity(), line);
    ++noTokens;
    
    alpha_yylex(dummy);
    line = tmpLine;

    q.pop();
}

