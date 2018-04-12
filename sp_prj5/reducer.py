#!/usr/bin/python3
# -*- coding: utf-8 -*-

from sys import stdin, stdout

dic = {}

while True:
	line = stdin.readline().strip()
	if line == "":
		break
	tmp = line.split('\t')
	key = tmp[0]
	value = tmp[1]
#	print(key,value)

	if key in dic:
		dic[key] += 1
	else:
		dic[key] = 1

for key in sorted(dic.keys()):
	print('%s\t%d'%(key,dic[key]))
