# Smart Voice UI solution release notes
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

