#include <iostream>
#include <regex>
#include <string>
#include <fstream>

using namespace std;
 
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
            cout << Outputs << endl;
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

    for(int i=0;i<fsm_io.size();i++){
        cout << "actual => " << fsm_io[i]["actual"]["1"] << endl;
        cout << "next => " << fsm_io[i]["next"]["1"] << endl;
        for(auto const& in:fsm_io[i]["inputs"]){
            cout << in.first << " => " << in.second << endl;
        }
        for(auto const& out:fsm_io[i]["outputs"]){
            cout << out.first << " => " << out.second << endl;
        }
        cout << endl;
    }

    inFile.close();
    return 0;
}