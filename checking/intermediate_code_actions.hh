#ifndef ACTIONS_HH
#define ACTIONS_HH

#include "symbol_table.hh"
#include "parser.tab.hh"

using namespace std;


Token dummyToken = Token("",0,0,0);
Token incrdecrToken = Token("1",0,0,0);
Token returnToken = Token("_t0",0,0,0);

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

enum action_commands : int {
    assignment,        addition,           subtraction,
    multiplication,     division,           module,
    uminus,             AND,                OR,
    NOT,                if_equal,           if_not_equal,
    if_less_equal,      if_greater_equal,   if_less,
    if_greater,         call,               parameters,
    RETURN,             get_return_value,   function_start,
    function_end,       table_creation,     table_get_element,
    table_set_element,  jump
};

class Quads {
public:

    int    action;
    Token  result;
    Token  arg1;
    Token  arg2;
    int    quad_num;
    int    label;
    int    line;

    Quads(const int action, const Token result, const Token arg1, const Token arg2, const int label, const int line):
    action(action), result(result), arg1(arg1), arg2(arg2), quad_num(label), label(-1), line(line){};
    Quads(){};

    auto set_jmp_label(const int jmp) {label = jmp;}

    void print(){
        string operation;

        switch(action){
            case action_commands::assignment: operation = "assignment"; break;
            case action_commands::addition: operation = "addition"; break;
            case action_commands::subtraction: operation = "subtraction"; break;
            case action_commands::multiplication: operation = "multiplication"; break;
            case action_commands::division: operation = "division"; break;
            case action_commands::module: operation = "module"; break;
            case action_commands::uminus: operation = "uminus"; break;
            case action_commands::AND: operation = "AND"; break;
            case action_commands::OR: operation = "OR"; break;
            case action_commands::NOT: operation = "NOT"; break;
            case action_commands::if_equal: operation = "if_equal"; break;
            case action_commands::if_not_equal: operation = "if_not_equal"; break;
            case action_commands::if_less_equal: operation = "if_less_equal"; break;
            case action_commands::if_greater_equal: operation = "if_greater_equal"; break;
            case action_commands::if_less: operation = "if_less"; break;
            case action_commands::if_greater: operation = "if_greater"; break;
            case action_commands::call: operation = "call"; break;
            case action_commands::parameters: operation = "parameters"; break;
            case action_commands::RETURN: operation = "RETURN"; break;
            case action_commands::get_return_value: operation = "get_return_value"; break;
            case action_commands::function_start: operation = "function_start"; break;
            case action_commands::function_end: operation = "function_end"; break;
            case action_commands::table_creation: operation = "table_creation"; break;
            case action_commands::table_get_element: operation = "table_get_element"; break;
            case action_commands::table_set_element: operation = "table_set_element"; break;
            case action_commands::jump: operation = "jump"; break;
            default: cerr << "System error. Aborting" << endl; exit(1);
        }
                
        std::cout <<  quad_num << ": " << operation << "\t\t" << result.getLexem() << "\t" << arg1.getLexem() << "\t" << arg2.getLexem();
        if(label != -1) cout << "\t" << label;
        cout << endl;
        
    }

    auto test() -> void {cout<<"labe: "<<label<<endl;cout<<"jump: "<<action<<endl;if(label == -1) cout<<"didnt change"<<endl;if(action!=action_commands::jump)cout<<"jump NOOOOO"<<endl;}
};



extern vector<Quads> quads_list;
extern vector<Token> token_stack;
extern Symbol_Table symbol_table;
extern vector<Token> tokenList;
extern stack<Quads*> function_stack;
extern queue<Token*> parameters_queue;
extern int label;
extern int num_param;
int temporary_counter = 0;


//==========================================================================

// Temporary variable names

inline auto reset_temporary_counter() -> void {
    temporary_counter = 0;
}

inline auto get_new_temporary_variable_name(const Token* token) -> Token* {
    
    const std::string& name = "_t" + to_string(temporary_counter++);
    Token* returning_token = symbol_table.search_current_scope(token, name);

    return returning_token;
}

//==========================================================================


inline auto set_quad(const int action, Token result, Token arg1, Token arg2, const int label, const int line) -> void {
    quads_list.emplace_back(Quads(action, result, arg1, arg2, label, line));
}


//==========================================================================


// AUXILIARY functions

auto is_number(const Token* operative) -> const bool{
    int lexical_token = operative->getLexicalToken();
    return (lexical_token >= 299 && lexical_token <= 301);
}

//==========================================================================


