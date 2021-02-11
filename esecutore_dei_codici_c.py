import matplotlib as pl
import pandas as pd
import subprocess
from subprocess import check_output

def exec_base(path1="dataset/stringa_lcs_1.txt",path2="dataset/stringa_lcs_2.txt"):
    out = check_output(
        ["lcs_base_compilato_a_mano.exe", path1, path2],
        text = True, 
        )
    #print(out)
    l = out.split()
    #print(l)
    t_lett = l[1]
    t_lcs = l[3]
    return t_lett, t_lcs

def exec_blocchi(path1="dataset/stringa_lcs_1.txt",path2="dataset/stringa_lcs_2.txt",block=100):
    out = check_output(
        ["lcs_a_blocchi_parallel_compilato_a_mano.exe", path1, path2, str(block)],
        text = True, 
        )
    #print(out)
    l = out.split()
    #print(l)
    t_lett = l[1]
    t_lcs = l[3]
    return t_lett, t_lcs


#Prove
#t_lett, t_lcs = exec_blocchi()
#print(str(t_lett) + " " + str(t_lcs) )



#Codice vero
#Inizializzazione
df = pd.DataFrame()
block_sizes = [50,100,200,300,400,500]
Ns_thread = [4] 
column_names = ["N_thread","File1","File1_size","File2","File2_size","t_serial_lett","t_serial_lcs","block_size","t_paral_lett","t_paral_lcs"]
df = pd.DataFrame(columns = column_names)
print(df)

#Inizio cicli

file1 = "dataset/stringa_lcs_1.txt"
file1_size = 10000
file2 = "dataset/stringa_lcs_2.txt"
file2_size = 10000
#Ci vorrebbe un ciclo ancora più esterno che fa variare i file, ma quello dopo
for n_thread in Ns_thread:
    #Eseguo il seriale una volta sola dato il numero di thread
    t_lett_serial,t_lcs_serial = exec_base()
    for block in block_sizes:
        t_lett_parallel,t_lcs_parallel = exec_blocchi(block=block)
        df = df.append({"N_thread":n_thread, "File1": file1 ,"File1_size": file1_size ,
                        "File2": file2,"File2_size":file2_size,
                        "t_serial_lett": t_lett_serial,"t_serial_lcs": t_lcs_serial,
                        "block_size":block,"t_paral_lett":t_lett_parallel,"t_paral_lcs":t_lcs_parallel}
                       , ignore_index=True)


from tabulate import tabulate
print(tabulate(df, headers='keys', tablefmt='simple'))
df.to_csv("risultati_cicli.csv", index=False)

        
        





        
