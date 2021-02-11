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

#Codice
df = pd.DataFrame()
t_lett, t_lcs = exec_blocchi()
print(str(t_lett) + " " + str(t_lcs) )
