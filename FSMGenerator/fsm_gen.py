#!/usr/bin/env python

import json
import math

class FsmGenerator():


# -------------------------------------------------------------------------------------------
# Reading of the manually filled .JSON file
# -------------------------------------------------------------------------------------------    

    def __init__(self, file):    #Initialize the class

        with open(file) as json_file:
            self.data = json.load(json_file)  #Creating an attribute data that will contain all the JSON info

        self.verify()

# -------------------------------------------------------------------------------------------
# Integrity verification of the manually filled .JSON file
# -------------------------------------------------------------------------------------------

    def verify(self):
        key_words = [["format", "data"], ["actual_state", "next_state", "inputs", "outputs"]] #Keywords to check the information of the user 

        flag_l1 = 0     #Flags to check the JSON data keywords 
        flag_l2 = 0

        for key in self.data.keys():   
            if key in key_words[0]:
                flag_l1 += 1

        if flag_l1 == len(self.data):
            for file, element in enumerate(self.data["data"]):   
                for key in self.data["data"][file].keys():
                    if key in key_words[1]:
                        flag_l2 += 1

                    if key == "inputs":                                        #Data treatment for inputs
                        for input in self.data['data'][file]["inputs"]:        
                            if len(input)==1:                                  #Checking if the input has only the name and no value 
                                input.append(-1)                               #If the value is missing append -1 
                            if input[1] == ("x" or "X"):                       #If the value is a don't care condition 
                                input[1] = -1                                  #Append a -1



        if flag_l2 % len(self.data["data"]) == 0:   #Checking the length of the flags to verify the JSON contains all the keywords
            print("Verified file")
            return True

        else:
            print("Warning!!!! Wrong format")
            return False

# -------------------------------------------------------------------------------------------
# FSM States detection
# -------------------------------------------------------------------------------------------           

    def state_treatment(self):

        import collections

        states = []

        for state in self.data["data"]:        
            states.append(state["actual_state"])     #Check the total non repeated states

        #states = sorted(states)
        states = collections.Counter(states)  #Making the states array into a dictionary
        return states                         #Returning the dictoinary with the name of the states and the number of repetitions

# -------------------------------------------------------------------------------------------
# FSM Ports width identification 
# ------------------------------------------------------------------------------------------- 

    def find_bus(self,key):  #This function returns the bis width depending the max value of the inputs/outputs

        bus_dict = {}

        for line in self.data["data"]:              #Iterator into the subsections inputs/outputs of data

            for input in line[key]:                    

                if input[0] in bus_dict.keys():

                    bus_dict[input[0]].append(input[1])
                else:
                    bus_dict[input[0]] = [input[1]]       #Bus_dict will contain all the values of inputs/outputs

        for key in bus_dict.keys():

            max_el = max(bus_dict[key])                     #This function calculates the biggest value of the bus_dict array

            try:
                max_el = math.floor(math.log(max_el)/math.log(2))+1 #Calculate the number of bits that will be needed to represent the value
                bus_dict[key] = max_el                             
            except ValueError:
                bus_dict[key] = -1


        return bus_dict                    #Return the dictionary with all the name of the inputs/outputs with its biggest value


