f1 = open("dataset2.txt","r")
f2 = open("dataset3.txt","w")

txt=f1.read()

for i in range(10):
    f2.write(txt)

f1.close()
f2.close()
