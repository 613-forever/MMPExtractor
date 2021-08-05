#! /usr/bin/sh

QUICKBMS=quickbms.exe
GIM2PNG=GIM2PNG.EXE
TACHIEASSEMBLE=MMPExtractor_StandAssemble.exe

FILES=$(ls pac)

mkdir extracted_raw
mkdir extracted_png
mkdir assembled_png

for FILE in $FILES; do
	echo "extracting $FILE"
	NAME=${FILE%.*}
	mkdir extracted_raw/$NAME
	$QUICKBMS madoka_pac_extractor.txt pac/$FILE extracted_raw/$NAME/
	GIMS=$(ls extracted_raw/$NAME | grep GIM$)
	mkdir extracted_png/$NAME/
	mkdir assembled_png/$NAME/
	echo "GIM files extracted from \"$FILE\":"
	echo $GIMS
	for GIM in $GIMS; do
		$GIM2PNG -9 extracted_raw/$NAME/$GIM
	done
	$TACHIEASSEMBLE --scan extracted_raw/$NAME/ assembled_png/$NAME/
	mv extracted_raw/$NAME/*.png extracted_png/$NAME/
done

