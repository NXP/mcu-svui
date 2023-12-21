"""

Copyright 2019, 2022 NXP.

This software is owned or controlled by NXP and may only be used strictly in accordance with the
license terms that accompany it. By expressly accepting such terms or by downloading, installing,
activating and/or otherwise using the software, you are agreeing that you have read, and that you
agree to comply with and are bound by, such license terms. If you do not agree to be bound by the
applicable license terms, then you may not retain, install, activate or otherwise use the software.

File
++++
setup.py

Brief
+++++
** Ivaldi Modules Setup file **

.. versionadded:: 0.0

API
+++

"""

import distutils.log
from distutils.core import setup
from distutils.command.install_data import install_data
import shutil as sh
import os
import platform
import stat

#distutils.log.set_verbosity(distutils.log.DEBUG)

USB_TOOLS = ('sdphost', 'blhost')

OS_ABS = {
    'Linux':
    {
        'Microsoft':
        {
            '64bit': './Tools/<x>/win/<x>.exe',
            '32bit': './Tools/<x>/win/<x>.exe'
        },
        'Generic':
        {
            '64bit': './Tools/<x>/linux/amd64/<x>',
            '32bit': './Tools/<x>/linux/i386/<x>'
        }
    },
    'Darwin':
    {
        'Generic':
        {
            '64bit': './Tools/<x>/mac/<x>',
            '32bit': 'Not Supported'
        }
    },
    'Windows':
    {
        'Generic':
        {
            '64bit': '.\\Tools\\<x>\\win\\<x>.exe',
            '32bit': '.\\Tools\\<x>\\win\\<x>.exe'
        }
    }
}

def get_os_path(tool_name=None):
    """
        Get appropriate executables for host OS

        :param tool_name: Executable name
        :type tool_name: string
        :returns: (str) Path to host os compatible executable
    """
    thisSys = platform.system()
    thisRel = 'Microsoft' if -1 < platform.release().find('Microsoft') else 'Generic'
    thisArch = platform.architecture()[0]

    if tool_name:
        if tool_name in USB_TOOLS:
            # USB Tools must use executable that can access USB, i.e., *.exe on WSL systems
            return OS_ABS[thisSys][thisRel][thisArch].replace('<x>', tool_name)
        else:
            # Non-USB access tools can use 'Generic' platform executable
            return OS_ABS[thisSys]['Generic'][thisArch].replace('<x>', tool_name)
    else:
        # Special case for determining which directory to save the executables
        return 'Scripts' if thisSys == 'Windows' else 'bin'

class PostInstall(install_data):
    def run(self):

        # Gather list of binary files
        data_paths = self.get_inputs()

        # Build destination path for binary files
        dest_path = os.environ['VIRTUAL_ENV'] + '/' + data_paths[0][0] + '/'

        # Copy each file to new destination, changing permissions to executable
        for d in data_paths[0][1]:

            file_dest = dest_path + os.path.basename(d)

            try:
                sh.copyfile(d, file_dest)
                os.chmod(file_dest, os.stat(file_dest).st_mode | stat.S_IEXEC)
            except Exception as e:
                print("")
                print("----- ERROR -----")
                print(e)
                is_mac = platform.system() == 'Darwin'
                is_cst_srk = (os.path.basename(d) == 'cst') or (os.path.basename(d) == 'srktool')
                if is_mac and is_cst_srk:
                    distutils.log.info("WARNING: %s is currently not supported on this platform.", d)
                else:
                    distutils.log.info("ERROR: Unable to copy file from '%s' to '%s'", d, file_dest)
                print("-----------------")
                print("")


setup(
    name='Ivaldi',
    version='0.0',
    packages=['Ivaldi',
              'Ivaldi.blhost',
              'Ivaldi.sdphost',
              'Ivaldi.helpers',
              'Ivaldi.onboard',
              'Ivaldi.elftosb',
              'Ivaldi.cst'],
    data_files=[(get_os_path(), [get_os_path('blhost')
                              , get_os_path('sdphost')
                              , get_os_path('elftosb')
                              , get_os_path('cst')
                              , get_os_path('srktool')])],
    license='See LA_OPT_NXP_Software_License',
    long_description=open('README.md').read(),
    cmdclass={'install_data': PostInstall}
)
