#!/bin/bash


var1=$1
var2=$2
var3=$3
var4=$4

#echo $var3
#ls -l "/home/manasa/client_files" | awk '{print $8}' | awk -v v1="$var1" -v v2="$var2" -v v3="$var3" -v v4="$var4" -F: '{if($1>=v1&&$1<=v2&&$2>=v3&&$2<=v4){print $0}}'  

a1=0
a2=0
a3=0
a4=0
a5=0
a6=0
counter=0
row=0
flag=0
r1=0
r2=0
for file in `ls -l "/home/manasa/server_files"`
do
	row=$((row+1))
	if [ $row -gt 2 ]
	then
	counter=$((counter+1))	
	
		if [ $((counter%9)) -eq 1 ]
		then
			a1=$file	 
	 

		elif [ $((counter%9)) -eq 5 ]
		then
			a2=$file	 
	 
	 
		elif [ $((counter%9)) -eq 8 ]
		then
			r1=`echo $file | awk -F: '{print $1}'`
			r2=`echo $file | awk -F: '{print $2}'`
			a4=$file	 

		elif [ $((counter%9)) -eq 0 ]
		then
			a5=$file	 
			if [ $r1 -ge $var1 ] && [ $r1 -le $var2 ]
			then
				if [[ ( $r1 -eq $var1  &&  $r2 -ge $var3 ) || ( $r1 -eq $var2  &&  $r2 -le $var4 ) || ( $r1 -gt $var1 && $r1 -lt $var2 ) ]] 

				then
					printf "%s %s %s %s %s\n" "$a1" "$a2" "$a4" "$a5"
				fi
			fi
		fi 
	fi
done


