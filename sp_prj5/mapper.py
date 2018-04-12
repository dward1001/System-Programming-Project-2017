#!/usr/bin/python3
# -*- coding: utf-8 -*-

from sys import stdin, stdout

while True:
	line = stdin.readline().strip()
	if line == "":
		break
	word = line.split()
	for i in range(len(word)-1):
		print('%s,%s\t1'%(word[i].lower(),word[i+1].lower()))
