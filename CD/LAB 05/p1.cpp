#include<bits/stdc++.h>
using namespace std;

string input;
int i_str = 0;
vector<string> D;

bool fun2(){
    int backtrack_ptr = i_str; 
    
    if(i_str < input.length() && input[i_str] == 'a'){
        i_str++;
        if(i_str < input.length() && input[i_str] == 'b'){
            i_str++;
            D.push_back("A -> ab"); 
            return true;
        }
    }
    
    i_str = backtrack_ptr; 
    
    if(i_str < input.length() && input[i_str] == 'a'){
        i_str++;
        D.push_back("A -> a"); 
        return true;
    }
    
    return false;
}

bool fun1(){
    if(i_str < input.length() && input[i_str] == 'c'){
        i_str++;
        
        if(fun2()){
            if(i_str < input.length() && input[i_str] == 'd'){
                i_str++;
               D.insert(D.begin(), "S -> cAd"); 
                return true;
            }
        }
    }
    return false;
}

int main(){
    cin >> input;
    
    i_str = 0;
    
    
    if(fun1() && i_str == input.length()){
        cout << "Derivation:\n";
        for(string d : D){
            cout << d << "\n";
        }
        cout << "Result: ACCEPTED\n"; 
    } else {
        cout << "Result: REJECTED\n"; 
    }
    
    return 0;
}