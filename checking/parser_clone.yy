%language "C++"
%skeleton "lalr1.cc"
%require "3.2"
%define parse.trace
%defines

%code requires{
        #include "Token.hh"

        namespace yy {
                class Linker;
                class Lexer;
        }

        struct lvalue_val{const Token* token_val; bool is_declared_local;};


}

%parse-param {Lexer& lexer}
%parse-param {Linker& linker}


%code{
        #include "symbol_table.hh"
        #include "al.hh"
        #include "linker.hh"
        #include "intermediate_code_actions.hh"

        #undef yylex
        #define yylex lexer.yylex

        using token = yy::parser::token;

        extern vector<Token> tokenList;
        extern int noTokens;
        extern int line;

        extern int yydebug_;
        vector<Token> unnamed_functions;
        vector<Quads> quads_list;
        vector<Token> token_stack;
        stack<Quads*> function_stack;
        queue<Token*> parameters_queue;



        Symbol_Table symbol_table;

        bool local_id = false;

        int function_num = 0;
        int label = 1;
        int num_param = 0;
        bool increment_scope = true;

        auto add_unnamed_function() -> void ;
        auto assignexpr_action(const bool local, const Token* token) -> void ;
        auto lvalue_action(bool local, const Token* token) -> void ;
        auto incrementing_lvalue_action(const bool local, const Token* token, const bool prefix) -> void ;
        auto decrementing_lvalue_action(const bool local, const Token* token, const bool prefix) -> void ;
        int test(int first, int op, int second) ;


        
}



%define api.value.type variant
%define parse.assert

%type <lvalue_val> lvalue
%type <lvalue_val> assignexprlvalue
%type <const Token*> term
%type <const Token*> expr
%type <const Token*> primary
%type <const Token*> const


%locations




%token IF 
%token ELSE
%token WHILE
%token FOR
%token FUNCTION
%token RETURN
%token BREAK
%token CONTINUE
%token AND
%token NOT
%token OR
%token LOCAL
%token TRUE
%token FALSE
%token NIL
%token ASSIGNMENT
%token ADDITION
%token SUBTRACTION
%token MULTIPLICATION
%token DIVISION
%token MODULO
%token EQUAL
%token NOT_EQUAL
%token INCREMENT
%token DECREMENT
%token GREATER
%token LESSER
%token GREATER_EQUAL
%token LESSER_EQUAL
%token LEFT_BRACKET
%token RIGHT_BRACKET
%token LEFT_SUBSCRIPT                     // '['
%token RIGHT_SUBSCRIPT                    // ']' 
%token LEFT_PARENTHESIS
%token RIGHT_PARENTHESIS
%token SEMICOLON
%token COMMA
%token MEMBER_INITIALISER_LIST            // :
%token SCOPE_RESOLUTION                   // ::
%token STRUCTURE_REFERENCE                // .
%token DOUBLE_FULL_STOP                   // ..
%token ID
%token INTCONST
%token REALCONST
%token STR
%token NESTED_COMMENT
%token BLOCK_COMMENT
%token LINE_COMMENT



//Priority rules
%right ASSIGNMENT
%nonassoc REALCONST INTCONST STR NIL TRUE FALSE ID
%nonassoc ELSE
%left OR
%left AND
%nonassoc EQUAL NOT_EQUAL
%nonassoc GREATER GREATER_EQUAL LESSER LESSER_EQUAL
%left ADDITION SUBTRACTION
%left MULTIPLICATION DIVISION MODULO
%nonassoc NOT INCREMENT DECREMENT
%left LEFT_PARENTHESIS RIGHT_PARENTHESIS
%left UMINUS
%left COMMA



%start programm

//End of priority rules

%%

programm:   stmts {}
            ;


stmts : stmt stmtreduce
        ;


stmtreduce: stmt stmtreduce
            |
            ;


stmt:   expr SEMICOLON {reset_temporary_counter();}
        | ifstmt
        | whilestmt
        | forstmt
        | returnstmt
        | BREAK SEMICOLON
        | CONTINUE SEMICOLON
        | block
        | funcdef
        | ignore
        | error SEMICOLON {cerr << "Add semicolon." << endl; yyerrok;}
        | error LEFT_PARENTHESIS {}
        ;


