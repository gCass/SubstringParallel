path = "../dataset/cog_2020-04-24_alignment.fasta"
fread = open(path,"r")
basi_azotate = ["A","T","C", "G"]
cleared_txt = ""
while True:
    txt = fread.read(500)
    if not txt:
        break
    cleared_txt = cleared_txt +  ''.join([i for i in txt if i in basi_azotate])
fread.close()
    
path = "../dataset/cleared_fasta.txt"
fw = open(path,"w")
fw.write(cleared_txt)
fw.close()


