#ifndef QUADS_HH
#define QUADS_HH

#include "Token.hh"
#include <vector>


enum scope_space : int {
    program_variable,
    function_variable,
    formal_variable_al
};

enum symbol_type : int {
    variable_al,
    user_function,
    library_function
};

enum action_command : int {
    assignation,        addition,           subtraction,
    multiplication,     division,           module,
    uminus,             AND,                OR,
    NOT,                if_equal,           if_not_equal,
    if_less_equal,      if_greater_equal,   if_less,
    if_greater,         call,               parameters,
    RETURN,             get_return_value,   function_start,
    function_end,       table_creation,     table_get_element,
    table_set_element
};


class Quads {
private:
    action_command action;
    Token*         result;
    Token*         arg1;
    Token*         arg2;
    int            label;
    int            line;

public:
    Quads(action_command action, Token* result, Token* arg1, Token* arg2, int label, int line):
    action(action), result(result), arg1(arg1), arg2(arg2), label(label), line(line){};

    void print(){
        std::cout << label << ":\t" << result->getLexem() << "\t" << arg1->getLexem();

        if(arg2) std::cout << arg2->getLexem() << "\t";
        
        std::cout << "\t" << label << std::endl;
    }
};

extern vector<Quads> quads_list;

#endif