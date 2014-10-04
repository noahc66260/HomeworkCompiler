#!/bin/bash

# This script will move files with the extension .tex
# in the working directory to a predefined directory according
# to the user's command line input

realAnalysisFolder="/home/noah/Documents/MATH_136_Real_Analysis_II"
complexAnalysisFolder="/home/noah/Documents/MATH_158_Complex_Analysis"
electromagneticsFolder="/home/noah/Documents/EE_135_Advanced_Electromagnetics"
numberTheoryFolder="/home/noah/Documents/MATH_115_Number_Theory"

if [ -z $1 ]
then
	echo usage: bash moveTexFiles keyword
	exit
fi

if [ "real" == "$1" ]
then
	destination=$realAnalysisFolder
fi

if [ "complex" == "$1" ]
then
	destination=$complexAnalysisFolder
fi

if [ "electro" == "$1" ]
then
	destination=$electromagneticsFolder
fi

if [ "number" == "$1" ]
then
	destination=$numberTheoryFolder
fi


#echo "destination = $destination"
mv -i *.tex $destination
