%{
#include <bits/stdc++.h>
using namespace std;

string translatePigLatin(string word, int consCount) {
    if (consCount == 0) {
        return word + "way";
    }
    
    bool isFirstUpper = isupper(word[0]);
    string moved = word.substr(0, consCount);
    string rest = word.substr(consCount);
    
    return rest + moved + "ay";
}
%}

%option c++
%option noyywrap

VOWEL [aeiouAEIOU]
CONS  [b-df-hj-np-tv-zB-DF-HJ-NP-TV-Z]

%%

{VOWEL}[a-zA-Z]* {
    cout << translatePigLatin(yytext, 0);
}

{CONS}{2}[a-zA-Z]* {
    cout << translatePigLatin(yytext, 2);
}

{CONS}[a-zA-Z]* {
    cout << translatePigLatin(yytext, 1);
}

\n {
    cout << "\n";
}

. {
    cout << yytext;
}

%%