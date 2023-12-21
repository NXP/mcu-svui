"""
Copyright 2023 NXP.

NXP Confidential. This software is owned or controlled by NXP and may only be used strictly in accordance with the
license terms that accompany it. By expressly accepting such terms or by downloading, installing,
activating and/or otherwise using the software, you are agreeing that you have read, and that you
agree to comply with and are bound by, such license terms. If you do not agree to be bound by the
applicable license terms, then you may not retain, install, activate or otherwise use the software.
"""

import os
import argparse

def main():
    """
    Creates the defines for the audio files that need to be copied in sln_flash_files.h in the main app
    """

    """ Parse the provided parameters """
    parser = argparse.ArgumentParser()
    parser.add_argument('-af', '--audio-folder', default="../../../Image_Binaries/svui_audio_files/", type=str, help="Specify the folder that contains the audio files")

    args = parser.parse_args()

    header = open('sln_flash_files.h', 'w')
    try:
        for filename in os.listdir(args.audio_folder):

            demo_dir = os.path.join(args.audio_folder, filename)

            if os.path.isdir(demo_dir):
                for lang_dir_name in os.listdir(demo_dir):
                    lang_dir = os.path.join(demo_dir, lang_dir_name)

                    if os.path.isdir(lang_dir):
                        for f in os.listdir(lang_dir):
                            f_path = os.path.join(lang_dir, f)
                            lfs_path = filename + "/" + lang_dir_name + "/" + f

                            fn = lfs_path.split("/")[2]

                            aux = fn.split(".opus")[0].upper()

                            if aux[0].isdigit() and aux[0].isdigit():
                                left = "AUDIO" + aux[2:]
                            else:
                                left = "AUDIO_" + aux

                            right = '"' + lfs_path  + '"'
                            spaces_len = 57 - len(left)
                            spaces = ""
                            for i in range(spaces_len):
                                spaces += " "
                            header.write("#define " + left + spaces + right + "\n")
        print("SUCCESS! generated sln_flash_files.h") 
    except:
        print("ERROR! could not generate the header")    

if __name__ == '__main__':
    main()
