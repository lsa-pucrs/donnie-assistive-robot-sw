.. gaitech documentation master file, created by
   sphinx-quickstart on Sun May  8 09:56:52 2016.
   You can adapt this file completely to your liking, but it should at least
   contain the root `toctree` directive.

.. _getting-started:

==================================
How to Build Your Own Donnie Robot
==================================

To build your own Donnie robot, please refer to the following repositories (github login and read access required):

- `Donnie electronics <https://github.com/lsa-pucrs/donnie-assistive-robot-hw>`_
- `Donnie 3D printing <https://github.com/lsa-pucrs/donnie-assistive-robot-3d>`_


Install Donnie's Software on an Embedded Computer (Raspberry Pi)
----------------------------------------------------------------------

Once the eletronics and 3D printing are done, the operating system and Donnie's software must be installed in the robot's Raspberry Pi.

Loading the Donnie Image into the Pi's SD Card
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The easiest way to setup the embedded computer is to use the pre-built image (comming soon!).
Please follow these steps to burn the SD card:

.. code-block:: none

   wget xxxxxxxx.img
   continue ...


Compile, Configure the OS, and Install the Donnie Image into the SD Card
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

This option is for programmers experient with Raspberry Pi.

Raspbian 8.0 (`Jessie <https://www.raspberrypi.org/downloads/raspbian/>`_) is the recommended OS distribution for the robot. 
Log onto Donnie's embedded computer, open a terminal, and run the following to download and execute the software installation script:

.. code-block:: none

   mkdir ~/donnie; cd ~/donnie
   wget https://github.com/lsa-pucrs/donnie-assistive-robot-sw/raw/devel/install-rpi.sh
   chmod +x ./install-rpi.sh 
   ./install-rpi.sh

Experienced programmers can configure the same paramerters presented `here <install.rst>`_, for the Raspberry Pi.