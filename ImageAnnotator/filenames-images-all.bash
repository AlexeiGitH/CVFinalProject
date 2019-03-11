#Bash script to create a csv file with all files names 
ls Images_all | sed 's/.*/Images_all\/&/g' | tr [:space:] ',' | sed 's/.$//' >> images-all.txt
