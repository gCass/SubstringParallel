path = "../dataset/cog_2020-04-24_alignment.fasta"
fread = open(path,"r")
for i in range(1,10):
    txt = fread.read(500)
    print(txt)
fread.close()
    


