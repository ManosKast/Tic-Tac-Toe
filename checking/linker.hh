#ifndef CUSTOM_PARSER_HH
#define CUSTOM_PARSER_HH

#include <memory>
#include <fstream>
#include <memory>
#include "symbol_table.hh"
#include "al.hh"
#include "parser.tab.hh"



namespace yy {

    class Linker{
    private:
       
        unique_ptr<yy::parser> parser;
        unique_ptr<yy::Lexer> scanner;

    public:
        Linker() = default;
        ~Linker() = default;

        auto link(const string& file_name) -> void {
            std::ifstream text_file(file_name);
            if (!text_file.good()) { std::cerr << "File: " << file_name << " does not exist. Aborting." << std::endl; exit(1); }
            
            scanner = std::make_unique<yy::Lexer>(&text_file);
            parser = std::make_unique<yy::parser>(*scanner, *this);

            parser->parse();
        }   

    };
}

#endif