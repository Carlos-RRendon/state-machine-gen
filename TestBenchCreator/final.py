import re
import random

def toBin(dimension):                           #Function to generate binary number for the testbench
    num = random.randint(0,(2**dimension)-1)    #Random number based on the maximum number of possible combinations
    numBin=bin(num)                             #Transforn the random number to binary
    numBin =numBin[2:]                          #Eliminate the 0b in the given string
    zeros=("0"*(dimension-len(numBin)))         #Fill with zeros if the number is smaller than the maximum dimension
    return(zeros+numBin)                        #Return the binary number with a specific dimension

#Starts file reading (.sv)
punc2= "(input|output|inout|module)"            #Punctuation to find reserved wors
punc= '\t\n  '                                  #Punctuation to find \t and \n
slash= "\\/+\\/+.*"
multi= "\\/+\\*+.*\\*\\/+"
text = ""                                       #Initialize the variable to save the read file
archivo = open("hello.sv", "r")                 #Open the file.sv
for linea in archivo:                           #For loop to read all the lines in the file
    comment = re.split(slash,linea)             #Find normal comments
    linea = comment[0]                          #Erase normal comments
    k2 = re.findall(punc2, linea)               #Find the reserved words
    if(len(k2)>0):                              #If a punctuation was found
        text += linea                           #Add the found data to text
archivo.close                                   #Close file
for linea in text:                              #For loop to read all the lines
    if linea in punc:                           #If there is a \t or \n
        text=text.replace(linea,' ')            #Replace it with a space
text=re.sub(multi,"",text)                      #Erase multiline comments
print(text)
#Ends file reading (.sv)

#Starts regex for variables
mod_pat = "module\\s+([a-zA-Z]\\w*)"            #Pattern to find the module name given by Lenin
var_pat = "(input|output|inout)"                #Pattern with the type of variables
inout_pat = "\\b(?!endmodule\\b)\\b(?!reg\\b)(?![b,d,h]+)[a-zA-Z]+\\w*"#Pattern to detect extract the variable names
bra_pat = "\\[\\s*((\\d+)\\s*:\\s*(\\d+))\\s*\]\\s*"#Pattern to extract the digits from brackets
in_pat = "\\W*((input|output|inout)\\s*(logic|reg|wire)*\\s*(\\[\\d+:\\d+\\]\\s*|\\s+)([_a-zA-Z]\\s*\\w*(\\s*,\\s*[_a-zA-Z]\\w*)*)\\s*[,|)|;])"
str_tb = "_TB"                                  #Pattern with _TB to add in the testbench
tem_list= list()                                #Create an empty list to save the values of a row
tb = dict()                                     #Create a dictionary for the module
mod_tem = re.findall(mod_pat,text)              #Save the module name
tb["module"] = [''.join(mod_tem)]               #Assign the name of the module to the dictionary
tb["input"] = []                                #Create an empty field for the inputs
tb["output"] = []                               #Create an empty field for the outputs
tb["inout"] = []                                #Create an empty field for the inouts
var_reg = re.split(var_pat, text)               #Split the text with the type of variables
for index, var in enumerate(var_reg):           #For loop to read the entries in the created vector
    find= re.findall(in_pat, text)
    #print(find)
    if (index % 2) == 0 and index > 0:          #If the index is an odd number
        tem_reg = re.findall(inout_pat,var)     #Find all the varible names
        ind_reg = re.findall(bra_pat,var)       #Find all the indexes
        for in_tem in tem_reg:                  #For loop to read all the found variables
            tem_list.append(var_reg[index-1])   #Append the type of variable in a temporal list
            tem_list.append(in_tem)             #Append the variable name in a temporal list
            if len(ind_reg) > 0:                #If indexes were found in the module
                tem_list.append(int(ind_reg[0][1]))#Append the first index in a temporal list
                tem_list.append(int(ind_reg[0][2]))#Append the second index in a temporal list
            else:                               #If indexes were not found
                tem_list.append(0)              #Append a 0 to the first index
                tem_list.append(0)              #Append a 0 to the second index
            tb[tem_list[0]].append(tem_list)    #Append all the temporal list to an entry in the dictionary
            tem_list=list()                     #Clean the list

print("Module name: "+tb["module"][0])   #Print the module name
for i,j in enumerate(tb["input"]):       #Print the information of each input
    print("Input "+str(i+1)+":"+str(j))
for i,j in enumerate(tb["output"]):      #Print the information of each output
    print("Output "+str(i+1)+":"+str(j))
for i,j in enumerate(tb["inout"]):       #Print the information of each inout
    print("Inout "+str(i+1)+":"+str(j))
#Ends regex for variables

