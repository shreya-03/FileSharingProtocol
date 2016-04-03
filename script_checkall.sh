#!/bin/bash

var="/home/manasa/server_files"

for f in `ls "$var"`
do
	md5sum "$var"/"$f"
	ls -l "$var"/"$f" | awk '{print $8}' 
done
