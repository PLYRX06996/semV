#include<bits/stdc++.h>
#include<FlexLexer.h>
#include<fstream>
using namespace std;


int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <filename>" << std::endl;
        return 1;
    }

    std::ifstream inputFile(argv[1]);
    if (!inputFile.is_open()) {
        std::cerr << "Failed to open: " << argv[1] << std::endl;
        return 1;
    }

    yyFlexLexer* lexer = new yyFlexLexer(&inputFile);
    
    lexer->yylex();

    delete lexer;
    return 0;
}