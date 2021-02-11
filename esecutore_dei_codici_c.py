#import matplotlib as pl
#import pandas as pd
import subprocess
from subprocess import Popen
from subprocess import check_output
from subprocess import PIPE

#out = check_output(
#    ["da_chiamare_con_python.exe"],
#    text = True, 
#    )

#print(out)

out = check_output(
    ["lcs_base.exe", "dataset/stringa_lcs_1.txt", "dataset/stringa_lcs_2.txt"],
    text = True, 
    )
print(out)



