# Smart Voice UI solution release notes

## V1.1.0.
### What's new

* Updated VIT library to version 4.8.1
* Updated sln_asr lib to DSMT recognition engine 2.2.18.0
* Shell command "version" now displays also the VIT / DSMT lib version
* Added support for RT106C
* Added a new shell command, "factoryreset", which will revert project to default settings
* Added a new shell command: "changebank", which will change the boot settings from the active bank to the other one
* Added hello world project for Smart Voice
* Added iled blinky project for Smart Voice
* Added well documented project customization examples and patches that can be used to see the examples end-results
* Added pre-configured Ivaldi_updater on github (it will be removed from nxp.com SVUI solution page)\
    The Ivaldi_updater contains latest pre-built binaries as well
* Made ASR initialization more generic for DSMT
* Optimized VIT memory usage
* Improved VAD usage for going at a lower frequency after 10 minutes of silence
* Added guard for reported wake word longer than the max supported wake word

### Bug fixes

* Fixed an issue where VIT filtering was taking into account the command id instead of the action
* The announce_demo function did not check if the demo prompt was NULL
* When boot demo was the same as the default demo, the project skipped wake word detection at first boot
* APP_LAYER_HandleFirstBoardBoot was causing a compilation error when DSMT was enabled and NXP_OOBE definition was set to 0
* Push-to-talk setting was not persistent
* Setting ENABLE_STREAMER to 0 in app.h will make the project crash at runtime
* Setting VAD_ENABLED to 0 in app.h will completely break the detection
* Audio dump should not output garbage data as amp stream
* Bootloader project will always start to compile from scratch, even though nothing was modified since last build
* Sometimes microphones were still active after "mute on" shell command


### List of known issues:
|Ticket number|Description|
|:---|:---|
|SSV-294|Timeout doesn't expire when mute is on|
|SSV-322|MSD does not work with Linux|
|SSV-405|When image signature verification is enabled, multiple consecutive OTW updates will make the signature verification to fail|

## V1.0.0.
### What's new

* First official version of Smart Voice UI Solution project
* Contains smart home, elevator and washing machine demos
* Voice recognition using NXP's Voice Intelligent Technology (VIT) or Cybron/DSMT
* Multiple languages (en/de/fr/cn)
* Software support for multiple microphone configurations
* Voice activity detection

### List of known issues:
|Ticket number|Description|
|:---|:---|
|SSV-244|Board doesn't reboot itself after failed OTW update on the same bank|
|SSV-245|There is no error message when OTW update with empty bin file|
|SSV-246|Board doesn't get any error when wrong signature is set for OTW update|
|SSV-247|Board doesn't get any error when empty signature is set for OTW update|
|SSV-262|Default demo is not saved in flash after timeout on first boot(VIT)|
|SSV-294|Timeout doesn't expire when mute is on|
|SSV-295|VAD activation is too sensible|

### NOTE
Data lost prevention tools might interfere with the MSD process.

