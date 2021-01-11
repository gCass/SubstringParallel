path = "../dataset/dna-1-dal-pdf-corto.txt"
fread = open(path,"r")
txt = fread.read()
#print(txt)
fread.close()

path_pulito = "../dataset/dna_corto_pulito.txt"
fw = open(path_pulito,"w")
fw.write(txt)
fw.close()



