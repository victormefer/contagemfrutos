#!/bin/bash

make release;

# teste sem cortar blobs e sem dist transf
./segmenter 2 treinos/laranja_01.xml 0 10 0 0
# teste sem cortar blobs e com dist transf
./segmenter 2 treinos/laranja_01.xml 0 10 0 1


for i in 0 1 # dist transf ou nao
do
	for j in 0 1 2 # canal de corte
	do
		for k in 10 15 20 25 30 35 40 # threshold de corte
		do
			./segmenter 2 treinos/laranja_01.xml $j $k 1 $i
		done
	done
done