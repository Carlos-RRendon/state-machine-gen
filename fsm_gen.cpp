#include <iostream>
#include <regex>
#include <string>
#include <fstream>
#include <math.h>

using namespace std;

int BinToDec(long long n)                   //Function to convert a binary number into a decimal number
{
    int decimalNumber = 0, i = 0, remainder;//Initial variables
    while (n!=0)                            //While 
    {
        remainder = n%10;                   //Save the remainder of n%10
        n /= 10;                            //Update n with n=n/10
        decimalNumber += remainder*pow(2,i);//Update the decimal number
        ++i;                                //Increase the counter
    }
    return decimalNumber;                   //Return the final decimal number
}

string decToBinary(int n, int dim)          //Function to convert a decimal number into a binary number
{ 
    int binaryNum[dim];                     //Array to store the binary number
    string binario;                         //Strin to save the binary number
    int i = 0;                              //Counter for the binary array
    while (n > 0) {                         //While n bigger than zero
        binaryNum[i] = n % 2;               //Store the remainder in the binary array 
        n /= 2;                             //Update n with n = n/2
        i++;                                //Increase the counter
    } 
    for(int k = 0; k< dim-i;k++)            //For to add to extend the binary number until the required dimension
        binario += "0";                     //Fill with zero
    for (int j = i - 1; j >= 0; j--)        //For loop to change the order of the binary number
        binario += to_string(binaryNum[j]); //Change the order of thee binary number
    return binario;                         //Return the final binary number
} 

//Function to intialize states
map <string,int> rep_states(map <int,map <string,map <string,string> > > states){
    map <string,int> countMap;              //Map to save the key and the number of repetitions
    for(int i=0;i<states.size();i++){       //For loop to read all the given states
        //Insert in all the found keys 1
        auto result = countMap.insert(pair<string,int>(states[i]["actual"]["1"],1));
        if(result.second == false)          //If a 1 cannot be given
            result.first->second++;         //Increase the counter of the key
    }
    return countMap;                        //Return the map with the number of repetitions
}

//Function to intialize states
map <string,string> ini_states(map <int,map <string,map <string,string> > > states){
    map <string,int> countMap;              //Map to save the key and the number of repetitions
    for(int i=0;i<states.size();i++){       //For loop to read all the given states
        //Insert in all the found keys 1
        auto result = countMap.insert(pair<string,int>(states[i]["actual"]["1"],1));
        if(result.second == false)          //If a 1 cannot be given
            result.first->second++;         //Increase the counter of the key
    }
    //Bit dimension and initializate states
    int bits = ceil(log2(countMap.size())); //Maximum dimension of the states
    int j=0;                                //Initialize counter
    map <string,string> iniStates;          //Map to save the state name and its binary representation
    for (auto const& out:countMap){         //Print actual states along its number of repetitions
        //Insert into the iniStates map the name and its binary number
        iniStates.insert(pair<string,string> (out.first, decToBinary(j,bits)));
        j+=1;                               //Increase counter
    }
    return iniStates;                       //Return the created map
}
//Function to return the 
map <string,int> port_width(map <int,map <string,map <string,string> > > states,string type){
    map <string,int> maximos;
    vector <string> key;
    for(int i=0;i<states.size();i++){       //For loop to read all the entries in the map
        for(auto const& in:states[i][type]){//For loop to read all the inputs/outputs
            key.push_back(in.first);        //Extract the keys from inputs/outputs
        }
    }
    auto end = key.end();                   //Create a variable end to save the last index
    for(auto it=key.begin();it != end; ++it){//For loop to iterate over all the extracted keys
            end = remove(it + 1, end, *it); //Remove a key if it was found before
    }
    key.erase(end,key.end());               //Erase the duplicated keys
    vector <long long> tempo;               //Create a variable tempo
    for(int j=0; j<key.size();j++){         //For loop of the keys without repetition
        for(int i=0;i<states.size();i++){   //For loop to read all the entries in the map
            for(auto const& in:states[i][type]){//For loop to find in the inputs/outpus map
                if(in.first == key[j]){     //If the input name is equal to the key
                    if((in.second != "x")&&(in.second !="X")){//If the signal value is different than x|X
                        //Transform the binary number into decimal
                        long long binsignal = BinToDec(stoi(in.second));
                        tempo.push_back(binsignal);//Save the conversion in the temporal vector
                    }
                }
            }
        }
        int max = *std::max_element(tempo.begin(),tempo.end()); //Find the maximum value of the temporal vector
        if (max == 0){
            max = 1;
        }
        int bits = floor(log(max)/log(2))+1;
        if(bits == 1){                                           //If the bits are less than 0
            maximos.insert(pair<string,int>(key[j],0));         //The signals do not have buffer
        }else {                                                //In any other case
            maximos.insert(pair<string,int>(key[j],bits));      //The bits variable has the required buffer
        } 
        tempo.clear();                                          //Clean the temporal vector
    }
    return maximos;                                             //Return the map with the input/output buffers
}
 
int main()                                                      //Main function
{
    ifstream inFile;                                            //Create an ifstream variable to read the .csv file                                     
    string Present;                                             //String to save the actual state
    string Next;                                                //String to save the next state 
    string Inputs;                                              //String to save the inputs
    string Outputs;                                             //String to save the outputs
    inFile.open("FSM_MIPS.csv");                                //Open the file called FSM_MIPS.csv
    if(inFile.is_open()){                                       //If the file was opened
        cout << "File has been opened" << endl;                 //Print that the file has been opened
    }else{                                                      //If the files was not opened
        cout << "File couldn't been opened" << endl;            //Print that the file couldn't been opened
    }           
    int ren = 0;                                                //
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
            text_out += "input [";
            string buffer = to_string(out.second-1);
            text_out += buffer + ":0] "+ out.first+",\n";
        }
    }

    map <string,int> output = port_width(fsm_io,"outputs");
    int k=0;
    for (auto const& out:output){         //Print actual states along its number of repetitions
        if (out.second == 0){
            text_out += "output reg " + out.first;
        }else{
            text_out += "output reg [";
            if (out.second >= 2){
                string buffer = to_string(out.second-1);
                text_out += buffer + ":0] "+ out.first;
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
cout << "Que onda" << endl;
    for(auto const& sta:rep_states(fsm_io)){
        if(sta.second == 1){
            for(int i=0;i<fsm_io.size();i++){
                if(sta.first == fsm_io[i]["actual"]["1"]){
                    text_out += sta.first+": \n";
                    for(auto const& out:fsm_io[i]["inputs"]){
                        if((out.second != "x")&&(out.second != "X")){
                            longitud += 1;
                            if (flag == false){
                                text_out += "if ((" + out.first +" == ";
                                text_out += out.second;
                                //cout << out.second.size() << endl;
                                flag = true;
                            }else{
                                text_out += ") & ( " + out.first +" == ";
                                text_out += out.second;
                                //cout << out.second.size() << endl;
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
                            string n_bit=to_string(out.second.size());
                            if (flag == false){
                                text_out += "if ((" + out.first +" == ";
                                text_out += n_bit + "'b" +out.second;
                                flag = true;
                            }else{
                                text_out += ") & ( " + out.first +" == ";
                                text_out += n_bit+"'b" +out.second;
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
                    int int_bit = out.second.size();
                    string n_bit=to_string(int_bit);
                    if (int_bit == 1){
                        text_out += "        "+out.first+" = "+ out.second+";\n";
                    }else{
                        text_out += "        "+out.first+" = " + n_bit+"'b" + out.second+";\n";
                    }
                    
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