# -------------------------------------------------------------------------------------------
# Generation of the Verilog textfile design
# -------------------------------------------------------------------------------------------        

    def fsm_creator(self):

        import os   

        states = self.state_treatment()         #Calling the method that will return the name of the states and its number of repetitions 
        inputs_bus = self.find_bus("inputs")    #Calling the method that will return the name of the all inputs with the bus
        outputs_bus = self.find_bus("outputs")  #Calling the method that will return the name of the all outputs with the bus

        name = "FSM_P.sv"                       #Generating the name of the new file 
        
        #Adding comments for the header of the template 
        
        text_out = "//=============================================================================\n"
        text_out += "// FSM Verilog design\n"
        text_out += "//=============================================================================\n\n\n"

        #Adding comments for name and port declarations to the template

        text_out += "//-----------------------------------------------------------------------------\n"
        text_out += "// Module name and ports declaration\n" 
        text_out += "//-----------------------------------------------------------------------------\n\n"   
        
        
        
        text_out += "module state_machine (\n"    #Adding the name of the module in the template

        text_out += "input clk, rst,\n"           #Adding the clock and the reset inputs 

        for key, value in inputs_bus.items():     #For to add all the inputs with it's bus length
            if value == 1 :
                text_out += f'input {key},\n'     #If to check if the bus with is 0 or 1 to add only the name
            else:
                text_out += f'input [{value-1}:0] {key},\n'   #If it has a bus length bigger than 1 then put the bus lenght 


        for key, value in outputs_bus.items():    #For to add all the outputs with it's bus length

            if value == 1 :                        #If to check if the bus with is 0 or 1 to add only the name
                text_out += f'output reg {key}'
            else:                                             #If it has a bus length bigger than 1 then put the bus lenght 
               text_out += f'output reg [{value-1}:0] {key}'

            if key != (list(outputs_bus)[-1]):          #Conditional to catch the last element of the outputs list to avoid the ,
                text_out += ",\n"


        text_out += ");\n\n"              #Closing the input-outputs declaration in the module

        #Adding comments to the states declaration
        text_out += "//-----------------------------------------------------------------------------\n"
        text_out += "// FSM states declaration\n" 
        text_out += "//-----------------------------------------------------------------------------\n" 
        
        bits = len(states)                     #Calculating the number of states
        bits = math.ceil(math.log2(bits))      #Calculating the number of bits to represent all states
        first_state = None                     

        for num, state in enumerate(states):  #Iter into the states dictionary

            if num == 0:
                first_state = state           #Creating a variable  that will determine the initial state

            num = bin(num).replace("0b","")   #Converting  the total num of states into the bus lengh to the state variables

            text_out += f"parameter {state} = {bits}'b{num};\n"  #Assign a value codification for each state and formating into verolog sintax

        if bits >= 2:

            text_out += f'reg [{bits-1}:0] state, next_state;\n'    #Declarating the state with the bus width i,e s0,s1,s2 will need 2 bits
        else:
            text_out += 'reg state, next_state; \n'                 #If it has nos bus only declare the state with it's names, ie:s0,s1 only need 1 bit

        text_out += " \n //FSM Initialization state"                #Comments for the template

        text_out += '\ninitial begin\n  state=0;\nend\n'            #Adding the state initialization into the initial block in verilog

        text_out += " \n //FSM State transitions (clock dependant)"   #Comments for the template

        text_out += "\nalways @ (posedge clk or rst)\n  begin\n  if (rst) state <= "  #Adding the always block for the change of states

        text_out += f"{first_state};\n"                              #Adding the default state if rst

        text_out += "  else state <= next_state;\n  end\n\n"        #Adding the assignation of next states if clk

        #Comments for states assignment fo the template 
        text_out += "//-----------------------------------------------------------------------------\n"
        text_out += "// FSM States assignment\n" 
        text_out += "//-----------------------------------------------------------------------------\n" 

        text_out += f"always @ (state, "             #Adding the always block for the contition of state transition

        for key in inputs_bus.keys():                #For to iter into the inputs to add the sentitivity list

            text_out += f'{key}'                     # Adding all the inputs that will change the state

            if key != (list(inputs_bus)[-1]):        #If to check if it's the last input to close 
                text_out += ', '

        text_out += ')\n'                            #Close the always statement
        text_out += 'begin\n case(state)\n'          #Adding the case statement
        flag = False                                 #Flags to determine the close of parenthesis if each state has 1 or more condition
        flag_states = False                          #Flag to determine if a state has 1 or more transition
        longitud = 0                                 # Numerical flag to avoid dont'care inputs

        for key,value in states.items():             #Iter into the states dictionary to ensure that they will no repeat into declaration
            if value == 1:                           #If only repeats one time
                for row in self.data["data"]:        #Iter into the original dictionary
                    if key ==row["actual_state"]:    #When find the actual state 
                        text_out += f'{row["actual_state"]}: \n'      #Add it into the template 
                        for input in row["inputs"]:                   #Iter into the inputs that make the transition of the state
                            if input[1] != -1:                        #If there is a don't care condition or a missing value
                                longitud += 1                         #Add 1 to the numerical flag
                                if flag == False:                     #Flag to determine if the condition has already added
                                    text_out += f"if (({input[0]} == {input[1]}"  #If is the first time add the input and its value 
                                    flag = True                                 #Change the flag to avoid repetition
                                else:
                                    text_out += f" ) & ( {input[0]} == {input[1]}"    #If has alredy passed check the next inputs and add it

                        flag = False                                                  #Reset the flag for the next state
                        #if (longitud == len(row['inputs'])) or ((len(row['inputs'])-longitud)==1):     
                        if (longitud != 0):                                       #Check if there are other transition in the same state
                            text_out += "))\n"                                    #If not close the parenthesis in the declaration
                            longitud = 0

                        text_out += f"  next_state <= {row['next_state']};\n"      #Add transition to the next state

            else:                                                         #If the state has more repetitions
                for row in self.data["data"]:
                    if key ==row["actual_state"]:                         #Check the the actual state is the same that the state we're writing
                        if flag_states == False:                          #Check if the state has already pass
                            text_out += f'{row["actual_state"]}: \n'      #Add the state into the template
                            flag_states = True                            #Put true to avoid overwritting
                            text_out += "  begin\n"                       #Put the beggin statement for to declare the next conditions inside the state
                        for input in row["inputs"]:                       #Iter into the inputs for the transition
                            if input[1] != -1:                            # Check if there is a missing value or don't care
                                if flag == False:                         #Condition to check if it has only one change condition
                                    text_out += f"   if (({input[0]} == {input[1]}"  #If not add the input into the template
                                    flag = True                         #Change the flag to avoid input rewriting
                                else:                                   #If are more conditions add it into the template
                                    text_out += f" ) & ( {input[0]} == {input[1]}"    #Adding the next change condition

                        flag = False                              #Check the last condition 
                        text_out += "))\n"                       #Close the parenthesis
                        text_out += f"     next_state <= {row['next_state']};\n"   #Add the next condition into the template

                text_out += "   end\n"     #Close the actual state case
                flag_states = False        #Change the flag to the next state



        text_out += "  endcase\nend\n\n"    #Close the case tag into the template


        #Adding output comments into the template 
        text_out += "//-----------------------------------------------------------------------------\n"
        text_out += "// FSM Outputs assignment\n" 
        text_out += "//-----------------------------------------------------------------------------\n"

        text_out += "always @ (state)\n"   #Adding the outputs assignment always block 

        text_out += "  begin\n  case(state)\n"     #Adding the begin case declaration into the template

 
        flag_out = True       #Flag to check the outputs 

        for key, value in states.items():    #iter into the states dictionary
            for row in self.data["data"]:      #Iter into the original data
                if key == row["actual_state"] and flag_out:       #Contition to verify the actual state and avoid repetition
                    text_out += f'    {row["actual_state"]}: \n'  #Adding the actual state into the template 
                    flag_out = False                              #Flag to avoid output repetition into the same state
                    text_out += "    begin\n"                     #Adding the begin statement into the template
                    for outputs in row["outputs"]:               #Iter for the outputs declaration given by the user 
                        text_out += f'        {outputs[0]} = {outputs[1]};\n'   #Adding the output and it's value

            flag_out = True              #Change the flag to avoid overwriten

            text_out += "    end\n"      #Adding the end statement into the template
        text_out += "  endcase\n"        #Closing the case declaration into the module  
        text_out += " end\n\nendmodule"  #Adding the endmodule tag into the template
         
        #Adding end comment for the file 
        text_out += "\n\n//=============================================================================\n"
        text_out += "//=============================================================================\n\n"
        
        #Print for user confirmation
        print(f"Your file {name} has been created")
        f = open(name, "w")      #Creation of the file
        f.write(text_out)        #Dump the text into the file
        f.close                  #Close the document

if __name__ == "__main__":
    gen = FsmGenerator("control_fsm.json")
    gen.fsm_creator()
