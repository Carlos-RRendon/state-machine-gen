#!/usr/bin/env python
import re
import random


class tesbench_creator:

    def __init__(self, path):
        self.path = path
        self.content = ""
        self.elements = {
            'module': None,
            'inputs': None,
            'outputs': None,
            'clk' : None,
            'rst' : None
        }
        self.abrir_archivo()
        self.data_clean = self.extract_info()

    def abrir_archivo(self):
        f = open(self.path, 'r')
        content = f.read()

        # Quita los commentarios
        content = re.sub("\/.*", "", content)
        
        # Quita los saltos de linea
        content = content.replace("\n", " ")

        f.close()

        # Actualiza el atributo content
        self.content = self.content + content

    def extract_info(self):

        self.abrir_archivo()

        pattern = '(module.*\(.*\)|input.*|output.*|inout.*)'
        data_clean = []

        self.content = self.content.split(";")

        for line in self.content:

            match = re.search(pattern, line)

            if match:
                data = match.group(1)
                data_clean.append(data)

        return data_clean

    def find_module(self):
        content = self.data_clean

        for i in content:

            pattern_module = re.search('module\s+([a-zA-Z]\w*)', i)

            if pattern_module:

                string_module = pattern_module.group(1)
                print(string_module)
                self.elements['module'] = string_module
                break

            else:
                print("No se encontro nombre del modulo")
                break

    def find_inputs(self):
        content = self.data_clean

        # INPUTS
        # ======================================================================================
        # ======================================================================================

        string_aux = " ".join(content)
        
        result_search_aux = re.findall(
            "\W*((input)\s*(\[\d+:\d+\]\s*|\s+)\s*(((,\s*|\s*)((?!input |output |inout )[_a-zA-Z]\w*\s*))*))", string_aux)

        inputs = []
        for i in range(len(result_search_aux)):
            string_raw_inputs = result_search_aux[i][0].replace("input", "")
            string_raw_inputs = string_raw_inputs.replace(" module ", "")
            string_input_aux_2 = re.search("(^|\s+)\[(.*?)\]", string_raw_inputs)
            if string_input_aux_2:
                input_bus_width = string_input_aux_2.group(0)
                input_bus_width = input_bus_width.replace(" ", "")
                string_input_aux_3 = string_raw_inputs.replace(input_bus_width, "")
                # string_input_aux_4 = re.findall("([_a-z0-9-]+)(?:,|\s)", string_input_aux_3)
                string_input_aux_4 = re.findall("\s+(\w*)", string_input_aux_3)
                if string_input_aux_4:
                    for j in range(len(string_input_aux_4)):
                        if not ((string_input_aux_4[j] in dict(inputs))) and (string_input_aux_4[j]):
                            inputs.append(tuple((string_input_aux_4[j], input_bus_width)))
                else:
                    string_input_aux_3 = string_input_aux_3.replace(" ", "")
                    if not ((string_input_aux_3 in dict(inputs))) and (string_input_aux_3):
                        inputs.append(tuple((string_input_aux_3, input_bus_width)))

            else:
                input_bus_width = tuple()
                string_input_aux_3 = re.findall("\s+(\w*)", string_raw_inputs)
                if string_input_aux_3:
                    for j in range(len(string_input_aux_3)):
                        if (not (string_input_aux_3[j] in dict(inputs))) and (string_input_aux_3[j]):
                            inputs.append(tuple((string_input_aux_3[j], input_bus_width)))
                else:
                    string_input_aux_3 = string_raw_inputs.replace(" ", "")
                    if not (string_input_aux_3 in dict(inputs)) and (string_input_aux_3):
                        inputs.append(tuple((string_input_aux_3, input_bus_width)))

        self.elements["inputs"] = inputs

    def find_outputs(self):
        # OUTPUTS
        # ======================================================================================
        # ======================================================================================
        content = self.data_clean
        string_aux = " ".join(content)
        result_search_aux = re.findall(
            "\W*((output)\s*(logic|reg|\s*)\s*(\[\d+:\d+\]\s*|\s+)\s*(((,\s*|\s*)((?!input |output |inout |logic )[_a-zA-Z]\w*\s*))*))",
            string_aux)

        outputs = []
        for i in range(len(result_search_aux)):
            string_raw_output = result_search_aux[i][0].replace("output", "")
            string_raw_output = string_raw_output.replace("reg", "")
            string_raw_output = string_raw_output.replace("logic", "")
            string_raw_output = string_raw_output.replace("module", "")
            string_output_aux_2 = re.search("(^|\s+)\[(.*?)\]", string_raw_output)
            if string_output_aux_2:
                output_bus_width = string_output_aux_2.group(0)
                output_bus_width = output_bus_width.replace(" ", "")
                string_output_aux_3 = string_raw_output.replace(output_bus_width, "")
                string_output_aux_4 = re.findall("\s+(\w*)", string_output_aux_3)
                if string_output_aux_4:
                    for j in range(len(string_output_aux_4)):
                        if (not (string_output_aux_4[j] in dict(outputs))) and (string_output_aux_4[j]):
                            outputs.append(tuple((string_output_aux_4[j], output_bus_width)))
                else:
                    string_output_aux_3 = string_output_aux_3.replace(" ", "")
                    if (not (string_output_aux_3 in dict(outputs))) and (string_output_aux_3):
                        outputs.append(tuple((string_output_aux_3, output_bus_width)))

            else:
                output_bus_width = tuple()
                string_output_aux_3 = re.findall("\s+(\w*)", string_raw_output)
                if string_output_aux_3:
                    for j in range(len(string_output_aux_3)):
                        if (not (string_output_aux_3[j] in dict(outputs))) and (string_output_aux_3[j]):
                            outputs.append(tuple((string_output_aux_3[j], output_bus_width)))
                else:
                    string_output_aux_3 = string_raw_output.replace(" ", "")
                    if (not (string_output_aux_3 in dict(outputs))) and (string_output_aux_3):
                        outputs.append(tuple((string_output_aux_3, output_bus_width)))

        self.elements["outputs"] = outputs

    def vector_signals(self):
        #self.find_inputs()
        inputs = self.elements["inputs"]

        inputs = (list(filter(lambda x: x[0] != "clk", inputs)))
        inputs = (list(filter(lambda x: x[0] != "clock", inputs)))
        inputs = (list(filter(lambda x: x[0] != "rst", inputs)))
        inputs = (list(filter(lambda x: x[0] != "reset", inputs)))


        user_format = int(input("Elija el formato de los números (1 decimal), (2 binario), (3 hexadecimal): "))

        inputs_vector = []
        for j in inputs:

            if j[1] != () :
                result_search_aux = re.search("\[\s*([\d]*)\s*\W*(\d*)", j[1])
                element_1 = int(result_search_aux.group(1))
                element_2 = int(result_search_aux.group(2))
         
                bus_width = ((element_1 - element_2) + 1)
                bus_range = 2 ** bus_width
                random_number = random.randint(0, (bus_range - 1))

                if user_format == 1:
                    prefix = "'d"

                elif user_format == 2:
                    random_number = bin(random_number).replace('0b','')
                    prefix = "'b"
                elif user_format == 3:
                    random_number = hex(random_number).replace('0x','')
                    prefix = "'h"


                dec_string = f"{j[0]}_tb = {bus_width}{prefix}{random_number}"
                inputs_vector.append(dec_string)

            else :

                random_number = random.randint(0, 1)

                if user_format == 1:
                    prefix = "'d"

                elif user_format == 2:
                    random_number = bin(random_number).replace('0b','')
                    prefix = "'b"
                elif user_format == 3:
                    random_number = hex(random_number).replace('0x','')
                    prefix = "'h"

                # Decimal
                dec_string = f"{j[0]}_tb = 1{prefix}{random_number}"
                inputs_vector.append(dec_string)

        return(inputs_vector)

    def clock_signal(self):
        #self.find_inputs()
        clock = self.elements["inputs"]  

        clock = (list(filter(lambda x: x[0] == "clock", clock)))
        if not clock :
            clock = self.elements["inputs"]
            clock = (list(filter(lambda x: x[0] == "clk", clock)))
            clock = clock[0][0]
        else : 
            clock = clock[0][0]

        self.elements['clock'] = clock

        return clock

    def reset_signal(self):
        #self.find_inputs()
        reset = self.elements["inputs"]  

        reset = (list(filter(lambda x: x[0] == "reset", reset)))
        if not reset :
            reset = self.elements["inputs"]
            reset = (list(filter(lambda x: x[0] == "reset", reset)))
            reset = reset[0][0]
        else : 
            reset = reset[0][0]

        self.elements['reset'] =reset

        return reset

    def tb_create(self):

        self.find_module()
        self.find_inputs()
        self.find_outputs()
        self.clock_signal()
        self.reset_signal()
        vectores = self.vector_signals()

        import os
        name = os.path.splitext(self.path)
        new_name = name[0] + "_tb" + name[1]
        inst_name = "UUT"
        f = open(new_name,"w")

        for key in self.elements.keys():

            if key == "module":
                tb_module_name = f"module {self.elements[key]}_tb;\n"
                print(tb_module_name)
                f.write(tb_module_name)
                connect_ports = f'\n{self.elements[key]} {inst_name} (\n'
                print(connect_ports)

            if key == "inputs":

                for input in self.elements[key]:


                    if input[1] != ():
                        tb_in_name = f'reg {input[1]} {input[0]}_tb;\n'
                        print(tb_in_name)
                        f.write(tb_in_name)
                        connect_ports = connect_ports + f'.{input[0]} ({input[0]}_tb),\n'


                    else:
                        tb_in_name = f'reg {input[0]}_tb;\n'
                        print(tb_in_name)
                        f.write(tb_in_name)
                        connect_ports = connect_ports + f'.{input[0]} ({input[0]}_tb),\n'

            if key == "outputs":

                flag = len(self.elements[key])
                last_element = self.elements[key][flag-1]


                for output in self.elements[key]:

                    if output != last_element:


                        if output[1] != ():
                            tb_out_name = f'wire {output[1]} {output[0]}_tb;\n'
                            print(tb_out_name)
                            f.write(tb_out_name)
                            connect_ports = connect_ports + f'.{output[0]} ({output[0]}_tb),\n'


                        else:
                            tb_out_name = f'wire {output[0]}_tb;\n'
                            print(tb_out_name)
                            f.write(tb_out_name)
                            connect_ports = connect_ports + f'.{output[0]} ({output[0]}_tb),\n'

                    else:
                        tb_out_name = f'wire {output[0]}_tb;\n'
                        print(tb_out_name)
                        f.write(tb_out_name)
                        connect_ports = connect_ports + f'.{output[0]} ({output[0]}_tb)\n);'


        f.write(connect_ports+"\n")
        f.write(f'\ninitial\n')
        f.write(f'\n  begin \n\n')
        f.write(f'    $dumpfile("{self.elements["module"]}_tb.vcd");\n')
        f.write(f'    $dumpvars(0,"{self.elements["module"]}_tb");\n\n')
        f.write(f'    {self.elements["clock"]}_tb=0;\n')
        f.write(f'    {self.elements["reset"]}_tb=0;\n\n')
        f.write(f'    #1\n')

        for vector in vectores:
            generated_vectors = f'    {vector};\n'
            f.write(generated_vectors)

        f.write(f'\n    $finish\n\n')
        f.write(f'  end\n\n')

        f.write(f'  always\n')
        f.write(f'  begin\n')
        f.write(f'  #1')
        f.write(f'  {self.elements["clock"]}_tb = ~{self.elements["clock"]}_tb\n')
        f.write(f'  end\n\n')
        f.write(f'endmodule')
        f.close()


if __name__ == "__main__":

    files = [
        "regex_breaker_rev_FPM.sv"
    ]

    for file in files:
        creator = tesbench_creator(file)
        print("This file is: ", file)
        creator.find_outputs()
        creator.clock_signal()
        creator.reset_signal()
        creator.vector_signals()
        

