#! /usr/bin/sh

QUICKBMS=quickbms.exe
GIM2PNG=GIM2PNG.EXE
GIMFROMCSB=MMPExtractor_GIMFromCSB.exe

ROOT_DIR=$1
PNG_DIR=$1_png
mkdir $PNG_DIR

function handle_pac {
	TARGET=$ROOT_DIR/$1.extracted
	if [[ ! -d "$TARGET" && "$1" != "./Scene_Dungeon/script.pac" ]] ; then
		echo "extracting $1"
		mkdir -p "$TARGET"
		$QUICKBMS madoka_pac_extractor.txt "$ROOT_DIR/$1" "$TARGET/"
	fi
	handle_dir "$1.extracted"
}

function handle_csb {
	TARGET=$ROOT_DIR/$1.extracted
	if [[ ! -d "$TARGET" ]] ; then
		echo "extracting $1"
		mkdir -p "$TARGET"
		$GIMFROMCSB "$ROOT_DIR/$1" "$TARGET/"
	fi
	handle_dir "$1.extracted"
}

function handle_dir {
	echo "handling dir $ROOT_DIR/$1"
	for FILE in $(ls "$ROOT_DIR/$1"); do
		if [[ -d "$ROOT_DIR/$1/$FILE" ]] ; then
			echo "entering dir $ROOT_DIR/$1/$FILE"
			handle_dir "$1/$FILE"
			echo "exiting dir $ROOT_DIR/$1/$FILE"
		else
			case "${FILE##*.}" in
				"csb" | "CSB")
					echo "handling csb file $ROOT_DIR/$1/$FILE"
					handle_csb "$1/$FILE"
					;;
				"pac" | "PAC")
					echo "handling pac file $ROOT_DIR/$1/$FILE"
					handle_pac "$1/$FILE"
					;;
				"gim" | "GIM")
					echo "handling gim file $ROOT_DIR/$1/$FILE"
					GIM_SOURCE=$ROOT_DIR/$1/$FILE
					PNG_TARGET=$1/${FILE%.*}.png
					if [[ ! -d "$PNG_DIR/$1" ]]; then
						mkdir -p "$PNG_DIR/$1/"
					fi
					if [[ ! -e "$PNG_DIR/$PNG_TARGET" ]]; then
						$GIM2PNG -9 "$GIM_SOURCE"
						mv "$ROOT_DIR/$PNG_TARGET" "$PNG_DIR/$PNG_TARGET"
						if [[ $? != 0 ]]; then
							echo "$GIM_SOURCE" >> error_files.txt
						fi
					fi
					;;
				*)
					;;
			esac

		fi
	done
}

handle_dir .

