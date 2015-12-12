Universidade de Brasilia - UnB
Departamento de Ciencia da Computacao - CIC/UnB
LISA - Laboratorio de Imagens, Sinais e Audio

Sistema de contagem automatica de frutos - Versao 1
Desenvolvido por: Victor Mesquita Ferreira e Gustavo Ribeiro Teixeira
Orientador: Prof. Dr. Flavio de Barros Vidal


No arquivo batch_test.sh, temos as seguintes instrucoes:

---------------------------------------------------------------------------------
#!/bin/bash

make release;			# Compila o programa

echo "================== MORANGO 30 sobre 70 ===================" >> saida.log;

for i in 0 1 					# 0: sem dist transf; 1: com dist transf
do
	for j in 0 1 2 				# canal de corte 0: L, 1: a e 2: b
	do
		for k in 10 20 30 40 50 60 70 80 90 100 				# threshold de corte, variando de 10 a 100
		do
			./segmenter 3 81 treinos/morango_80im.xml $j $k 1 $i			# execucao do programa com passagem dos parametros para a funcao TesteBatch(No. da fruta, imagem de inicio do processamento, caminho do arquivo de treinos, canal de corte, threshold, corte, transformacao de distancia)
		done
	done
done
---------------------------------------------------------------------------------

Para executar o programa em modo batch no terminal, basta digitar a seguinte linha:

$bash batch_test.sh

O codigo programa sera compilado e em seguida os loops farao iteracoes variando os parametros de entrada para execucao do programa. As especificacoes de cada iteracao executada e o resultado da F-measure sao escritas no arquivo saida2.log
