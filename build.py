#!/usr/bin/python
from os import listdir,system
import os
import sys
cFile = []
hFile = []
for i in listdir('src'):
    if 'c' in i.split('.')[-1]:
        cFile.append(i)
    elif 'h' == i.split('.')[-1]:
        hFile.append(i)
os.chdir('src')
#system('arm-openwrt-linux-gcc -o tingSLAM ' + ' '.join(cFile) +' ' + ' '.join(sys.argv[1:]))
system('g++ -o tingSLAM ' + ' '.join(cFile) +' ' + ' '.join(sys.argv[1:]))
system('mv tingSLAM ..')
