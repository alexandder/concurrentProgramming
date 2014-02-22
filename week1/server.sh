#!/bin/bash

while [[ true ]]; do
	if [[ -f input.txt ]]; then
		read liczba <input.txt
		rm input.txt
		echo $(($liczba*$liczba)) > output.txt
	fi
done