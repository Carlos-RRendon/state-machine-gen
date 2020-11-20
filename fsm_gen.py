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

                    if key == "inputs":
                        for input in self.data['data'][file]["inputs"]:
                            if len(input)==1:
                                input.append(-1)
                            if input[1] == ("x" or "X"):
                                input[1] = -1



        if flag_l2 % len(self.data["data"]) == 0:   #Checking the lenfg of the flags to verify the JSON contains all the keywords
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
        
        #Adding comments for the template 
        
        text_out = "//=============================================================================\n"
        text_out += "// FSM Verilog design\n"
        text_out += "//=============================================================================\n\n\n"

        #Adding comments to the template

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

        #Adding more coments to separate the sections
        text_out += "//-----------------------------------------------------------------------------\n"
        text_out += "// FSM states declaration\n" 
        text_out += "//-----------------------------------------------------------------------------\n" 
        
        bits = len(states)                     #
        bits = math.ceil(math.log2(bits))
        first_state = None

        for num, state in enumerate(states):

            if num == 0:
                first_state = state

            num = bin(num).replace("0b","")

            text_out += f"parameter {state} = {bits}'b{num};\n"

        if bits >= 2:

            text_out += f'reg [{bits-1}:0] state, next_state;\n'
        else:
            text_out += 'reg state, next_state; \n'

        text_out += " \n //FSM Initialization state"     

        text_out += '\ninitial begin\n  state=0;\nend\n'

        text_out += " \n //FSM State transitions (clock dependant)"   

        text_out += "\nalways @ (posedge clk or rst)\n  begin\n  if (rst) state <= "

        text_out += f"{first_state};\n"

        text_out += "  else state <= next_state;\n  end\n\n"


        text_out += "//-----------------------------------------------------------------------------\n"
        text_out += "// FSM States assignment\n" 
        text_out += "//-----------------------------------------------------------------------------\n" 

        text_out += f"always @ (state, "

        for key in inputs_bus.keys():

            text_out += f'{key}'

            if key != (list(inputs_bus)[-1]):
                text_out += ', '

        text_out += ')\n'
        text_out += 'begin\n case(state)\n'
        flag = False
        flag_states = False
        longitud = 0

        for key,value in states.items():
            if value == 1:
                for row in self.data["data"]:
                    if key ==row["actual_state"]:
                        text_out += f'{row["actual_state"]}: \n'
                        for input in row["inputs"]:
                            if input[1] != -1:
                                longitud += 1
                                if flag == False:
                                    text_out += f"if (({input[0]} == {input[1]}"
                                    flag = True
                                else:
                                    text_out += f" ) & ( {input[0]} == {input[1]}"

                        flag = False
                        #if (longitud == len(row['inputs'])) or ((len(row['inputs'])-longitud)==1):
                        if (longitud != 0):
                            text_out += "))\n"
                            longitud = 0

                        text_out += f"  next_state <= {row['next_state']};\n"

            else:
                for row in self.data["data"]:
                    if key ==row["actual_state"]:

                        if flag_states == False:
                            text_out += f'{row["actual_state"]}: \n'
                            flag_states = True
                            text_out += "  begin\n"
                        for input in row["inputs"]:
                            if input[1] != -1:
                                if flag == False:
                                    text_out += f"   if (({input[0]} == {input[1]}"
                                    flag = True
                                else:
                                    text_out += f" ) & ( {input[0]} == {input[1]}"

                        flag = False
                        text_out += "))\n"
                        text_out += f"     next_state <= {row['next_state']};\n"

                text_out += "   end\n"
                flag_states = False



        text_out += "  endcase\nend\n\n"



        text_out += "//-----------------------------------------------------------------------------\n"
        text_out += "// FSM Outputs assignment\n" 
        text_out += "//-----------------------------------------------------------------------------\n"

        text_out += "always @ (state)\n"

        text_out += "  begin\n  case(state)\n"


        flag_out = True

        for key, value in states.items():
            print(key)
            for row in self.data["data"]:
                if key == row["actual_state"] and flag_out:
                    text_out += f'    {row["actual_state"]}: \n'
                    flag_out = False
                    text_out += "    begin\n"
                    for outputs in row["outputs"]:
                        text_out += f'        {outputs[0]} = {outputs[1]};\n'

            flag_out = True

            text_out += "    end\n"
        text_out += "  endcase\n"
        text_out += " end\n\nendmodule"

        text_out += "\n\n//=============================================================================\n"
        text_out += "//=============================================================================\n\n"

        print(text_out)
        print(f"Your file {name} has been created")
        f = open(name, "w")
        f.write(text_out)
        f.close




if __name__ == "__main__":
    gen = FsmGenerator("control_fsm.json")
    gen.fsm_creator()
