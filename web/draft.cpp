#include <iostream>
#include <sstream>
using namespace std;


int main(){
    string s = "add x2, x1, x0";
    stringstream ss(s);
    string instructionWord;
    ss>>instructionWord;
    string instructionInfo;
    getline(ss, instructionInfo);

    cout << instructionWord << endl;
    cout << instructionInfo << endl;
}