// Quads-related opeartion functions

auto quads_assignment() -> void {

    Token arg1 = token_stack.back();
    token_stack.pop_back();
    Token result = token_stack.back();
    token_stack.pop_back();
    Token* second_assignment = get_new_temporary_variable_name(&result);


    set_quad(action_commands::assignment, result, arg1, dummyToken, label++, result.getLine());
    set_quad(action_commands::assignment, *second_assignment, result, dummyToken, label++, result.getLine());   

    token_stack.push_back(*second_assignment);
}


auto quads_uminus() -> void {
    Token arg1 = token_stack.back();
    token_stack.pop_back();
    
    Token* result = get_new_temporary_variable_name(&arg1);
    set_quad(action_commands::uminus, *result, arg1, dummyToken, label++, result->getLine());

    token_stack.push_back(*result);
}


// KANE void. Allakse ta %type, einai axreiasta.
auto quads_operation(const int op) -> Token* {

    Token arg2 = token_stack.back();
    token_stack.pop_back();
    Token arg1 = token_stack.back();
    token_stack.pop_back();

    if(!is_number(&arg1) || !is_number(&arg2)) {std::cerr << "Error: Operand must be a numeric constant or a numeric variable." << std::endl; return nullptr; }

    Token* result = get_new_temporary_variable_name(&arg2);
    set_quad(op, *result, arg1, arg2, label++, result->getLine());
    token_stack.push_back(*result);


    return &token_stack.back();
}


auto quads_increment(const bool prefix) -> void {
    Token arg1 = tokenList.back();
    Token arg2 = incrdecrToken;
    Token* result = nullptr;
    

    if(prefix){
        set_quad(action_commands::addition, arg1, arg1, arg2, label++, arg1.getLine());
        result = get_new_temporary_variable_name(&arg1);
        set_quad(action_commands::assignment, *result, arg1, dummyToken, label++, result->getLine());
    }
    else {
        result = get_new_temporary_variable_name(&arg1);
        set_quad(action_commands::assignment, *result, arg1, arg2, label++, result->getLine());
        set_quad(action_commands::addition, arg1, arg1, dummyToken, label++, result->getLine());
    }

    token_stack.push_back(*result);
}

auto quads_decrement(const bool prefix) -> void {
    Token arg1 = tokenList.back();
    Token arg2 = incrdecrToken;
    Token* result = nullptr;
        
    if(prefix) {
        set_quad(action_commands::subtraction, arg1, arg1, arg2, label++, arg1.getLine());
        result = get_new_temporary_variable_name(&arg1);
        set_quad(action_commands::assignment, *result, arg1, dummyToken, label++, arg1.getLine());
    }
    else {
        result = get_new_temporary_variable_name(&arg1);
        set_quad(action_commands::assignment, arg1, arg1, arg2, label++, arg1.getLine()); 
        set_quad(action_commands::subtraction, *result, arg1, dummyToken, label++, arg1.getLine());
    }

    token_stack.push_back(*result);
}

//==========================================================================


// function operations-related quads.

auto quads_func_start() -> void {
    set_quad(action_commands::jump, dummyToken, dummyToken, dummyToken, label++, -1);
    function_stack.push(&quads_list.back());
    set_quad(action_commands::function_start, token_stack.back(), dummyToken, dummyToken, label++, -1);
}

auto quads_func_end() -> void {
    set_quad(action_commands::RETURN, returnToken, dummyToken, dummyToken, label++, -1);
    set_quad(action_commands::jump, dummyToken, dummyToken, dummyToken, label++, -1);
    quads_list.back().set_jmp_label(label);
    set_quad(action_commands::function_end, dummyToken, dummyToken, dummyToken, label++, -1);
    function_stack.top()->set_jmp_label(label);
    function_stack.pop();
}


auto quads_start_call() -> void {
    
    while(!parameters_queue.empty()){
        Token* result = parameters_queue.back();
        set_quad(action_commands::parameters, *result, dummyToken, dummyToken, label++, -1);
        parameters_queue.pop();
    }

    set_quad(action_commands::call, token_stack.back(), dummyToken, dummyToken, label++, -1);
    token_stack.pop_back();
    set_quad(action_commands::get_return_value, returnToken, dummyToken, dummyToken, label++, 1);
}

auto quads_end_call() -> void {
    set_quad(action_commands::call, token_stack.back(), dummyToken, dummyToken, label++, -1);
    token_stack.pop_back();
}


auto print() -> void {
    for(auto quads : quads_list) quads.print();
}
#endif