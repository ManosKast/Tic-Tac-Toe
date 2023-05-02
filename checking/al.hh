#ifndef __MCSCANNER_HPP__
#define __MCSCANNER_HPP__ 1

#if ! defined(yyFlexLexerOnce)
#include <FlexLexer.h>
#endif

#include <memory>
#include "location.hh"
#include "parser.tab.hh"

namespace yy{

   class Lexer : public yyFlexLexer{
   private:
      // parser variables. They are required to cooperate with the Flexer. Making them unique_ptr will break the code.
      yy::parser::semantic_type* yylval = nullptr;
      yy::parser::location_type* loc    = nullptr;

   public:
      Lexer(std::istream *in) : yyFlexLexer(in) {
         loc = new yy::parser::location_type();
      };
   
      // yylex is virtual.
      using FlexLexer::yylex;

      // yylex definition. The initial definition must be undefined.
      virtual int yylex( yy::parser::semantic_type * const lval, yy::parser::location_type *location );

   };

}

#endif 