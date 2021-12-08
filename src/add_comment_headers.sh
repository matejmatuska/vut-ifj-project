#!/bin/bash

TEMPLATE="../comment_header_template.txt"

for file in .
do
    cat $TEMPLATE $file > $file
done
