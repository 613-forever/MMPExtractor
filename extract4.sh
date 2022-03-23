#!/usr/bin/sh

FFMPEG=ffmpeg
AHX2WAV=ahx2wav.exe

$AHX2WAV -b Voice/

FILES=$(ls Voice)

function handle_dir {
  if [[ -d Voice/$1 ]] ; then
    mkdir voice_wav/$1
  fi
  INNER_FILES=$(ls Voice/$1)
  for INNER_FILE in $INNER_FILES; do
    if [[ -d Voice/$1/$INNER_FILE ]] ; then
      handle_dir "$1/$INNER_FILE"
    else
      case "${INNER_FILE##*.}" in
        "wav")
          mv "Voice/$1/$INNER_FILE" "voice_wav/$1/$INNER_FILE"
          $FFMPEG -y -hide_banner -i "voice_wav/$1/$INNER_FILE" -c:a libvorbis "voice_ogg/$1/${INNER_FILE%.*}.ogg"
          ;;
        "adx")
          $FFMPEG -y -hide_banner -i "Voice/$1/$INNER_FILE" -c:a libvorbis "voice_ogg/$1/${INNER_FILE}.ogg"
          ;;
      esac
    fi
  done
}

for FILE in $FILES; do
  handle_dir "$FILE"
done
