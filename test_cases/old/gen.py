#!/usr/bin/python
import os
import sys
p = sys.argv[2]

fileName = str(sys.argv[1])+".txt"
#print fileName
outFile = open(fileName,"w")
size = int(p)

content = "";
for i in range(0,size):
    content+='a'
print len(content)
outFile.write(content)
outFile.close();
