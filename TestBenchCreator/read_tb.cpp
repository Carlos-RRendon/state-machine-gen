#include <iostream>
#include <regex>
#include <string>
#include <fstream>
#include <map>
#include<vector>
#include<sstream>
#include <time.h>
#include <algorithm>
#include <iterator>
#include <cmath>


using namespace std;

string open_file()
{
    fstream  textFile;
    string codeVerilog, tempText;
    string path = "design1.sv";

    textFile.open(path, ifstream::in); //for test_file the path is examples/test.txt
    if(textFile.is_open()){
        cout << "File " << path << " opened" << endl << endl;
        while(!textFile.eof()){
            getline(textFile, tempText);
            codeVerilog += tempText;
            cout << tempText << endl;
        }
        textFile.close();
    }
    else
        cout << "Can't open file " << path << endl;
    return codeVerilog;
}


vector<string> splitText(string text) {
   vector<string> result;
   stringstream s_stream(text); //create string stream from the string
   while(s_stream.good()) {
      string substr;
      getline(s_stream, substr, ','); //get first string delimited by comma
      result.push_back(substr);
    }
    return result;
}

int main()
{
    //map<string, vector<string> > data_tb;
    vector<string> tmp;
    map<string,string> input, output; 
    string module_name;
    string s = open_file();

    int user_format; 
    cout << "Choose the format of vectors testing (1 decimal), (2 binary), (3 hexadecimal): "; // Type a number and press enter
    cin >> user_format; // Get user input from the keyboard


    smatch m;
    regex re_module("module\\s+([a-zA-Z]\\w*)");

    if(regex_search(s,m,re_module)){
        module_name = m[1];
        //data_tb.insert(pair<string, vector<string> > ("module_name", tmp));
        s = m.suffix().str();
    }
    
    //regex re_inout("\\W*((input|output|inout)\\s*(\\[\\d+:\\d+\\]\\s*|\\s+)\\s*(((,\\s*|\\s*)((?!input|output|inout)[_a-zA-Z]\\w*\\s*))*))");
    regex re_input("\\W*((input)\\s*(\\[\\d+:\\d+\\]\\s*|\\s+)\\s*(((,\\s*|\\s*)((?!input|output|inout)[_a-zA-Z]\\w*\\s*))*))");
    while(regex_search (s,m,re_input)) //s variable, string where the file was opened; m match_results for string objects, re_input regex pattern
    {   tmp = splitText(m[4]);
        for(int i = 0; i < tmp.size(); i++) {    //print all splitted strings
            input.insert(pair <string, string> (tmp.at(i), m[3]));
        }
        s = m.suffix().str();
    }
    cout<<endl<<endl<<s<<endl<<endl;

    regex re_output("\\W*((output)\\s*(reg|\\s*)\\s*(\\[\\d+:\\d+\\]\\s*|\\s+)\\s*(((,\\s*|\\s*)((?!input|output|inout)[_a-zA-Z]\\w*\\s*))*))");
    while(regex_search (s,m,re_output))
    {
        tmp = splitText(m[5]);
        for(int i = 0; i < tmp.size(); i++) {    //print all splitted strings
            output.insert(pair <string, string> (tmp.at(i), m[4]));
        }
        s = m.suffix().str();
    }
    cout<<endl<<endl<<s<<endl<<endl;
    
    //cout << s <<endl;
    cout << "Module name: " << module_name << endl;

    // Generation of random vectors
    stringstream hex_stream;
    std::vector<string> key, value;
    std::vector<string> random_vectors; 
    std::string string_input, string_bus, formatted_string, complete_string, clock_input, reset_input;
    clock_input.clear();
    reset_input.clear();

    for(std::map<string,string>::iterator i = input.begin(); i != input.end(); ++i) 
    {
        key.push_back(i -> first);
        value.push_back(i -> second);
    }

    for(size_t i = 0; i < key.size(); ++i)
    {
        string_input = key[i];
        string_bus = value[i];

        if ((string_input == "clock") || (string_input == "clk"))
        {
            clock_input = string_input;
        }
        else if ((string_input == "reset") || (string_input == "rst"))
        {
            reset_input = string_input;
        }
        else
        {
       
            string_bus.erase(std::remove(string_bus.begin(), string_bus.end(), ' '), string_bus.end());

            std::regex rgx("\\[\\s*([\\d]*)\\s*\\W*(\\d*)");  

            if (std::regex_search(string_bus, m, rgx))
            {
                std::string element_1 = m.str(1);
                std::string element_2 = m.str(2);
                int bus_A = std::stoi(element_1);
                int bus_B = std::stoi(element_2);
                
                int bus_width = ((bus_A - bus_B) + 1);
                unsigned long long int bus_range = pow(2, bus_width);
                bus_range -= 1;
                srand (time(NULL));
                unsigned long long int random_number = rand() % bus_range;

                switch (user_format) //(1 decimal), (2 binary), (3 hexadecimal)
                {
                    case 1:{
                        formatted_string =  std::to_string(bus_width) + "'d" + std::to_string(random_number);
                        break;
                    }
                        
                    case 2:{
                        formatted_string = std::bitset<32>(random_number).to_string();
                        formatted_string =  std::to_string(bus_width) + "'b" + formatted_string;
                        break;
                    }

                    case 3:{
                        hex_stream.str(std::string());
                        hex_stream << hex << random_number; 
                        formatted_string = hex_stream.str();
                        formatted_string =  std::to_string(bus_width) + "'h" + formatted_string;
                        break;
                    }
                }

                complete_string = string_input + "_tb = " + formatted_string + ";"; 
                random_vectors.push_back(complete_string); 
            }

            else 
            {
                srand (time(NULL));
                unsigned long long int random_number = rand() % 2;
                complete_string = string_input + "_tb = " + std::to_string(random_number) +";";
                random_vectors.push_back(complete_string); 
    
            }
    
        }      
            
    }
    
    
        
    

    


    //Inicio de generacion de testbench 
    string text_tb;
    text_tb = "`timescale 1ns / 1ps\n"
                "module " + module_name + "_TB;\n";

    for (auto const& pair: input) {
        text_tb += "\treg " + pair.second + " " + pair.first + "_TB;\n";
	}
    for (auto const& pair: output) {
        text_tb += "\twire " + pair.second + " " + pair.first + "_TB;\n";
	}
    text_tb += module_name + " UUT(";
    for (auto const& pair: input) {
        text_tb += " ." + pair.first + "("+ pair.first + "_TB),";
	}
    for (auto const& pair: output) {
        text_tb += " ." + pair.first + "("+ pair.first + "_TB),";
	}
    text_tb.pop_back();
    text_tb += ");\n"
            "\ninitial\n"
            "\tbegin\n";
    text_tb += "\n      $dumpfile("+ module_name +".vcd);\n";
    text_tb += "      $dumpvars(1,"+ module_name+"_TB);\n";
    text_tb+=  "      " + clock_input + "_tb=0;\n";
    text_tb+=  "      " + reset_input + "_tb=0;\n\n";
    for (vector<string>::iterator it= random_vectors.begin();it!= random_vectors.end();it++)
    {
        text_tb += "      " + *it + "\n";
    }
    text_tb += "\n      $finish\n\n";
    text_tb += "     end\n\n";
    text_tb += "    always\n";
    text_tb += "      begin\n";
    text_tb += "        #1  ";
    text_tb += clock_input + "_tb = ~" + clock_input + "_tb\n" ;
    text_tb += "      end\n\n";
    text_tb += "endmodule";


    fstream  outfile;
    outfile.open("output.txt", ofstream::out);
    outfile << text_tb;
    outfile.close();

}