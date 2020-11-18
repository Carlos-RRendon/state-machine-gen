import json


options = ["bin" , "hex", "dec"]

#--------+-------------+--------+---------+
#  State |  Next_State | Inputs | Outputs |
#--------+-------------+--------+---------+


'''dict = {
"format" : options[0] ,
"data":
    [
        {
            "actual_state" :"s0",
            "next_state" :"S2",
            "inputs" : [["A",0b00001111],["B",15],["C",0xf]],
            "outputs" : [["x",0b10010100] , ["y" , 20], ["z", 0xff]]
        },

        {
            "actual_state" :"S1",
            "next_state" :"S2",
            "inputs" : [["A",0b00001111],["B",15],["C",0xf]],
            "outputs" : [["x",0b10010100] , ["y" , 20], ["z", 0xff]]
        }
    ]
}
state_name = "s2"

data2 = {

    "actual_state" : {
        state_name:
            [
                {
                    "next_state" : "s0",
                    "inputs" : [["A",0b00001111],["B",15],["C",0xf]],
                    "outputs" : [["x",0b10010100] , ["y" , 20], ["z", 0xff]]
                },
                {
                    "next_state" : "s3",
                    "inputs" : [["A",0b11111],["B",15],["C",0xf]],
                    "outputs" : [["x",0b10010100] , ["y" , 20], ["z", 0xff]]
                }

            ]
    }

}
'''

dict = {
"format" : options[0],
"data":
    [
        {
            "actual_state" :"s0",
            "next_state" :"s1",
            "inputs" : [["A",'x'],["B","x"]],
            "outputs" : [["x",0b01] , ["y" , 0b00]]
        },

        {
            "actual_state": "s1",
            "next_state": "s2",
            "inputs": [["A", 'x'], ["B", 'x']],
            "outputs": [["x", 0b00], ["y", 0b1]]
        },


           {
            "actual_state": "s2",
            "next_state": "s3",
            "inputs": [["A", 0b11], ["B", 'x']],
            "outputs": [["x", 0b11], ["y", 0b1]]
        },
{
            "actual_state": "s2",
            "next_state": "s4",
            "inputs": [["A", 0b11], ["B", 'x']],
            "outputs": [["x", 0b11], ["y", 0b1]]
        }

    ]
}
if __name__ == "__main__" :
    

    json_data = json.dumps(dict , indent = 1)

    print(json_data)

    jsonFile = 'data.json'
    # Open a json file for writing json data
    with open(jsonFile, 'w') as fileHandler1:
      json.dump(dict, fileHandler1, indent = 2)
