#!/bin/bash


FILENAME=$1
DIR=$2
CURRENT_PID=$$

if [[ -z "${FATHERS_PID+x}" ]]; then
	export FATHERS_PID=$$
fi

OUTPUTFILENAME="szukajwynik$FATHERS_PID"

if [[ -z "${FATHERS_DIR+x}" ]]; then
	export FATHERS_DIR=$DIR
fi

if [[ -f "$DIR$FILENAME" ]]; then
	echo "$DIR$FILENAME"
	if [[ ! -f "$FATHERS_DIR$OUTPUTFILENAME" ]]; then
		echo "" > "$FATHERS_DIR$OUTPUTFILENAME"
	fi
fi

for dir in $2*;
do
	if [[ -d "$dir" && ! -L "$dir" ]]; then
	 	./szukaj.sh $FILENAME $dir/ &
	fi
done

wait

if [[ "$FATHERS_PID" == "$CURRENT_PID" ]]; then
	if [[ ! -f "$FATHERS_DIR$OUTPUTFILENAME" ]]; then
		echo "Nie znaleziono pliku $FILENAME"
	else
		rm "$FATHERS_DIR$OUTPUTFILENAME"
	fi
fi

exit 0