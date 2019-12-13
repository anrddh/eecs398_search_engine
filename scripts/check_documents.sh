#!/bin/bash

PAGESTORE_FILE=$1

for i in $(seq 0 10000); do
    ALL_FOUND=true
    if [[ ! $(./print_page_store $PAGESTORE_FILE $i | grep " $2\$" ) ]]; then 
        ALL_FOUND=false 
    fi
    if [[ "$ALL_FOUND" == "true" ]]; then
        echo "$(( i+4119 )) "
    fi
done
