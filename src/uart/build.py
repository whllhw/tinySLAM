#!/usr/bin/python
# coding:utf-8

import os
sourceFile = []
for i in os.listdir('.'):
    if os.path.isfile(i) and 'cpp' in i.split('.')[-1]:
        sourceFile.append(i)
        print(i)
os.system('arm-openwrt-linux-g++ -std=c++11 '+' '.join(sourceFile))