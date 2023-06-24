#!/bin/bash
for f in ./color_ppm/*.ppm; do
    #echo $f
    sf="./out/out${f##*/}"
    # echo $sf
    ./color_bilateral_filter ${f} ${sf} 16.0 0.1
done
