#include <iostream>
#include <regex>
#include <string>
#include <fstream>
#include <math.h>

using namespace std;

int BinToDec(long long n)
{
    int decimalNumber = 0, i = 0, remainder;
    while (n!=0)
    {
        remainder = n%10;
        n /= 10;
        decimalNumber += remainder*pow(2,i);
        ++i;
    }
    return decimalNumber;
}

string decToBinary(int n, int dim) 
{ 
    // array to store binary number 
    int binaryNum[dim]; 
    string binario;
    // counter for binary array 
    int i = 0;    
    while (n > 0) { 
  
        // storing remainder in binary array 
        binaryNum[i] = n % 2; 
        
        n = n / 2; 
        i++; 
    } 
    for(int k = 0; k< dim-i;k++)
        binario += "0";
    for (int j = i - 1; j >= 0; j--) 
        binario += to_string(binaryNum[j]);
    return binario;
   
} 
//Function to intialize states
map <string,string> ini_states(map <int,map <string,map <string,string> > > states){
    map <string,int> countMap;              //Find duplicates
    for(int i=0;i<states.size();i++){
        auto result = countMap.insert(pair<string,int>(states[i]["actual"]["1"],1));
        if(result.second == false)
            result.first->second++;
    }
    //Bit dimension and initializate states
    int bits = ceil(log2(countMap.size()));
    //cout << bits << endl;
    int j=0;
    map <string,string> iniStates;
    for (auto const& out:countMap){         //Print actual states along its number of repetitions
        iniStates.insert(pair<string,string> (out.first, decToBinary(j,bits) ) );
        j+=1;
    }

    return iniStates;
}

map <string,int> port_width(map <int,map <string,map <string,string> > > states,string type){
    map <string,int> maximos;
    vector <string> key;
    for(int i=0;i<states.size();i++){
        for(auto const& in:states[i][type]){
            key.push_back(in.first);
        }
    }
    cout <<endl;            //Erase duplicates
    cout << type<< endl;
    auto end = key.end();
    for(auto it=key.begin();it != end; ++it){
            end = remove(it + 1, end, *it);
    }
    key.erase(end,key.end());
    vector <long long> tempo;
    for(int j=0; j<key.size();j++){         //Print the inputs in order
        for(int i=0;i<states.size();i++){
            for(auto const& in:states[i][type]){
                if(in.first == key[j]){
                    if((in.first != "x")&&(in.first !="X")){
                        long long binsignal = BinToDec(stoi(in.second));
                        tempo.push_back(binsignal);
                    }
                }
            }
        }
        int max = *std::max_element(tempo.begin(),tempo.end());
        maximos.insert(pair<string,int>(key[j],max));
        tempo.clear();
    }
    return maximos;
}
 
int main()
{
    ifstream inFile;
    string Present;
    string Next;
    string Inputs;
    string Outputs;
    inFile.open("FSM.csv");
    if(inFile.is_open()){
        cout << "File has been opened" << endl; 
    }else{
        cout << "File couldn't been opened" << endl;
    }
    int ren = 0;
    map <int,map <string,string> > fsm;
    map <int,map <string,map <string,string> > > fsm_io;
    string com = "\\s*(\\w+)\\s*\\=\\s*(\\w+)\\s*";
    string com1;
    string add_com = "\\;+\\s*(\\w+)\\s*\\=\\s*(\\w+)\\s*";
    smatch m; 
    while(!inFile.eof()){
        getline( inFile, Present, ',');
        getline( inFile, Next, ',');
        getline( inFile, Inputs, ',');
        getline( inFile, Outputs, '\n');
        if ((ren > 0) & (Present != "")){
            fsm[ren-1]["actual"] = Present;
            fsm[ren-1]["next"] = Next;
            fsm[ren-1]["inputs"] = Inputs;
            fsm[ren-1]["outputs"] = Outputs;
            fsm_io[ren-1]["actual"]["1"] = Present;
            fsm_io[ren-1]["next"]["1"] = Next;
            size_t pc_in = count(Inputs.begin(),Inputs.end(),';');
            size_t pc_ou = count(Outputs.begin(),Outputs.end(),';');
            if (pc_in > 0){ //For Inputs
                com1 = com;
                for(int i=0;i<pc_in;i++){
                    com1 += add_com;
                }
                regex comma(com1);
                regex_search(Inputs, m, comma);
                //cout << "Aqui" << endl;
                for(int i=1;i<=(pc_in+1);i++){
                    fsm_io[ren-1]["inputs"][m[((i+i)-1)]] = m[(i+i)];
                }
            }else{
                regex comma(com);
                regex_search(Inputs, m, comma);
                fsm_io[ren-1]["inputs"][m[1]] = m[2];
            }
            //cout << Outputs << endl;
            if (pc_ou > 0){ //For Outputs
                com1 = com;
                for(int i=0;i<pc_ou;i++){
                    com1 += add_com;
                }
                regex comma(com1);
                regex_search(Outputs, m, comma);
                //cout << "Aqui" << endl;
                for(int i=1;i<=(pc_ou+1);i++){
                    fsm_io[ren-1]["outputs"][m[((i+i)-1)]] = m[(i+i)];
                    cout << "Check: " << m[((i+i)-1)] << m[(i+i)] << endl;
                }
            }else{
                regex comma(com);
                regex_search(Outputs, m, comma);
                fsm_io[ren-1]["outputs"][m[1]] = m[2];
            }

        }
        ren += 1;
    }

    for(int i=0;i<fsm.size();i++){
        cout << fsm[i]["actual"] << endl;
        cout << fsm[i]["next"] << endl;
        cout << fsm[i]["inputs"] << endl;
        cout << fsm[i]["outputs"] << endl << "\n";
    }

    /*for(int i=0;i<fsm_io.size();i++){
        cout << "actual => " << fsm_io[i]["actual"]["1"] << endl;
        cout << "next => " << fsm_io[i]["next"]["1"] << endl;
        for(auto const& in:fsm_io[i]["inputs"]){
            cout << in.first << " => " << in.second << endl;
        }
        for(auto const& out:fsm_io[i]["outputs"]){
            cout << out.first << " => " << out.second << endl;
        }
        cout << endl;
    }*/
    inFile.close();
    //Initialize states
    for (auto const& out:ini_states(fsm_io)){         //Print actual states along its number of repetitions
        cout << out.first << " => " << out.second << endl;
    }

    cout << endl;
    
    for (auto const& out:port_width(fsm_io,"inputs")){         //Print actual states along its number of repetitions
        cout << out.first << " => " << out.second << endl;
    }
    for (auto const& out:port_width(fsm_io,"outputs")){         //Print actual states along its number of repetitions
        cout << out.first << " => " << out.second << endl;
    }     

    /*map <string,int> countMap;              //Find duplicates
    for(int i=0;i<fsm_io.size();i++){
        map <string,int> result;
        for(auto const& in:fsm_io[i]["inputs"]){
        result = countMap.insert(pair<string,int>(in.first,1));
        }
        if(result.second == false)
            result.first->second++;
    }*/

    /*cout << endl;

    for (auto const& out:maximos){         //Print actual states along its number of repetitions
        cout << out.first << " => " << out.second << endl;
    }*/

    return 0;
}
