#!/bin/bash

datas=("lcc_hprd" "lcc_human" "lcc_yeast")
suffix=("_n1" "_n3" "_n5" "_n8" "_s1" "_s3" "_s5" "_s8")

for data in "${datas[@]}"
do
    for kind in "${suffix[@]}"
    do
        printf "$data$kind \n"
        printf "$((`timeout 60 ../program "../data/${data}.igraph" "../query/${data}${kind}.igraph" "../candidate_set/${data}${kind}.cs" | wc -l` - 1))\n"
        echo "---------------"
    done
done
