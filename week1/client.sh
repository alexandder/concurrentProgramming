#!/bin/bash

echo "Podaj liczbe: "

read liczba

echo $liczba > input.txt

while [[ true ]]; do
	if [[ -f output.txt ]]; then
		read wynik <output.txt
		echo $wynik
		rm output.txt
		break		
	fi
done