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
map <string,int> rep_states(map <int,map <string,map <string,string> > > states){
    map <string,int> countMap;              //Find duplicates
    for(int i=0;i<states.size();i++){
        auto result = countMap.insert(pair<string,int>(states[i]["actual"]["1"],1));
        if(result.second == false)
            result.first->second++;
    }
    return countMap;
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
                    if((in.second != "x")&&(in.second !="X")){
                        long long binsignal = BinToDec(stoi(in.second));
                        tempo.push_back(binsignal);
                    }
                }
            }
        }
        int max = *std::max_element(tempo.begin(),tempo.end());
        int bits = ceil(log2(max));
        if(bits < 0){
            maximos.insert(pair<string,int>(key[j],0));
        }else if(bits == 1){
            maximos.insert(pair<string,int>(key[j],2));
        }else{
            maximos.insert(pair<string,int>(key[j],bits));
        }
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
    inFile.open("FSM_MIPS.csv");
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
                for(int i=1;i<=(pc_in+1);i++){
                    fsm_io[ren-1]["inputs"][m[((i+i)-1)]] = m[(i+i)];
                }
            }else{
                regex comma(com);
                regex_search(Inputs, m, comma);
                fsm_io[ren-1]["inputs"][m[1]] = m[2];
            }
            if (pc_ou > 0){ //For Outputs
                com1 = com;
                for(int i=0;i<pc_ou;i++){
                    com1 += add_com;
                }
                regex comma(com1);
                regex_search(Outputs, m, comma);
                for(int i=1;i<=(pc_ou+1);i++){
                    fsm_io[ren-1]["outputs"][m[((i+i)-1)]] = m[(i+i)];
                }
            }else{
                regex comma(com);
                regex_search(Outputs, m, comma);
                fsm_io[ren-1]["outputs"][m[1]] = m[2];
            }

        }
        ren += 1;
    }

    
    inFile.close();
      
    string text_out;
    
    string name = "FSM_C.sv";
    text_out = "//=============================================================================\n";
    text_out += "// FSM Verilog design\n";
    text_out += "//=============================================================================\n\n\n";
    text_out += "//-----------------------------------------------------------------------------\n";
    text_out += "// Module name and ports declaration\n"; 
    text_out += "//-----------------------------------------------------------------------------\n\n" ;  
    text_out += "module state_machine (\n";
    text_out += "input clk, rst,\n";

    for (auto const& out:port_width(fsm_io,"inputs")){         //Print actual states along its number of repetitions
        if (out.second == 0){
            text_out += "input " + out.first + ",\n";
        }else{
            text_out += "input " + out.first + " [";
            string buffer = to_string(out.second-1);
            text_out += buffer + ":0],\n";
        }
    }

    map <string,int> output = port_width(fsm_io,"outputs");
    int k=0;
    for (auto const& out:output){         //Print actual states along its number of repetitions
        if (out.second == 0){
            text_out += "output reg " + out.first;
        }else{
            text_out += "output reg " + out.first + " [";
            if (out.second >= 2){
                string buffer = to_string(out.second-1);
                text_out += buffer + ":0]";
            }
            else {
                string buffer = to_string(out.second);
                text_out += buffer + ":0]";
            }
            
        }
        k+=1;
        if(k!=output.size()){
            text_out += ",\n";
        }
    }

    text_out += ");\n\n";
    text_out += "//-----------------------------------------------------------------------------\n";
    text_out += "// FSM states declaration\n";
    text_out += "//-----------------------------------------------------------------------------\n"; 
    int buff;
    map <string,string> states = ini_states(fsm_io);
    for (auto const& out:states){         //Print actual states along its number of repetitions
        buff = out.second.size();
        string buf = to_string(out.second.size());
        text_out += "parameter " + out.first + " = " + buf + "'b" + out.second +";\n";
    }

    if (buff >= 2){
        string buffer = to_string(buff-1);
        text_out += "reg ["+buffer+ ":0] state, next_state; \n";
    }else{
        text_out += "reg state, next_state; \n";
    }

    for (auto const& out:states){         //Print actual states along its number of repetitions
        text_out += " \n //FSM Initialization state";     
        text_out += "\ninitial begin\n  state=0;\nend\n";
        text_out += " \n //FSM State transitions (clock dependant)";   
        text_out += "\nalways @ (posedge clk or rst)\n  begin\n  if (rst) state <= ";
        text_out += out.first+";\n";
        text_out += "  else state <= next_state;\n  end\n\n";
        break;
    }

    text_out += "//-----------------------------------------------------------------------------\n";
    text_out += "// FSM States assignment\n"; 
    text_out += "//-----------------------------------------------------------------------------\n"; 
    text_out += "always @ (state, ";
    k=0;
    map <string,int> ini = port_width(fsm_io,"inputs");
    for (auto const& out:ini){         //Print actual states along its number of repetitions
        text_out += out.first ;
        k+=1;
        if(k!=ini.size()){
            text_out += + ", ";
        }
    }

    text_out += ")\n";
    text_out += "begin\n case(state)\n";

    bool flag = false;
    bool flag_states = false;
    int longitud = 0;

    for(auto const& sta:rep_states(fsm_io)){
        if(sta.second == 1){
            for(int i=0;i<fsm_io.size();i++){
                if(sta.first == fsm_io[i]["actual"]["1"]){
                    text_out += sta.first+": \n";
                    for(auto const& out:fsm_io[i]["inputs"]){
                        if((out.second != "x")&&(out.second != "X")){
                            longitud += 1;
                            if (flag == false){
                                text_out += "if ((" + out.first +" == "+out.second;
                                flag = true;
                            }else{
                                text_out += ") & ( " + out.first +" == "+out.second;
                            }
                        }
                    }
                    flag = false;
                    if (longitud != 0){
                            text_out += "))\n";
                            longitud = 0;
                    }
                    text_out += " next_state <= "+fsm_io[i]["next"]["1"]+";\n";
                }            
            }
        }else{
            for(int i=0;i<fsm_io.size();i++){
                if(sta.first == fsm_io[i]["actual"]["1"]){
                    if (flag_states == false){
                            text_out +=  sta.first+" :\n";
                            flag_states = true;
                            text_out += "  begin\n";
                    }
                    for(auto const& out:fsm_io[i]["inputs"]){
                        if((out.second != "x")&&(out.second != "X")){
                            if (flag == false){
                                text_out += "if ((" + out.first +" == "+out.second;
                                flag = true;
                            }else{
                                text_out += ") & ( " + out.first +" == "+out.second;
                            }
                        }
                    }
                    flag = false;
                    text_out += "))\n";
                    text_out += "     next_state <= "+fsm_io[i]["next"]["1"]+";\n";
                }
            }
            text_out += "   end\n";
            flag_states = false;
        }
    }

    text_out += "  endcase\nend\n\n";
    text_out += "//-----------------------------------------------------------------------------\n";
    text_out += "// FSM Outputs assignment\n"; 
    text_out += "//-----------------------------------------------------------------------------\n";
    text_out += "always @ (state)\n";
    text_out += "  begin\n  case(state)\n";
    bool flag_out = true;

    for(auto const& sta:rep_states(fsm_io)){
        for(int i=0;i<fsm_io.size();i++){
            if((sta.first == fsm_io[i]["actual"]["1"])&&(flag_out)){
                text_out += "    "+sta.first+": \n";
                flag_out = false;
                text_out += "    begin\n";
                for(auto const& out:fsm_io[i]["outputs"]){
                    text_out += "        "+out.first+" = "+ out.second+";\n";
                }
            }
        }
        flag_out = true;
        text_out += "    end\n";
    }

    text_out += "  endcase\n";
    text_out += " end\n\nendmodule";
    text_out += "\n\n//=============================================================================\n";
    text_out += "//=============================================================================\n\n";

    fstream  outfile;
    outfile.open(name, ofstream::out); // name of testbench as the design name
    outfile << text_out;
    outfile.close();
    cout << "Your file " + name;
    cout  << " was succesful created" << endl;

    return 0;
}