expr:   assignexpr {quads_assignment();}
        | expr ADDITION expr {$$=quads_operation(action_commands::addition);}
        | expr SUBTRACTION expr {$$=quads_operation(action_commands::subtraction);}
        | expr MULTIPLICATION expr {$$=quads_operation(action_commands::multiplication);}
        | expr DIVISION expr {$$=quads_operation(action_commands::division);}
        | expr MODULO expr {$$=quads_operation(action_commands::module);}
        | expr GREATER expr {$$=quads_operation(action_commands::if_greater);}
        | expr GREATER_EQUAL expr {$$=quads_operation(action_commands::if_greater_equal);}
        | expr LESSER expr {$$=quads_operation(action_commands::if_less);}
        | expr LESSER_EQUAL expr {$$=quads_operation(action_commands::if_less_equal);}
        | expr EQUAL expr {$$=quads_operation(action_commands::if_equal);}
        | expr NOT_EQUAL expr {$$=quads_operation(action_commands::if_not_equal);}
        | expr AND expr {$$=quads_operation(action_commands::AND);}
        | expr OR expr {$$=quads_operation(action_commands::OR);}
        | term                                              {}



term:   LEFT_PARENTHESIS expr RIGHT_PARENTHESIS    {$$=nullptr;}
        | SUBTRACTION term %prec UMINUS            {$$=$2; quads_uminus();}
        | NOT expr                                 {$$=nullptr;}
        | INCREMENT lvalue                         {$$=nullptr;token_stack.push_back(tokenList.back());incrementing_lvalue_action($2.is_declared_local, $2.token_val, true);}
        | DECREMENT lvalue                         {$$=nullptr;token_stack.push_back(tokenList.back());decrementing_lvalue_action($2.is_declared_local, $2.token_val, true);}
        | lvalue INCREMENT                         {$$=nullptr;token_stack.push_back(tokenList.back());incrementing_lvalue_action($1.is_declared_local, $1.token_val, false);}
        | lvalue DECREMENT                         {$$=nullptr;token_stack.push_back(tokenList.back());decrementing_lvalue_action($1.is_declared_local, $1.token_val, false);}
        | primary                                  {$$=$1;token_stack.push_back(tokenList.back());}      
        ;

assignexpr: assignexprlvalue ASSIGNMENT expr {}
            ;

assignexprlvalue: lvalue {token_stack.push_back(tokenList.back()); $$=$1; assignexpr_action($1.is_declared_local, $1.token_val);}
                  ;

primary:    lvalue {$$=$1.token_val; lvalue_action($1.is_declared_local, $$);}
            | call {}
            | objectdef
            | LEFT_PARENTHESIS funcdef RIGHT_PARENTHESIS
            | const {$$ = $1;}
            ;

lvalue: ID                      {$$.token_val = &tokenList.back(); $$.is_declared_local = false;} 
        | LOCAL ID              {$$.token_val = &tokenList.back(); $$.is_declared_local = true;}
        | SCOPE_RESOLUTION ID   {$$.token_val = const_cast<Token*>(symbol_table.search_global_id(tokenList.back().getLexem<string>())); $$.is_declared_local = false;}
        | member                {$$.token_val = nullptr; $$.is_declared_local = false;}
        ;

member: lvalue STRUCTURE_REFERENCE ID                   {}
        | lvalue LEFT_SUBSCRIPT expr RIGHT_SUBSCRIPT    {}
        | call STRUCTURE_REFERENCE ID                   {}
        | call LEFT_SUBSCRIPT expr RIGHT_SUBSCRIPT      {}
        ;

call:   call LEFT_PARENTHESIS elist RIGHT_PARENTHESIS {quads_start_call();}
        | lvalue callsufix                            {quads_start_call();}
        | LEFT_PARENTHESIS funcdef RIGHT_PARENTHESIS LEFT_PARENTHESIS elist RIGHT_PARENTHESIS {quads_start_call();}
        ;

callsufix:  normcall
            | methodcall
            ;

normcall:   LEFT_PARENTHESIS elist RIGHT_PARENTHESIS
            ;

methodcall: DOUBLE_FULL_STOP ID LEFT_PARENTHESIS elist RIGHT_PARENTHESIS
            ;

elist:  LEFT_SUBSCRIPT expressionlist RIGHT_SUBSCRIPT
        ;

expressionlist: expr 
                | expr COMMA expressionlist
                ;


objectdef:  LEFT_SUBSCRIPT objectdeflist RIGHT_SUBSCRIPT   
            ;

objectdeflist: elist | indexed ;

indexed:    LEFT_SUBSCRIPT indexedelemlist RIGHT_SUBSCRIPT 
            ;

indexedelemlist: indexedelem
                 | indexedelem COMMA indexedelemlist
                 ;

