#include<bits/stdc++.h>
using namespace std;

bool fun3(){

}

int i_str = 0;
string fun2(string s, string i){
    if(i[i_str] == s[0]){
        i_str++;
        if(i[i_str] == 'd'){
            fun3();
        // } else fun2(); // recursive checking using A_ also
    } else return "Result: REJECTED";
}

string fun1(string s, string i){
    if(s[0] == i[0]){
        i_str++;
    return i;
    }
    else
    return "Result: REJECTED";
}

int main(){

    string S = "cAd";
    string A = "ab";
    string A_ = "a";
    string input;
    cin >> input;

    fun2(A, fun1(S, input));
    if(fun3()){
        cout << "Result: ACCEPTED";
    } else cout << "Result: REJECTED";
    return 0;
}