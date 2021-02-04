import random

def numToLetter(n):
    if n==0:
        return 'A'
    if n==1:
        return 'C'
    if n==2:
        return 'G'
    else:
        return 'T'


random.seed(a=989, version=2)
##n=random.randint(0,3) #Genera un numero casuale tra 0 e 3
##print(n)

file = open("../dataset/stringa_lcs_4.txt", "wb")
for i in range(10000):
    n=random.randint(0,3) #Genera un numero casuale tra 0 e 3
    c=numToLetter(n)    
    asci = c.encode('ascii')
    file.write(asci)


file.close()
