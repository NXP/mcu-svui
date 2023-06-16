# Description
Saves the audio streams as .wav files if audio dump is enabled on the board.
Requires python 3.7.0 or higher.


# Prerequisites:
1. pip install pyserial
2. pip install scipy


# Usage guide:
1. Run from Command Prompt or Linux terminal


# Arguments:
1. Required:
	-m, --microphones - Set the number of microphones.
	-p, --port        - Set the port the device is connected. Example for Window COM3, for Linux /dev/ttyACM3.
	-f, --folder      - Folder where to save audio files. Note: overwrite existing folder.
2. Optional:
	-mss, --mic-sample-size - Set microphone one sample's size in bytes. - default=2
	-t, --type              - Dump type: AFE (mics, amp, clean), ASR (clean), ASR2 (clean+amp) - default='AFE'


# Windows usage example
python parse_audio_stream.py -m 3 -p COM5 -f testfolder
