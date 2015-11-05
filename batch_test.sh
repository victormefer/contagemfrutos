#!/bin/bash

make release;

# teste sem cortar blobs e sem dist transf
# ./segmenter 6 treinos/pessego_10im.xml 0 10 0 0
# teste sem cortar blobs e com dist transf
# ./segmenter 6 treinos/pessego_10im.xml 0 10 0 1


# echo "================== ACEROLA ===================" >> saida.log;

# for i in 0 1 # dist transf ou nao
# do
# 	for j in 0 1 2 # canal de corte
# 	do
# 		for k in 10 20 30 40 50 60 70 80 90 100 # threshold de corte
# 		do
# 			./segmenter 1 treinos/acerola_01.xml $j $k 1 $i
# 		done
# 	done
# done

# echo "================== LARANJA ===================" >> saida.log;

# for i in 0 1 # dist transf ou nao
# do
# 	for j in 0 1 2 # canal de corte
# 	do
# 		for k in 10 20 30 40 50 60 70 80 90 100 # threshold de corte
# 		do
# 			./segmenter 2 treinos/laranja_33im.xml $j $k 1 $i
# 		done
# 	done
# done

# echo "================== MORANGO ===================" >> saida.log;

# for i in 0 1 # dist transf ou nao
# do
# 	for j in 0 1 2 # canal de corte
# 	do
# 		for k in 10 20 30 40 50 60 70 80 90 100 # threshold de corte
# 		do
# 			./segmenter 3 treinos/morango_34im.xml $j $k 1 $i
# 		done
# 	done
# done

# echo "================== PESSEGO ===================" >> saida.log;

# for i in 0 1 # dist transf ou nao
# do
# 	for j in 0 1 2 # canal de corte
# 	do
# 		for k in 10 20 30 40 50 60 70 80 90 100 # threshold de corte
# 		do
# 			./segmenter 2 treinos/laranja_33im.xml $j $k 1 $i
# 		done
# 	done
# done

# echo "================== CEREJA ===================" >> saida.log;

# for i in 0 1 # dist transf ou nao
# do
# 	for j in 0 1 2 # canal de corte
# 	do
# 		for k in 10 20 30 40 50 60 70 80 90 100 # threshold de corte
# 		do
# 			./segmenter 7 treinos/cereja_15im.xml $j $k 1 $i
# 		done
# 	done
# done

echo "================== CAJU ===================" >> saida.log;

for i in 0 1 # dist transf ou nao
do
	for j in 0 1 2 # canal de corte
	do
		for k in 10 20 30 40 50 60 70 80 90 100 # threshold de corte
		do
			./segmenter 8 47 treinos/caju_46im.xml $j $k 1 $i
		done
	done
done

echo "================== AMEIXA ===================" >> saida.log;

for i in 0 1 # dist transf ou nao
do
	for j in 0 1 2 # canal de corte
	do
		for k in 10 20 30 40 50 60 70 80 90 100 # threshold de corte
		do
			./segmenter 9 43 treinos/ameixa_42im.xml $j $k 1 $i
		done
	done
done

# echo "================== MANGA ===================" >> saida.log;

# for i in 0 1 # dist transf ou nao
# do
# 	for j in 0 1 2 # canal de corte
# 	do
# 		for k in 10 20 30 40 50 60 70 80 90 100 # threshold de corte
# 		do
# 			./segmenter 5 treinos/manga_11im.xml $j $k 1 $i
# 		done
# 	done
# done