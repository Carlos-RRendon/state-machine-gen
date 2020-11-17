#include <iostream>
#include <regex>
#include <string>
#include <fstream>

using namespace std;
 

int main()
{
    ifstream inFile;
    string Present;
    inFile.open("FSM.csv");
    if(inFile.is_open()){
        cout << "File has been opened" << endl; 
    }else{
        cout << "File couldn't been opened" << endl;
    }
    while(!inFile.eof()){
        getline( inFile, Present, ',');
        cout << "Read: " << Present << endl;
    }
    inFile.close();
    return 0;
}