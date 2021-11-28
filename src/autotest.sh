#!/bin/bash

echo "Correct inputs:"
for file in ../tests/correct/*
do
    ./proj < $file
    if [ $? -ne 0 ]; then
        echo "$file failed!"
    fi
done

echo "Incorrect inputs:"
for file in ../tests/incorrect/*
do
    ./proj < $file
    if [ $? -eq 0 ]; then
        echo "$file failed!"
    fi
done