indexedelem:    LEFT_BRACKET expr MEMBER_INITIALISER_LIST expr RIGHT_BRACKET
                ;



block:  blockopenscope stmts blockclosescope
        ;

blockopenscope: LEFT_BRACKET { if(increment_scope) symbol_table.initialise_new_scope(); }
                ;

blockclosescope: RIGHT_BRACKET { increment_scope=true; symbol_table.conclude_scope(); }
                 ;



funcdef:    FUNCTION functioname funcdefopenscope idlist RIGHT_PARENTHESIS block {quads_func_end();}
            ;

functioname: ID {token_stack.push_back(tokenList.back()); quads_func_start(); symbol_table.define_function(&tokenList.back());}
             |  {add_unnamed_function(); symbol_table.define_function(&unnamed_functions.back());} 
             ;

funcdefopenscope: LEFT_PARENTHESIS { increment_scope=false; symbol_table.initialise_new_scope(); }
                  ;



const:  REALCONST  {$$=&tokenList.back();}
        | INTCONST {$$=&tokenList.back();}
        | STR      {$$=&tokenList.back();}
        | NIL      {$$=&tokenList.back();}
        | TRUE     {$$=&tokenList.back();}
        | FALSE    {$$=&tokenList.back();}
        ;

idlist: idlistarguments {}
        |
        ;

idlistarguments: idlistsingle
                 | idlistmultiple idlistarguments


idlistsingle: ID                {symbol_table.search_formal_identifier(&tokenList.back());}
              ; 

idlistmultiple: ID COMMA        {symbol_table.search_formal_identifier(&tokenList.back());}
                ;

ifstmt: IF LEFT_PARENTHESIS expr RIGHT_PARENTHESIS stmt 
        | IF LEFT_PARENTHESIS expr RIGHT_PARENTHESIS stmt ELSE stmt
        ;


whilestmt:  WHILE LEFT_PARENTHESIS expr RIGHT_PARENTHESIS stmt
            ;

forstmt:    FOR LEFT_PARENTHESIS elist SEMICOLON expr SEMICOLON elist RIGHT_PARENTHESIS stmt
            ;




returnstmt: RETURN returnstmts  {}
            ;

returnstmts: expr SEMICOLON {}
             | SEMICOLON
             | error SEMICOLON {cout<<"edweisai"<<endl;}
             ;


ignore: BLOCK_COMMENT 
        | LINE_COMMENT 
        | NESTED_COMMENT 
        ;



%%


auto print_system_error() -> void {
        std::cerr << "System error. Aborting" << std::endl;
        exit(1);
}

auto add_unnamed_function() -> void {
        std::string number = to_string(function_num++);
        const std::string lexem = "$" + number;
        unnamed_functions.emplace_back(Token(lexem, 260, line, ++noTokens));
}


auto assignexpr_action(const bool local, const Token* token) -> void {
        //if(token == nullptr) print_system_error();
        
        if(local) symbol_table.lvalue_declare_local_identifier(token); else symbol_table.lvalue_search_variable(token);
        
}

auto lvalue_action(bool local, const Token* token) -> void {
        //if(token == nullptr) print_system_error();
        
        if(local) symbol_table.declare_local_identifier(token); else symbol_table.search_variable(token);
}

auto incrementing_lvalue_action(const bool local, const Token* token, const bool prefix) -> void {
        //if(token == nullptr) print_system_error();
        if(local) symbol_table.lvalue_declare_local_identifier(token); else symbol_table.search_variable(token);
        quads_increment(prefix);
}

auto decrementing_lvalue_action(const bool local, const Token* token, const bool prefix) -> void {
        //if(token == nullptr) print_system_error();

        if(local) symbol_table.lvalue_declare_local_identifier(token); else symbol_table.lvalue_search_variable(token);
        quads_decrement(prefix);
}


void yy::parser::error(const yy::location& loc, const string& msg){
        cout <<"Syntax error at " << loc << ": " << msg << endl;
}


int yywrap(){return 1;}

int main(int argc, char **argv) {
   if(argc != 2) { std::cerr << "Invalid number of arguments. Aborting" << std::endl; exit(1); }
   
   yy::Linker linker;
   token_stack.reserve(100);
   tokenList.reserve(100);
   quads_list.reserve(100);
   linker.link(argv[1]);
   print();
   symbol_table.print();

   return 0;
}


int test(int first, int op, int second){
        cout << first << ", " << second << endl;
        if (op == token::ADDITION) return (first+second);
        else if (op == token::SUBTRACTION) return (first-second);
        else return (first*second);
}

