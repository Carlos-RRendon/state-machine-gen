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
    string binario;                         //String to save the binary number
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
        if (max == 0){                                          //If the maximum equal to zero
            max = 1;                                            //Set max as 1
        }
        int bits = floor(log(max)/log(2))+1;                    //Calculate the number of bits required
        if(bits == 1){                                          //If the bits are less than 0
            maximos.insert(pair<string,int>(key[j],0));         //The signals do not have buffer
        }else {                                                 //In any other case
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
    int ren = 0;                                                //Initialize t
    map <int,map <string,map <string,string> > > fsm_io;        //Create a map with many map to store the states and inputs/outputs
    string com = "\\s*(\\w+)\\s*\\=\\s*(\\w+)\\s*";             //Regex to identify ny groups the variable name and its value
    string com1;                                                //Create an empty string
    string add_com = "\\;+\\s*(\\w+)\\s*\\=\\s*(\\w+)\\s*";     //Regex to add everytime there is a ;
    smatch m;                                                   //Create a smatch
    while(!inFile.eof()){                                       //While the file is opened
        getline( inFile, Present, ',');                         //Save the value in the actual state
        getline( inFile, Next, ',');                            //Save the value in the next state
        getline( inFile, Inputs, ',');                          //Save the input variables
        getline( inFile, Outputs, '\n');                        //Save the output variables
        if ((ren > 0) & (Present != "")){                       //If the Present string is not empty and the row is bigger than 0
            fsm_io[ren-1]["actual"]["1"] = Present;             //Save the actual state in the map
            fsm_io[ren-1]["next"]["1"] = Next;                  //Save the next state in the map
            size_t pc_in = count(Inputs.begin(),Inputs.end(),';');//Count how many ";" are inside the input string 
            size_t pc_ou = count(Outputs.begin(),Outputs.end(),';');//Count how many ";" are inside the output string
            if (pc_in > 0){                                     //If there are at least one ";" in the Inputs string
                com1 = com;                                     //Assign the regex to com1
                for(int i=0;i<pc_in;i++){                       //For loop to add the regex addition to the original regex
                    com1 += add_com;                            //Add a regex addition each time there is a ";" 
                }
                regex comma(com1);                              //Regex to find the input names and its value
                regex_search(Inputs, m, comma);                 //Search the ";" in the Inputs string
                for(int i=1;i<=(pc_in+1);i++){                  //For loop to save each input independently
                    fsm_io[ren-1]["inputs"][m[((i+i)-1)]] = m[(i+i)];//Save the input name and its value
                }
            }else{                                              //If there are no ";"
                regex comma(com);                               //Regex to find the output names and its value
                regex_search(Inputs, m, comma);                 //Search the ";" in the Inputs string
                fsm_io[ren-1]["inputs"][m[1]] = m[2];           //Save the input name and its value
            }
            if (pc_ou > 0){                                     //If there are at least one ";" in the Outputs string
                com1 = com;                                     //Assign the regex to com1
                for(int i=0;i<pc_ou;i++){                       //For loop to add the regex addition to the original regex
                    com1 += add_com;                            //Add a regex addition each time there is a ";"
                }
                regex comma(com1);                              //Regex to find the input names and its value
                regex_search(Outputs, m, comma);                //Search the ";" in the Inputs string
                for(int i=1;i<=(pc_ou+1);i++){                  //For loop to save each input independently
                    fsm_io[ren-1]["outputs"][m[((i+i)-1)]] = m[(i+i)];//Save the input name and its value
                }
            }else{                                              //If there are no ";"
                regex comma(com);                               //Regex to find the output names and its value
                regex_search(Outputs, m, comma);                //Search the ";" in the Inputs string
                fsm_io[ren-1]["outputs"][m[1]] = m[2];          //Save the input name and its value
            }
        }
        ren += 1;                                               //Increase the ren (row) variable
    }
    inFile.close();                                             //Close the .csv file
      
    string text_out;                                            //Create a text_out string to write the csv file
    string name = "FSM_C.sv";                                   //File name
    //Print the file header
    text_out = "//=============================================================================\n";
    text_out += "// FSM Verilog design\n";
    text_out += "//=============================================================================\n\n\n";
    text_out += "//-----------------------------------------------------------------------------\n";
    text_out += "// Module name and ports declaration\n"; 
    text_out += "//-----------------------------------------------------------------------------\n\n" ;  
    //Start the module
    text_out += "module state_machine (\n";
    text_out += "input clk, rst,\n";
    for (auto const& out:port_width(fsm_io,"inputs")){          //For loop to read the inputs
        if (out.second == 0){                                   //If the bus is zero
            text_out += "input " + out.first + ",\n";           //Print single inputs
        }else{                                                  //If the bus is not zero
            text_out += "input [";                              //Start a [
            string buffer = to_string(out.second-1);            //Tranform the bus -1 into a string
            text_out += buffer + ":0] "+ out.first+",\n";       //Print the bus and its name
        }
    }
    map <string,int> output = port_width(fsm_io,"outputs");     //Save a map with the ouput buffers
    int k=0;                                                    //Initialize a counter variable
    for (auto const& out:output){                               //For loop to read the ouputs
        if (out.second == 0){                                   //If the bus is zero
            text_out += "output reg " + out.first;              //Print single inputs
        }else{                                                  //If the bus is not zero
            text_out += "output reg [";                         //Start a [
            if (out.second >= 2){                               //If the value if bigger than 2
                string buffer = to_string(out.second-1);        //Convert the buffer - 1 to a string
                text_out += buffer + ":0] "+ out.first;         //Close the buffer
            }                                   
            else {                                              //If the value is smaller
                string buffer = to_string(out.second);          //Print just its name
                text_out += buffer + ":0]";                     //Close the buffer
            }
        }
        k+=1;                                                   //Increase the counter
        if(k!=output.size()){                                   //If the item is not the last
            text_out += ",\n";                                  //Print a ","
        }
    }
    text_out += ");\n\n";                                       //Close the inputs/outputs initialization
    //Start the FSM state declaration
    text_out += "//-----------------------------------------------------------------------------\n";
    text_out += "// FSM states declaration\n";
    text_out += "//-----------------------------------------------------------------------------\n"; 
    int buff;                                                   //Integer with the buffer
    map <string,string> states = ini_states(fsm_io);            //Create a map to save the initial states
    for (auto const& out:states){                               //For loop to read the states
        buff = out.second.size();                               //Save the size of the buffer
        string buf = to_string(out.second.size());              //Conver the buffer to an integer
        text_out += "parameter " + out.first + " = " + buf + "'b" + out.second +";\n";//Print the required parameter
    }
    if (buff >= 2){                                             //If the buffer was bigger than 2
        string buffer = to_string(buff-1);                      //Decrease the buffer by 1
        text_out += "reg ["+buffer+ ":0] state, next_state; \n";//Print the buffer size
    }else{                                                      //If the buffer is not bigger than 2
        text_out += "reg state, next_state; \n";                //Print single states and next_states
    }   
    for (auto const& out:states){                           
        //Print the initial state    
        text_out += " \n //FSM Initialization state";       
        text_out += "\ninitial begin\n  state=0;\nend\n";
        text_out += " \n //FSM State transitions (clock dependant)";   
        text_out += "\nalways @ (posedge clk or rst)\n  begin\n  if (rst) state <= ";
        text_out += out.first+";\n";
        text_out += "  else state <= next_state;\n  end\n\n";
        break;
    }
    //Start the FSM States assignment
    text_out += "//-----------------------------------------------------------------------------\n";
    text_out += "// FSM States assignment\n"; 
    text_out += "//-----------------------------------------------------------------------------\n"; 
    text_out += "always @ (state, ";
    k=0;                                                                //Restart the counter k
    map <string,int> ini = port_width(fsm_io,"inputs");                 //Map to read the input buffers
    for (auto const& out:ini){                                          //For loop to read the input buffers
        text_out += out.first ;                                         //Print the input name
        k+=1;                                                           //Increase the counter
        if(k!=ini.size()){                                              //If the element is not the last one
            text_out += + ", ";                                         //Print a ","
        }
    }
    text_out += ")\n";                                                  //always statement
    text_out += "begin\n case(state)\n";                                //Begin the cases
    bool flag = false;                                                  //Flag to know if there are more than one dependant inputs
    bool flag_states = false;                                           //Flag to know if one state can go to two or more states
    int longitud = 0;                                                   //Variable to knoe the last input
    for(auto const& sta:rep_states(fsm_io)){                            //For loop to red the states without repetition
        if(sta.second == 1){                                            //If there are no repetitions
            for(int i=0;i<fsm_io.size();i++){                           //For loop to read the general map
                if(sta.first == fsm_io[i]["actual"]["1"]){              //If the state name is the same as the first element
                    text_out += sta.first+": \n";                       //Print the state and start a case
                    for(auto const& out:fsm_io[i]["inputs"]){           //For loop to read all the inputs
                        if((out.second != "x")&&(out.second != "X")){   //If the values is different than x|X
                            longitud += 1;                              //Increase longitud
                            if (flag == false){                         //If the flag is false
                                text_out += "if ((" + out.first +" == ";//Print the beginning of the case and the input name
                                text_out += out.second;                 //Print the input value
                                flag = true;                            //Change the flag to true
                            }else{                                      //If the flag is true
                                text_out += ") & ( " + out.first+" == ";//Print the next input condition and its name
                                text_out += out.second;                 //Print the input value
                            }
                        }
                    }
                    flag = false;                                       //Reset the flag
                    if (longitud != 0){                                 //If longitud different than 0
                            text_out += "))\n";                         //Close the statement
                            longitud = 0;                               //Restart longitud flag
                    }
                    text_out += " next_state <= "+fsm_io[i]["next"]["1"]+";\n";//Print the next state statement
                }            
            }
        }else{                                                          //If there are repetitions in the states 
            for(int i=0;i<fsm_io.size();i++){                           //For loop to read the general map
                if(sta.first == fsm_io[i]["actual"]["1"]){              //If the state name is the same as the first element
                    if (flag_states == false){                          //If the flag_states is false start the block
                            text_out +=  sta.first+" :\n";              //Start the case with its name
                            flag_states = true;                         //Change the states flag
                            text_out += "  begin\n";                    //Begin the block
                    }
                    for(auto const& out:fsm_io[i]["inputs"]){           //For loop to read all the inputs
                        if((out.second != "x")&&(out.second != "X")){   //If the values is different than x|X
                            string n_bit=to_string(out.second.size());  //See the 
                            if (flag == false){                         //If the flag is false
                                text_out += "if ((" + out.first +" == ";//Print the beginning of the case and the input name
                                text_out += n_bit + "'b" +out.second;   //Print the input value
                                flag = true;                            //Change the flag to true
                            }else{                                      //If the flag is true
                                text_out += ") & ( " + out.first+" == ";//Print the next input condition and its name
                                text_out += n_bit+"'b" +out.second;     //Print the input value
                            }
                        }
                    }
                    flag = false;                                       //Restart the flag
                    text_out += "))\n";                                 //Close the codition
                    text_out += "     next_state <= "+fsm_io[i]["next"]["1"]+";\n";//Print the next state
                }
            }
            text_out += "   end\n";                                     //End the case
            flag_states = false;                                        //Restart the states flag
        }
    }
    text_out += "  endcase\nend\n\n";                                   //Finish the cases
    //Start the output declaration
    text_out += "//-----------------------------------------------------------------------------\n";
    text_out += "// FSM Outputs assignment\n"; 
    text_out += "//-----------------------------------------------------------------------------\n";
    //Start the always statement
    text_out += "always @ (state)\n";
    text_out += "  begin\n  case(state)\n";
    bool flag_out = true;                                               //Intialize the a flag
    for(auto const& sta:rep_states(fsm_io)){                            //Read the states without repetition
        for(int i=0;i<fsm_io.size();i++){                               //For loop to read the general map
            if((sta.first == fsm_io[i]["actual"]["1"])&&(flag_out)){    //If the state name is the same as the first element 
                text_out += "    "+sta.first+": \n";                    //Print the state name and start the case
                flag_out = false;                                       //Change the flag to false
                text_out += "    begin\n";                              //Begin the case
                for(auto const& out:fsm_io[i]["outputs"]){              //For loop to read the outputs
                    int int_bit = out.second.size();                    //Read the size of the ouputs
                    string n_bit=to_string(int_bit);                    //Change to string the buffer size
                    if (int_bit == 1){                                  //If the buffer size is equal to 1
                        text_out += "        "+out.first+" = "+ out.second+";\n";//Print single output
                    }else{                                              //If the buffer is bigger than 1
                        text_out += "        "+out.first+" = " + n_bit+"'b" + out.second+";\n";//Print the outputs with its buffer
                    }
                }
            }
        }
        flag_out = true;                                                //Restart the flag
        text_out += "    end\n";                                        //Close the case
    }
    text_out += "  endcase\n";                                          //Close the state cases
    text_out += " end\n\nendmodule";                                    //End the module
    text_out += "\n\n//=============================================================================\n";
    text_out += "//=============================================================================\n\n";
    fstream  outfile;                                                   //Close the fstream of the .sv file to be generated
    outfile.open(name, ofstream::out);                                  //Open or create the output file
    outfile << text_out;                                                //Print the FSM into the ouput file
    outfile.close();                                                    //Close the file
    cout << "Your file " + name;                                        //Print the name of the file in the terminal
    cout  << " was succesful created" << endl;                          //Print a message to the user
    return 0;
}
