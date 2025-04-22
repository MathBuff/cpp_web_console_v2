#!/bin/bash

# Check if a parameter was provided
if [ -z "$1" ]; then
    echo "Error: Please provide a string parameter for the repo to sshlink\n assuming that ssh is set up"
    exit 1
fi

# Store the string parameter
input_string="$1"

# Example operations with the string
echo "Received: $input_string"

git init
git add .
git commit -m "initial commit"

git remote add origin "$input_string"
git branch -M main
git push -u origin main
