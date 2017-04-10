#!/bin.sh

if [ $# -ne 1 ]; then
	printf "%s ttf_file_name\n" $0
	exit 1
fi

newfile=mod_`basename $1`

# check whether apple font tool is installed or not
which -s ftxdumperfuser
if [ $? -ne 0 ]; then
	echo "Apple fonttool is not installed"
	echo ""
	echo "https://developer.apple.com/textfonts/Fonttools/Index.html"

	exit 1
fi

# duplicate font
cp -a $1 $newfile

# dump name table
ftxdumperfuser -t name $newfile > tmp.xml
if [ $? -ne 0 ]; then
	echo "failed to dump name table"
	exit
fi

# update xml file
python fonttool.py tmp.xml 
if [ $? -ne 0 ]; then
	echo "failed to run fonttool.py"
	exit
fi

# apply updated name table
ftxdumperfuser -t name -d tmp.xml $newfile 
if [ $? -ne 0 ]; then
	echo "failed to update name table"
	exit
fi

# remove tmp file
rm tmp.xml
