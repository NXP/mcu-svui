#!/usr/bin/env python3

"""

Copyright 2019, 2022 NXP.

This software is owned or controlled by NXP and may only be used strictly in accordance with the
license terms that accompany it. By expressly accepting such terms or by downloading, installing,
activating and/or otherwise using the software, you are agreeing that you have read, and that you
agree to comply with and are bound by, such license terms. If you do not agree to be bound by the
applicable license terms, then you may not retain, install, activate or otherwise use the software.

File
++++
/Scripts/demos/aws_onboard.py

Brief
+++++
** Test script for programatically creating a thing. **

.. versionadded:: 0.0

"""

import Ivaldi.onboard.aws as ob

def cert_cb(id, certPem):
    """
        Handles the generated PEM certificate.

        :param id: Certificate ID
        :type id: String
        :param certPem: PEM Encoded certificate
        :type certPem: String

        :returns: None
    """
    print(id)
    with open('./thisCert.pem', 'w+') as c:
        c.write(certPem)
        c.close()

def pkey_cb(id, prvKey, pubKey):
    """
        Handles the generated PEM private key and public key.

        :param id: Certificate ID
        :type id: String
        :param prvKey: PEM Encoded Private Key
        :type prvKey: String
        :param pubKey: PEM Encoded Public Key
        :type pubKey: String

        :returns: None
    """
    print(id)
    with open('./thisPrvKey.pem', 'w+') as prv:
        prv.write(prvKey)
        prv.close()
    with open('./thisPubKey.pem', 'w+') as pub:
        pub.write(pubKey)
        pub.close()

def main():
    """
        Create new AWS thing and attach certificate and policy

        :returns: None
    """

    # Create a new MakeThing object
    new_thing = ob.MakeThing(clientType='iot', thingName='thisnewthing')

    # If an IoT thing with the same name already exists, delete it
    new_thing.clean()

    # Create new thing and credentials
    thing_data = new_thing.create(cert_callback=cert_cb, key_callback=pkey_cb)

    print(thing_data)

    # Attach policy to the thing
    new_thing.attach('shadow_test')

if __name__ == '__main__':
    main()
