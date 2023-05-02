#ifndef SCOPE_HH
#define SCOPE_HH

#include<unordered_map>
#include "Token.hh"


class Scope{
private:

    std::unordered_map<std::string, Token*> scope_variables; // Stores this scope's variables.
    Token* last_inserted_token;                              // Stores the most recently insterted token.
    const int scope;                                         // Numbers this scope.
    bool can_access_homonymous_variables;                    // Specifies if homonymous variables outside this scope can be accessed.

public:

    explicit Scope(const int scope) : last_inserted_token(nullptr), scope(scope), can_access_homonymous_variables(true) {};

    inline auto insert(const Token* token, const int symbol_type, const bool access) -> void {
        last_inserted_token = const_cast<Token*>(token);
        last_inserted_token->setSymbolType(symbol_type);

        can_access_homonymous_variables = access;
        scope_variables.insert({token->getLexem(), last_inserted_token});
    }

    inline auto insert(const Token* token, const int symbol_type) -> void {
        last_inserted_token = const_cast<Token*>(token);
        last_inserted_token->setSymbolType(symbol_type);

        scope_variables.insert({token->getLexem(), last_inserted_token});
    }

    // SEARCHING
    inline auto contains(const std::string& identifier) const -> const bool { return scope_variables.contains(identifier); }
    inline auto find(const std::string& identifier) const -> const std::unordered_map<std::string, Token*>::const_iterator { return scope_variables.find(identifier); }
    
    // GETTERS
    inline auto get_last_inserted_element() const -> const Token* { return last_inserted_token; }
    inline auto get_scope() const noexcept ->  const int { return scope; }

    // ITERATORS
    inline auto cend() const -> const std::unordered_map<std::string, Token*>::const_iterator { return scope_variables.cend(); }
    inline auto cbegin() const -> const std::unordered_map<std::string, Token*>::const_iterator { return scope_variables.cbegin(); }

    // ERASE TOKEN
    inline auto erase (const string& identifier) -> void { scope_variables.erase(identifier); }

    inline auto can_access_homonymous_variable() const noexcept -> const bool {return can_access_homonymous_variables;}

    inline auto prints(){   
        auto print = [this](const auto& tok){cout<< tok.second->template getLexem<string>() <<endl;};
        for_each(scope_variables.cbegin(), scope_variables.cend(), print);
        cout << endl;
    }


};

#endif