#include "symbol_table.hh"


auto Symbol_Table::print() const -> void {

   string types[5] = {{"library function"}, {"global variable"}, {"user function"}, {"local variable"}, {"formal argument"}};
   
   string& symbol_type = types[0];
   int current_scope = -1;


   auto print_scopes = [this, &current_scope, &symbol_type, &types](const auto& scope){
      if(scope.first != current_scope)
         cout << "---------------------\tScope #"<< scope.first <<"\t---------------------"<< endl;

      const string& identifier = scope.second.template getLexem<string>();

      if(library_functions.contains(identifier))
         symbol_type = types[0];
      

      else{

         switch(scope.second.getSymbolType()){
               case Symbol_Types::function: symbol_type = types[2]; break;
               case Symbol_Types::variable: symbol_type = types[3]; break;
               case Symbol_Types::formal_variable: symbol_type = types[4]; break;
               case Symbol_Types::global_variable: symbol_type = types[1]; break;
               //default: cerr << scope.second->getSymbolType() << " : System  failure. Illegal symbol type. Aborting!" << endl; exit(1);
         }

      }

      cout << "\"" << identifier << "\"  [" << symbol_type << "]  (line " << scope.second.getLine() << ")  (scope " << scope.first << ")" << endl;
      
      current_scope = scope.first;
   };

   std::for_each(printing.cbegin(), printing.cend(), print_scopes);
   
}


std::array<std::pair<std::string, Token>, 11> library_functions_pair {{
   {"print", Token("print", 260, 0, 1)},
   {"input", Token("input", 260, 0, 2)},
   {"objectmemberkeys", Token("objectmemberkeys", 260, 0, 3)},
   {"objecttotalmembers", Token("objecttotalmembers", 260, 0, 4)},
   {"totalarguments", Token("totalarguments", 260, 0, 5)},
   {"argument", Token("argument", 260, 0, 6)},
   {"typeof", Token("typeof", 260, 0, 7)},
   {"strtonum", Token("strtonum", 260, 0, 8)},
   {"sqrt", Token("sqrt", 260, 0, 9)},
   {"cos", Token("cos", 260, 0, 10)},
   {"sin", Token("sin", 260, 0, 11)}
}};



// Initialises the library's functions into a different hash table, since they can't be shadowed.
auto Symbol_Table::initialise_library_functions() -> void {
   

   scope_variables.emplace_back(0);
   // std::views h const auto& [] sto for_each.
   for(const auto& library_function : library_functions_pair){
      library_functions.insert({library_function.first});
      insert_token(library_function.first, const_cast<Token*>(&library_function.second), Symbol_Types::function);
   }

}

auto Symbol_Table::insert_token(const string& id, const Token* token, const int symbol_type, const bool access) -> void {
   
   scope_variables[scope_num].insert(token, symbol_type, access);
   printing.insert({scope_num, *token});

}


auto Symbol_Table::insert_token(const string& id, const Token* token, const int symbol_type) -> void {
   
   scope_variables[scope_num].insert(token, symbol_type);
   printing.insert({scope_num, *token});

}