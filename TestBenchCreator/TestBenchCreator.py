#!/usr/bin/env python
import re
import random


class TestbenchCreator:

    def __init__(self, path):
        self.path = path
        self.content = ""
        self.elements = {
            'module': None,
            'inputs': None,
            'outputs': None,
            'clk': None,
            'rst': None
        }
        #self.abrir_archivo()
        #self.data_clean = self.extract_info()

    def abrir_archivo(self):

        f = open(self.path, 'r')
        content = f.read()
        f.close()
        # Quita comentarios multilinea
        content = re.sub('\/\*[\s|\w]*\*\/', "", content)

        # Quita los commentarios
        content = re.sub("\/.*", "", content)

        # Quita los saltos de linea
        content = content.replace("\n", " ")

        # Actualiza el atributo content y regresa su valor
        self.content = content
        return content

    def extract_info(self):

        self.abrir_archivo()

        pattern = '(module.*\(.*\)|input.*|output.*|inout.*)'
        data_clean = []

        self.content = self.content.split(";")

        for line in self.content:

            match = re.search(pattern, line)

            if match:
                data = match.group(1)
                data = re.sub("\s{2,}", "", data)
                data_clean.append(data)

        self.content = data_clean
        return data_clean

    def find_module(self):
        self.extract_info()
        content = self.content

        for i in content:

            pattern_module = re.search('module\s+([a-zA-Z]\w*)', i)

            if pattern_module:

                string_module = pattern_module.group(1)

                self.elements['module'] = string_module
                break

            else:
                print("No se encontro nombre del modulo")
                break

        print(f"The module name is: {string_module}")

        return string_module

    def __find_all(self,list, type, regex):
        list = list
        outs = []
        regex = regex
        for element in list:
            matches = re.finditer(regex, element, re.MULTILINE)
            for num, match in enumerate(matches, start=1):
                groups = match.groups()
                for groupnum in range(1, len(groups) + 1):
                    data = match.group(groupnum)
                    if data != None:
                        outs.append(data)

        data_return = []
        bus_regex = "(\[.*\])"

        for element in outs:
            separate_outputs = element.split(type)
            for elements in separate_outputs:
                elements = re.sub("\s*", "", elements)
                if elements:
                    match = re.search(bus_regex, elements)
                    if match:
                        bus = match.group(1)
                        elements = elements.replace(bus, "")
                        elements = elements.split(",")
                        for element in elements:
                            if element != "":
                                output = tuple((element, bus))
                                data_return.append(output)
                    else:
                        elements = elements.split(",")
                        for element in elements:
                            if element != '':
                                output = tuple((element, ()))
                                data_return.append(output)

        return data_return

    def find_inputs(self):
        self.extract_info()
        content = self.content
        input_regex = r'(input.*?)output|\)|(input.*)'
        inputs = self.__find_all(content, "input", input_regex)
        self.elements['inputs'] = inputs
        return inputs

    def find_outputs(self):

        self.extract_info()
        content = self.content
        output_regex = r'(output.*?)input|\)|(output.*)'
        outputs = self.__find_all(content, "output", output_regex)
        self.elements['outputs'] = outputs
        return outputs

    def vector_signals(self):
        self.find_inputs()
        inputs = self.elements["inputs"]


        inputs = (list(filter(lambda x: x[0] != "clk", inputs)))
        inputs = (list(filter(lambda x: x[0] != "clock", inputs)))
        inputs = (list(filter(lambda x: x[0] != "rst", inputs)))
        inputs = (list(filter(lambda x: x[0] != "reset", inputs)))

        user_format = int(input("Elija el formato de los números (1 decimal), (2 binario), (3 hexadecimal): "))

        inputs_vector = []
        for j in inputs:

            if j[1] != ():
                #print(j[1])

                result_search_aux = re.search("\[\s*([\d]*)\s*\W*(\d*)", j[1])
                element_1 = int(result_search_aux.group(1))
                element_2 = int(result_search_aux.group(2))

                bus_width = ((element_1 - element_2) + 1)
                bus_range = 2 ** bus_width
                random_number = random.randint(0, (bus_range - 1))

                if user_format == 1:
                    prefix = "'d"

                elif user_format == 2:
                    random_number = bin(random_number).replace('0b', '')
                    prefix = "'b"
                elif user_format == 3:
                    random_number = hex(random_number).replace('0x', '')
                    prefix = "'h"

                dec_string = f"{j[0]}_tb = {bus_width}{prefix}{random_number}"
                inputs_vector.append(dec_string)

            else:

                random_number = random.randint(0, 1)

                if user_format == 1:
                    prefix = "'d"

                elif user_format == 2:
                    random_number = bin(random_number).replace('0b', '')
                    prefix = "'b"
                elif user_format == 3:
                    random_number = hex(random_number).replace('0x', '')
                    prefix = "'h"

                # Decimal
                dec_string = f"{j[0]}_tb = 1{prefix}{random_number}"
                inputs_vector.append(dec_string)

        #print(inputs_vector)
        return (inputs_vector)

    def clock_signal(self):
        # self.find_inputs()
        clock = self.elements["inputs"]

        clock = (list(filter(lambda x: x[0] == "clock", clock)))
        if not clock:
            clock = self.elements["inputs"]
            clock = (list(filter(lambda x: x[0] == "clk", clock)))
            clock = clock[0][0]
        else:
            clock = clock[0][0]

        self.elements['clock'] = clock

        return clock

    def reset_signal(self):
        # self.find_inputs()
        reset = self.elements["inputs"]
        #print(self.elements['inputs'])

        reset = (list(filter(lambda x: x[0] == "reset", reset)))

        if not reset:
            reset = self.elements["inputs"]
            reset = (list(filter(lambda x: x[0] == "reset", reset)))
            reset = reset[0][0]
        else:
            reset = reset[0][0]

        self.elements['reset'] = reset

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
        f = open(new_name, "w")

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
                last_element = self.elements[key][flag - 1]

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

        f.write(connect_ports + "\n")
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

        print(f"Your testbench: {new_name} hass been created")


if __name__ == "__main__":

    files = [
         "regex_breaker_rev_FPM.sv"
    ]

    for file in files:
        creator = TestbenchCreator(file)

        creator.tb_create()