#Stars testbench generation
clk_rst = "(\\s*\\w*[cC]{1}[lL]{1}[oO]{0,1}[cC]{0,1}[kK]{1}[0-9]*\\s*)|(\\s*\\w*[rR]{1}[eE]{0,1}[sS]{1}[eE]{0,1}[tT]{1}[0-9]*\\s*)"
flag_clk = 2                                         #Flag to detect if there is a clock
flag_rst = 2
name_clk = ""
name_rst = ""
len_var = len(tb["inout"])+len(tb["output"])+len(tb["input"])#Length of the variables
dimension = 0                                       #Initialize dimension variable
f = open("testbench.sv",'w')                        #Create or open the testbench file
f.write("`timescale 1ns / 1ps\n")                   #Print the timescale
f.write("module "+tb["module"][0]+str_tb+";\n")     #Print the module name
for val in tb["input"]:                             #Print the information of each input
    if val[2]==val[3]:
        f.write("reg "+val[1]+str_tb+";\n")
        dimension = dimension+1                     #Variable dimension is equal to dimension + 1
    else:
        f.write("reg ["+str(val[2])+":"+str(val[3])+"] "+val[1]+str_tb+";\n")
        dimension = dimension+max(val[2],val[3])+1  #Add to the dimension variable the maximum of the indexes + 1
    fd_rc= re.search(clk_rst,val[1])
    if fd_rc:                                       #I changed it                                          
        print("We have detected a signal called {}".format(val[1]))
        if(fd_rc.group(1)):                              #Clock detection
            while flag_clk == 2:
                in_clk = input("Do you want that the values of this signal are treated as a clock y/n? ")
                if in_clk == ("y" or "Y"):
                    flag_clk=1
                    name_clk=val[1]
                elif in_clk == ("n" or "N"):
                    flag_clk=0
            continue
        if(fd_rc.group(2)):                           #Reset detection
            #print(fd_rc[1])
            while flag_rst == 2:
                in_rst = input("Do you want this signal to be treated as a reset y/n? ")
                if in_rst == ("y" or "Y"):
                    flag_rst=1
                    name_rst=val[1]
                elif in_rst == ("n" or "N"):
                    flag_rst=0
    #REGEX RST
for val in tb["output"]:       #Print the information of each input
    if val[2]==val[3]:
        f.write("wire "+val[1]+str_tb+";\n")
    else:
        f.write("wire ["+str(val[2])+":"+str(val[3])+"] "+val[1]+str_tb+";\n")
#Interconnections
f.write(tb["module"][0]+" UUT(")                        #Start the interconections between signals
for val in tb["input"]:       #Print the information of each input
    f.write("."+val[1]+"("+val[1]+str_tb+"),")#Print the interconections with a ), at the end 
for ind,val in enumerate(tb["output"]):       #Print the information of each input
    if ind == len(tb["output"])-1:                          #If it is the last input/output
        f.write("."+val[1]+"("+val[1]+str_tb+"));\n")#Print the interconection with a )); at the end
    else:                                           #If it is not the last input/output
        f.write("."+val[1]+"("+val[1]+str_tb+"),")#Print the interconections with a ), at the end 

f.write("initial\n")                                #Start the declaration of the inputs
f.write("begin\n")                                  
f.write('$dumpfile("'+tb["module"][0]+str_tb+'.vcd");\n')#Create the dumpfile for simulation
f.write("$dumpvars(1, "+tb["module"][0]+str_tb+");\n")  #Load the variables for the simulation
#Initialize
for val in tb["input"]:       #Print the information of each input
    if val[2]==val[3]:
        if val[1] == name_rst:
            f.write(val[1]+str_tb+"=1'b1;\n")    #Initialize the variable at 0
        else:
            f.write(val[1]+str_tb+"=1'b0;\n")    #Initialize the variable at 0
    else:
        mayor = max(val[2],val[3])#Get the bigger index
        f.write(val[1]+str_tb+"="+str(mayor+1)+"'b0;\n")#Initialize the variable at 0 with the right dimension
#Simulating cycles
if flag_rst == 2:
    flag_rst = 0
if flag_clk == 2:
    flag_clk = 0

for val in tb["input"]:       #Print the information of each input
    if flag_rst==1:
        if val[1] == name_rst:
            f.write("#1\n")
            f.write(val[1]+str_tb+"=1'b0;\n")    #Initialize the variable at 0

if (flag_clk and flag_rst and len(tb["input"])==2) or ((flag_clk or flag_rst) and len(tb["input"])==1):
    print("All the inputs have been detected as special")
    test_time = int(input("How long the simulation will last? (s): "))
    f.write("#"+str(test_time)+"\n")
else:
    chk_cycle = 0
    while chk_cycle == 0:
        cycles = input("How many cycles do you want to simulate? ")
        try:
            cycles=int(cycles)
            chk_cycle = 1
            break
        except ValueError:
            chk_cycle = chk_cycle
    for i in range(1,cycles+1):                               #Creation of multiple random scenarios for the inputs
        estimulo = toBin(dimension)                     #save the random numbers in binary
        ini = 0                                         #Initialize the temporal indexes at 0
        fin = 0
        f.write("#1\n")                                 #Write #1 for the timescale
        for val in tb["input"]:                         #Print the information of each input
            if val[2]==val[3]:
                fin = ini+1                             #Update the final index
                if val[1] != name_clk and val[1] != name_rst:                       #If the input name is different than clk
                    f.write(val[1]+str_tb+"=1'b"+estimulo[ini:fin]+";\n")
                ini = ini+1                             #Update inicial index with ini+1
            else:
                mayor = max(val[2],val[3])              #Get the bigger index
                fin=ini + mayor                         #Update the end of the string with the maximum index number
                f.write(val[1]+str_tb+"="+str(mayor+1)+"'b"+estimulo[ini:fin+1]+";\n")
                ini=fin+1                               #Update the final index with the fin+1

f.write("$finish;\n")                               #Finalize the simulation
f.write("end\n")    
if flag_clk:                                        #If there was a clk detected
    f.write("always forever #1 clk"+str_tb+" = ~clk"+str_tb+";\n")#Print always function with th clk
f.write("endmodule\n")                              #End the module
f.close                                             #Close the testbench file
print("The testbench has been created successfully")
#Ends testbench generation