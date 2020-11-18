import json
import math


class FsmGenerator():

    def __init__(self, file):

        with open(file) as json_file:
            self.data = json.load(json_file)

    def verify(self):
        key_words = [["format", "data"], ["actual_state", "next_state", "inputs", "outputs"]]

        flag_l1 = 0
        flag_l2 = 0

        for key in self.data.keys():
            if key in key_words[0]:
                flag_l1 += 1

        if flag_l1 == len(self.data):
            for file, element in enumerate(self.data["data"]):
                for key in self.data["data"][file].keys():
                    if key in key_words[1]:
                        flag_l2 += 1

        if flag_l2 % len(self.data["data"]) == 0:
            print("Verified file")
            return True

        else:
            print("Warning!!!! Wrong format")
            return False

    def state_treatment(self):

        import collections

        states = []

        for state in self.data["data"]:
            states.append(state["actual_state"])

        #states = sorted(states)
        states = collections.Counter(states)
        return states


    def find_bus(self,key):

        bus_dict = {}

        for line in self.data["data"]:

            for input in line[key]:

                if input[0] in bus_dict.keys():

                    bus_dict[input[0]].append(input[1])
                else:
                    bus_dict[input[0]] = [input[1]]

        for key in bus_dict.keys():

            max_el = max(bus_dict[key])
            max_el = math.ceil(math.log2(max_el))
            bus_dict[key] = max_el

        return bus_dict


    def fsm_creator(self):

        import os

        states = self.state_treatment()
        inputs_bus = self.find_bus("inputs")
        outputs_bus = self.find_bus("outputs")


        name = "FSM.sv"

        text_out = "module state_machine (\n"

        text_out += "input clk, rst,\n"

        for key, value in inputs_bus.items():
            if value == 0 :
                text_out += f'input {key},\n'
            else:
                text_out += f'input [{value-1}:0] {key},\n'

        for key, value in outputs_bus.items():

            if value == 0 :
                text_out += f'output {key}'
            else:
                text_out += f'output [{value-1}:0] {key}'

            if key != (list(outputs_bus)[-1]):
                text_out += ",\n"


        text_out += ");\n\n"

        bits = len(states)
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

        text_out += '\ninitial begin\n  state=0;\nend\n'

        text_out += "\nalways @ (posedge clk or rst)\n  begin\n  if (rst) <= "

        text_out += f"{first_state};\n"

        text_out += "  else state <= next_state;\n  end\n"

        text_out += "always @ (state,"




        #print(first_state)

        #print(inputs_bus,outputs_bus)
        print(text_out)

        #f = open(new_name, "w")




if __name__ == "__main__":
    gen = FsmGenerator("data.json")
    gen.fsm_creator()
