import json

class FsmGenerator():

    def __init__(self, file):

        with open(file) as json_file:
            self.data = json.load(json_file)

    def verify(self):
        key_words = [["format" , "data"] , ["actual_state", "next_state" , "inputs" , "outputs"]]

        flag_l1 = 0
        flag_l2 = 0

        for key in self.data.keys():
            if key in key_words[0]:
                flag_l1 += 1


        if flag_l1 == len(self.data):
            for file, element in enumerate (self.data["data"]):
                for key in self.data["data"][file].keys():
                    if key in key_words[1]:
                        flag_l2 += 1


        if flag_l2 % len(self.data["data"]) == 0:
            print("Verified file")
            return True

        else:
            print("Warning!!!! Wrong format")
            return False

    #def state_treatment(self):



if __name__ == "__main__":

    gen = FsmGenerator("data.json")
    gen.verify()

