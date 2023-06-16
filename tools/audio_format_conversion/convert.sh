#   Copyright 2023 NXP.
#   NXP Confidential. This software is owned or controlled by NXP and may only be used strictly in accordance
#   with the license terms that accompany it. By expressly accepting such terms or by downloading, installing,
#   activating and/or otherwise using the software, you are agreeing that you have read, and that you
#   agree to comply with and are bound by, such license terms. If you do not agree to be bound by the
#   applicable license terms, then you may not retain, install, activate or otherwise use the software.
 
#!/bin/bash

files=$(find . -type f -name '*.mp3')

while IFS= read -r f; do
    if [[ ${f::1} == "/" ]]
    then
      f=".$f"
    fi

    file_name=${f::-4}

    rm "$file_name".wav &>/dev/null
    rm "$file_name".opus &>/dev/null
    rm "$file_name".wav.new &>/dev/null
    rm "$file_name".h &>/dev/null

    if [ "$1" = "wav" ] && [ "$2" = "bin" ]; then
        echo "Processing $f file..."
        ffmpeg -i "$file_name".mp3 -sample_fmt s16 -ar 48000 -ac 1 -y "$file_name".wav 2> logs.txt

        echo ""

    elif [ "$1" = "wav" ] && [ "$2" = "c_array" ]; then
        echo "Processing $f file..."
        ffmpeg -i "$file_name".mp3 -sample_fmt s16 -ar 48000 -ac 1 -y "$file_name".wav 2> logs.txt
        xxd -i "$file_name".wav "$file_name".h

        rm "$file_name".wav

        echo ""

    elif [ "$1" = "opus" ] && [ "$2" = "bin" ]; then
        echo "Processing $f file..."
        ffmpeg -i "$file_name".mp3 -sample_fmt s16 -ar 48000 -ac 1 -y "$file_name".wav 2> logs.txt
        ./opus_encoder "$file_name".wav "$file_name".opus "$file_name".wav.new

        rm "$file_name".wav
        rm "$file_name".wav.new

        echo ""

    elif [ "$1" = "opus" ] && [ "$2" = "c_array" ]; then
        echo "Processing $f file..."
        ffmpeg -i "$file_name".mp3 -sample_fmt s16 -ar 48000 -ac 1 -y "$file_name".wav 2> logs.txt
        ./opus_encoder "$file_name".wav "$file_name".opus "$file_name".wav.new
        xxd -i "$file_name".opus > "$file_name".h
        sed -i 's/Standard/standard/g' "$file_name".h

        rm "$file_name".wav
        rm "$file_name".wav.new
        rm "$file_name".opus

        echo ""

    else
        echo "Please specify a valid audio format"
        echo "Valid formats: wav bin, wav c_array, opus bin, opus c_array"
        echo "Usage example: ./convert.sh opus bin"
        break
    fi

done <<< "$files"