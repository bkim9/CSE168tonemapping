#!/bin/bash

for f in ./ori/*; do
    #echo $f
    sf="./out/out${f##*/}"
    echo ./logmap -i ${f} -o ${sf} [options]
done