#Bash script to create a csv file with all files names 
ls $1* | tr [:space:] ',' | sed 's/.$//' >> images-train.txt
