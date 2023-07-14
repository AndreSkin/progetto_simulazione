#!/bin/bash

# Imposta il percorso della cartella principale
path="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"

# Esegui il comando in ogni sottocartella
for dir in "$path"/*/
do
    cd "$dir"
    opp_scavetool.exe x *.sca -o scalars.csv
    opp_scavetool.exe x *.vec -o vectors.csv

    # Cancella tutti i file ad eccezione dei file .csv
    find . -type f ! -name '*.csv' -delete
done
