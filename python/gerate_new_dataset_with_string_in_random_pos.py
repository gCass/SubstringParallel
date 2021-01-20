import random
import time

def replacer(s, newstring, index, nofail=False):
    # raise an error if index is outside of the string
    if not nofail and index not in range(len(s)):
        raise ValueError("index outside given string")

    # if not erroring, but the index is still not in the correct range..
    if index < 0:  # add it to the beginning
        return newstring + s
    if index > len(s):  # add it to the end
        return s + newstring

    # insert the new string between "slices" of the original
    return s[:index] + newstring + s[(index + len(newstring)):]


path = "../dataset/dataset1.txt"
fread = open(path,"r")
full_txt = ""

start_time = time.time()
#Read whole file
while True:
    txt = fread.read(1000)
    if not txt:
        break
    full_txt = full_txt + txt
fread.close()
end_time = time.time()
print("Time to read file "+str(end_time-start_time))
print(full_txt)
#exit()


#generate some random positions
stringToPut = "CAGATA"
l = len(stringToPut)
numberOfRandomIndex = 15
listOfRandomIndex = list()
start_time = time.time()
i = 0
while i < numberOfRandomIndex:
    #Il range deve fare in modo che l'ultimo valore possible sia abbastanza
    #indietro da poter mettere la stringa nuova
    indx = random.randint(0,len(full_txt)-l)

    #Check if this indx overrides another random insertion
    #If the new index is less distant than the new string length to insert from
    #previous, then redo this iteration since this new indx
    #will override a previous insertion
    flag = True;

    for previousIndx in listOfRandomIndex:
        if(abs(previousIndx - indx) < l):
            flag = False
            #print("False")
            break

    if(flag):
        full_txt = replacer(full_txt, stringToPut, indx, nofail=False)
        listOfRandomIndex.append(indx)
    else:
        i = i - 1

    i = i + 1
    #print(i)

end_time = time.time()
print("Time to insert strings "+str(end_time-start_time))
print(full_txt)

path = "../dataset/random_inserted/inserted_dataset1.txt"
fw = open(path,"w")
fw.write(full_txt)
fw.close()

#Now create a file containing positions in which we inserted the word
path = "../dataset/random_inserted/position_inserted_dataset1.txt"
fw = open(path,"w")
listOfRandomIndex.sort()
for element in listOfRandomIndex:
    fw.write(str(element)+"\n")
fw.close()

