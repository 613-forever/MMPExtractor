#!/usr/bin/sh

GIM16=MMPExtractor_GIM16.exe

IFS=$'\n'
for LINE in $(cat error_files.txt); do
	PNG_TARGET_TMP=${LINE/\//_png\/}
	PNG_TARGET_TMP=${PNG_TARGET_TMP%.*}.png
	#PNG_TARGET_TMP_BAK=${PNG_TARGET_TMP}.bak
	#mv "$PNG_TARGET_TMP" "$PNG_TARGET_TMP_BAK"
	echo "Parsing $LINE into $PNG_TARGET_TMP"
	$GIM16 "${LINE}" "$PNG_TARGET_TMP"
	if [[ $? != 0 ]]; then
		echo "$LINE" >> error_files2.txt
	else
		echo "$PNG_TARGET_TMP" >> changed_list.txt
	fi
done

