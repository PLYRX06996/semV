#include<bits/stdc++.h>
#include<FlexLexer.h>
#include<fstream>
using namespace std;

extern int word_count;
extern int whitespace_sequence_replaced;
extern bool space_pending;

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
    cout << endl;
    cout << "Total word count: " << word_count << endl;
    cout << "Whitespace sequence replaced: " << whitespace_sequence_replaced << endl;
    delete lexer;
    return 0;
}