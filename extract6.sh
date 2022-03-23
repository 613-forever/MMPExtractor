#!/usr/bin/sh

FFMPEG=ffmpeg

FILES=$(ls Bgm)

for FILE in $FILES; do
  if [[ ${FILE##*.} -eq "adx" ]]; then
    $FFMPEG -y -hide_banner -i "Bgm/$FILE" -vn -acodec libvorbis "Bgm_ogg/${FILE}.ogg"
  fi
done

