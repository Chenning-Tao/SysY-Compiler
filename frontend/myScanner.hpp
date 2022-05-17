#ifndef COMPILER_MYSCANNER_HPP
#define COMPILER_MYSCANNER_HPP
#if ! defined(yyFlexLexerOnce)
#include <FlexLexer.h>
#endif
class myScanner : public yyFlexLexer {
public:
    myScanner(std::istream *in) : yyFlexLexer(in) {};
    virtual ~myScanner() = default;

    virtual int yylex();
};

#endif // COMPILER_MYSCANNER_HPP