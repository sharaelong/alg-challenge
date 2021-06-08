#!/bin/bash

datas=("lcc_human" "lcc_yeast")
suffix=("_n3" "_n5" "_n8" "_s3" "_s5" "_s8")

for data in "${datas[@]}"
do
    for kind in "${suffix[@]}"
    do
        printf "$data$kind: "
        printf "$((`gtimeout $2 ../src/$1 "../data/${data}.igraph" "../query/${data}${kind}.igraph" "../candidate_set/${data}${kind}.cs" | wc -l`))\n"
    done
done
