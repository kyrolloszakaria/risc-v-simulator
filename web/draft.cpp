#include <iostream>
#include <sstream>
#include <map>
#include <bitset>
using namespace std;


int main(){
    map<unsigned int, int> memory;
    memory.insert(make_pair(4,12));
    memory.insert(make_pair(8,0));
    string regFormat = "b";
    if (regFormat == "b") {
    // Print memory contents
    if (!memory.empty()) {
        for (const auto& it : memory) {
            cout << it.first << " : " << bitset<32>(it.second) << "\n";
        }
    } else {
        cout << "Memory : free\n";
    }

}


}