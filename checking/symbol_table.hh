#ifndef SYMBOL_TABLE_HH
#define SYMBOL_TABLE_HH

#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <unordered_map>
#include <algorithm>
#include <iterator>
#include <utility>
#include <ranges>
#include <unordered_set>
#include <stack>
#include <map>

#include "scope.hh"


using namespace std;

    extern vector<Token> tokenList;

    constexpr const int GLOBAL_SCOPE = 0;
    const string redeclared[2] = { {"variable"}, {"function"} };

    class Symbol_Table{

    private:

        std::vector<Scope> scope_variables;                 // Stores active scopes' variables.
        std::queue<Scope> closed_scopes;                   // Stores closed scopes' variables.
        std::unordered_set<std::string> library_functions;  // Stores library function names.
        int scope_num;                                      // Stores active scopes' number - 1.
        
        std::multimap<int, Token> printing;                // Auxiliary data structure for printing. 

        auto insert_token(const string& id, const Token* token, const int symbol_type) -> void ;
        auto insert_token(const string& id, const Token* token, const int symbol_type, const bool access) -> void ;

        // Initialises the library's functions into a different hash table, since they can't be shadowed.
        auto initialise_library_functions() -> void ;


        inline auto formal_id_not_taken(const std::string& identifier) const -> const bool {

            const bool formal_has_been_declared = scope_variables[scope_num].contains(identifier);

            if(formal_has_been_declared)
                cerr << "Error: A formal variable named \"" << identifier << "\" has already been declared." << endl;

            return !formal_has_been_declared;
        }

        // --Checks if the identifier attempts to shadow a library's function.
        auto doesnot_shadow_library_function(const std::string& identifier) const -> const bool {
            bool doesnt_shadow = true;

            if(library_functions.contains(identifier)){
                doesnt_shadow = false;
                std::cerr << "Error: \"" << identifier << "\" corresponds to a library's function, it must not be shadowed." << std::endl;
            }

            return doesnt_shadow;
        }


        inline auto doesnot_redeclare_identifier(const Token* token) const -> const bool {
            
            bool doesnt_redeclare = true;
            
            const std::string& id = token->getLexem<string>();
            const int& new_symbol_type = Symbol_Types::function;

            const auto& id_redeclared = scope_variables[scope_num].find(id);

            if(id_redeclared != scope_variables[scope_num].cend()){
                const int redeclared_symbol_type = id_redeclared->second->getSymbolType();

                if(redeclared_symbol_type != new_symbol_type){
                    doesnt_redeclare = false;
                    const int index = (new_symbol_type == Symbol_Types::function) ? 1 : 0;
                    cerr << "Error: Redeclaration of \"" << id << "\" as " << redeclared[index] << "." << endl;
                }
                else if (new_symbol_type == Symbol_Types::function){
                    doesnt_redeclare = false;
                    cerr << "Error: Function \"" << id << "\" has already been defined within this scope." << endl;
                }

            }

            return doesnt_redeclare;
        }


    //====================================================================================================================================================
        inline auto search_scopes(const std::string& identifier, const Token** token) const -> const std::vector<Scope>::const_reverse_iterator {
            
            auto scope_contains_identifier = [&identifier, &token](const Scope& scope){ 
                auto id = scope.find(identifier);
                if(id != scope.cend()){
                    *token = id->second;
                    return true; 
                }
                else 
                    return false; 
            };

            return std::find_if(scope_variables.crbegin(), scope_variables.crend(), scope_contains_identifier);
        }







        auto search_scopes_starting_from( const reverse_iterator<std::vector<Scope>::const_iterator>& start, const string& identifier, Token** token ) {
            const auto& end = scope_variables.crend();

            auto scope_contains_identifier = [&identifier, &token](const Scope& scope){ 
                auto id = scope.find(identifier);
                if(id != scope.cend()){
                    *token = id->second;
                    return true; 
                }
                else 
                    return false; 
            };

            return std::find_if(start, end, scope_contains_identifier);
        }




        // Helps search where a function starts.
        inline auto is_inside_function(const std::vector<Scope>::const_reverse_iterator& scope) const -> const bool {
            return (scope==scope_variables.crend()) ? true : !scope->can_access_homonymous_variable();
        }



    public:

        Symbol_Table(){
            this->scope_num = 0;
            this->initialise_library_functions();
        }

        // Searches for a given global variable.
        auto search_global_id(const std::string& global_id) const -> const Token* {
            const auto& token = scope_variables[GLOBAL_SCOPE].find(global_id);
            Token* global_token = nullptr;

            if(token == scope_variables[GLOBAL_SCOPE].cend())
                cerr << "Error: Global id \"" << global_id << "\" was not found." << endl;
            else global_token = token->second;
            
            return global_token;
        }

        // Initialises new scope by incrementing scope_num variable.
        inline auto initialise_new_scope() noexcept -> void { scope_variables.emplace_back(++scope_num); }



        // Closes most recently opened scope and insert its variables into the closed_scopes vector.
        inline auto conclude_scope() -> void {
            this->closed_scopes.push(scope_variables[this->scope_num--]);
            this->scope_variables.pop_back();
        }


        inline auto error_conclude_scope() -> void {
            --this->scope_num;
            this->scope_variables.pop_back();
        }

    // Verifies whether an identifier is eligible for initialisation and, if so, initialises it.
        auto declare_local_identifier(const Token* token) -> const Token* {
            const std::string& identifier = token->getLexem<string>();
            Token* returning_token = const_cast<Token*>(token);

            const auto& not_found = scope_variables[scope_num].cend();
            const auto& id = scope_variables[scope_num].find(identifier);

            if(id == not_found){
                if(doesnot_shadow_library_function(identifier)){
                    Symbol_Types type_variable = (scope_num == GLOBAL_SCOPE) ? Symbol_Types::global_variable : Symbol_Types::variable;
                    insert_token(identifier, token, type_variable);
                }
            }
            else 
                returning_token = id->second; 

            
            return returning_token;
        }
        



    auto search_variable(const Token* token) -> const Token* {

        if(token == nullptr || token->getSymbolType() == Symbol_Types::global_variable)
            return token;
        
        const string& identifier = token->getLexem();
        Token* homonymous_token = nullptr;
        bool id_retrieved = false;
        
        auto scope = scope_variables.crbegin();
        const auto& end = scope_variables.crend();
        do {
            auto search_token = scope->find(identifier);
            if(search_token != scope->cend()){
                homonymous_token = search_token->second;
                id_retrieved = true;
                break;
            }
        } while(!is_inside_function(++scope));


        if( !id_retrieved ){
            const auto& found = search_scopes_starting_from(scope, identifier, &homonymous_token);

            if(found == end) {
                Symbol_Types type_variable = (scope_num == GLOBAL_SCOPE) ? Symbol_Types::global_variable : Symbol_Types::variable;
                insert_token(identifier, token, type_variable);
            }            
            else if(Symbol_Types::function != homonymous_token->getSymbolType() && Symbol_Types::global_variable != homonymous_token->getSymbolType())
                    cerr << "Error: Cannot access variable \"" << identifier << "\" in scope " << scope->get_scope() << "." << endl;
            
        }

        return (homonymous_token==nullptr) ? token : homonymous_token;                
    }
        


        auto lvalue_search_variable(const Token* token) -> const Token* {

            if(token == nullptr || token->getSymbolType() == Symbol_Types::global_variable)
                return token;

            const Token* homonymous_token = search_variable(token);
            
            if(homonymous_token->getSymbolType() == Symbol_Types::function)
                cerr << "Error: Function \"" << token->getLexem<string>() << "\" is being illegally used as l-value." << endl;
            

            return (homonymous_token==nullptr) ? token : homonymous_token;
        }



        auto lvalue_declare_local_identifier(const Token* token) -> const Token* {

            const auto& not_found = scope_variables[scope_num].cend();
            const Token* returning_token = nullptr;

            const string& identifier = token->getLexem<string>();
            const auto& search_id = scope_variables[scope_num].find(identifier);

            if(search_id == not_found){
                if(doesnot_shadow_library_function(identifier)) {
                    Symbol_Types type_variable = (scope_num == GLOBAL_SCOPE) ? Symbol_Types::global_variable : Symbol_Types::variable;
                    insert_token(identifier, token, type_variable);
                    returning_token = const_cast<Token*>(token);
                }
            }
            else { 

                if( search_id->second->getSymbolType() == Symbol_Types::function )
                    cerr << "Error: Variable \"" << token->getLexem<string>() << "\" points to a function. It cannot be used as an l-value." << endl;
                
                else
                    returning_token = search_id->second;
            }

            return returning_token;
        }



        auto search_formal_identifier(Token* token){
            const std::string& identifier = token->getLexem<string>();
            
            if ( doesnot_shadow_library_function(identifier) && formal_id_not_taken(identifier) )
                insert_token(identifier, token, Symbol_Types::formal_variable);

        }


        auto define_function(const Token* token) -> void {
            const std::string& identifier = token->getLexem<string>();
            std::vector<Scope>::const_reverse_iterator scope;

            if(doesnot_shadow_library_function(identifier) && doesnot_redeclare_identifier(token))
                insert_token(identifier, token, Symbol_Types::function, false);         

        }


        auto print() const -> void ;

        int get_scope_num(){return scope_num;}

        // allages
        inline auto search_current_scope(const Token* token, const string& identifier) const -> Token* {
            Token* returning_token = nullptr;
            auto current_scope = scope_variables[scope_num];

            const auto& id = current_scope.find(identifier);

            if(id == current_scope.cend()){
                tokenList.emplace_back(Token(identifier, 299, token->getLine(), token->getTokensNumber() + 1));
                returning_token = &tokenList.back();
                current_scope.insert(returning_token, Symbol_Types::variable);
            }
            else returning_token = id->second;

            return returning_token;
        }

    };



#endif