#!/bin/bash

# Read input from standard input and echo it to the terminal
while IFS= read -r line; do
    echo "$line"
done