#!/usr/bin/sh

FFMPEG=ffmpeg

FILES=$(ls Bgm)

for FILE in $FILES; do
  if [[ ${FILE##*.} -eq "adx" ]]; then
    $FFMPEG -y -hide_banner -i "Bgm/$FILE" -vn -acodec libvorbis "bgm_ogg/${FILE}.ogg"
    # $FFMPEG -y -hide_banner -i "Bgm/$FILE" -acodec flac "bgm_flac/${FILE}.flac"
  fi
done

