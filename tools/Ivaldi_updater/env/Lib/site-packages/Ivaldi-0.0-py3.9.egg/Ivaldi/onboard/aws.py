"""

Copyright 2019, 2022 NXP.

This software is owned or controlled by NXP and may only be used strictly in accordance with the
license terms that accompany it. By expressly accepting such terms or by downloading, installing,
activating and/or otherwise using the software, you are agreeing that you have read, and that you
agree to comply with and are bound by, such license terms. If you do not agree to be bound by the
applicable license terms, then you may not retain, install, activate or otherwise use the software.

File
++++
Ivaldi/onboard/aws.py

Brief
+++++
** Ivaldi aws onboarding wrapper **

.. versionadded:: 0.0

API
+++

"""

import boto3

class MakeThing(object):
    """
        AWSCmd


    """

    def __init__(self, clientType='iot', thingName=''):
        """
        """
        self._client = boto3.client(clientType)
        self._thingName = thingName
        self._thingArn = ''
        self._thingId = ''
        self._certArn = ''
        self._certId = ''

    def create(self, cert_callback=None, key_callback=None):
        """
            Create an AWS IoT Thing, Certificate and Key pair

            :param cert_callback: Callback function for saving certificate
            :type cert_callback: function(id, certPem)

            :param key_callback: Callback function for saving key pair
            :type key_callback: function(id, prvKeyPem, pubKeyPem)

            :returns: (dict) Thing name, arn and ID, and Cert arn and ID
        """

        print("Creating new thing...")

        resThing = self._client.create_thing(thingName=self._thingName)

        self._thingArn = resThing['thingArn']
        self._thingId = resThing['thingId']

        resCert = self._client.create_keys_and_certificate(setAsActive=True)

        self._certArn = resCert['certificateArn']
        self._certId = resCert['certificateId']

        certPem = resCert['certificatePem']

        # Save certificate using cert_callback method if provided
        if cert_callback:
            cert_callback(self._certId, certPem)
        else:
            with open('./' + self._certId + '_crt.pem', 'w+') as cert:
                cert.write(certPem)
                cert.close()

        prvKey = resCert['keyPair']['PrivateKey']
        pubKey = resCert['keyPair']['PublicKey']

        # Save key pair using key_callback method if provided
        if key_callback:
            key_callback(self._certId, prvKey, pubKey)
        else:
            with open('./' + self._certId + '_prv.pem', 'w+') as prv:
                prv.write(prvKey)
                prv.close()
            with open('./' + self._certId + '_pub.pem', 'w+') as pub:
                pub.write(pubKey)
                pub.close()

        return {'thing': {'name': self._thingName, 'arn': self._thingArn, 'id': self._thingId}, 'certificate': {'arn': self._certArn, 'id': self._certId}}

    def attach(self, policy_name):
        """
            Attach policy and principal to AWS IoT Thing

            :param policy_name: Policy name
            :type policy_name: str

            :returns: None
        """

        self._client.attach_thing_principal(thingName=self._thingName, principal=self._certArn)
        self._client.attach_policy(policyName=policy_name, target=self._certArn)

    def clean(self):
        """
            Delete existent AWS IoT Thing and
            all certificates attached to it

            :returns: None
        """

        # Check if an IoT thing with the current name exists
        try:
            self._client.describe_thing(thingName=self._thingName)

        except:
            # Thing does not exist, nothing to delete
            pass

        else:
            # Delete the old thing with the same name
            print('Deleting existing thing...')
            all_principals = self._client.list_thing_principals(thingName=self._thingName)

            for principal_name in all_principals['principals']:
                certificate_id = principal_name.split("/")[1]
                all_policies = self._client.get_effective_policies(principal=principal_name)
                policy_name = all_policies['effectivePolicies'][0]['policyName']

                if len(all_policies['effectivePolicies']) > 0:
                    policy_name = all_policies['effectivePolicies'][0]['policyName']
                    self._client.detach_policy(policyName=policy_name, target=principal_name)

                self._client.detach_thing_principal(thingName=self._thingName, principal=principal_name)
                self._client.update_certificate(certificateId=certificate_id, newStatus='INACTIVE')
                self._client.delete_certificate(certificateId=certificate_id)

            self._client.delete_thing(thingName=self._thingName)