.. _getting-started:

==================================
How to Install Donnie's Software
==================================

There are several ways to install Donnie's software environment:

- `Run Donnie's Software from a Virtual Machine (VM)`_
- `Install Donnie's Software with the Debian Package`_
- `Build and Install Donnie's Software`_


_`Run Donnie's Software from a Virtual Machine (VM)`
--------------------------------------------------

Download this VM file (comming soon!), with all the software pre-installed and ready to run.
The VM file can be run with a Virtualization tool such as `VirtualBox <https://www.virtualbox.org>`_.


_`Install Donnie's Software with the Debian Package`
-----------------------------------------------------

Ubuntu 14.04 (`Trusty Tahr <http://releases.ubuntu.com/14.04/>`_) is the recommended OS distribution. For older computers or VMs, `Lubuntu <http://cdimage.ubuntu.com/lubuntu/releases/14.04/release/>`_ 14.04 or `Ubuntu Mate <https://ubuntu-mate.org/download/index.html>`_ 14.04 are recommended.

Download the package (comming soon!) and execute the following steps in the terminal:

- It is recommended to update your packages before the instalation:

.. code-block:: bash

	sudo apt-get install update

- To install, you can double-click the ".deb" package or run:

.. code-block:: bash

	sudo dpkg -i donnie_1.0.0_amd64.deb

- In case of missing dependencies, try:

.. code-block:: bash

	sudo apt-get install -f

- To uninstall:

.. code-block:: bash

	sudo dpkg -r donnie


_`Build and Install Donnie's Software`
-------------------------------------

Ubuntu 14.04 (`Trusty Tahr <http://releases.ubuntu.com/14.04/>`_) is the recommended OS distribution. For older computers or VMs, `Lubuntu <http://cdimage.ubuntu.com/lubuntu/releases/14.04/release/>`_ 14.04 or `Ubuntu Mate <https://ubuntu-mate.org/download/index.html>`_ 14.04 are recommended.

Open a terminal, download the installation script and execute it:

.. code-block:: bash

	mkdir ~/donnie; cd ~/donnie
	wget https://github.com/lsa-pucrs/donnie-assistive-robot-sw/raw/devel/install.sh
	chmod +x ./install.sh
	./install.sh

Configuring Donnie
^^^^^^^^^^^^^^^^^^^^^^^

The installation script composes a standard instalation that we believe is the most appropriate for the average user. 
However, advanced parameters can be set if the user has experience with the appropriate tools.

The build system is based on cmake, so experience with Linux, `make <https://www.gnu.org/software/make/>`_, and `cmake <https://cmake.org/>`_ is required. All the individual parts of Donnie's Software Stack are also based on CMake. These are the software parts that can be customized, each with its own set of parameters:

- raspicam driver
- Player
- Stage
- Donnie Software

Developers interested in customization might want to read the following files:

- `install.sh <https://github.com/lsa-pucrs/donnie-assistive-robot-sw/blob/master/install.sh>`_: For desktop setup procedure;
- `setup.sh.in <https://github.com/lsa-pucrs/donnie-assistive-robot-sw/blob/master/install/setup.sh.in>`_
- `install-rpi.sh <https://github.com/lsa-pucrs/donnie-assistive-robot-sw/blob/master/install-rpi.sh>`_: For embedded computer (e.g. Raspberry Pi) setup procedure;
- `setup-rpi.sh.in <https://github.com/lsa-pucrs/donnie-assistive-robot-sw/blob/master/install/setup-rpi.sh.in>`_
- and all the *CMakeLists.txt* files

Parameters for Donnie's Software
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The following list explains Donnie's main compilation parameters:

.. code-block:: none

	BUILD_DOCS           OFF              Generate Donnie's documents.
	BUILD_DOXYGEN        ON               This is the default document in HTML, meant only for developers.
	BUILD_DOXYGEN_PDF    OFF              The same document before, but in PDF.
	BUILD_EXAMPLES       OFF              Build the examples for each part of Donnie.
	BUILD_MANUAL         OFF              Build the manuals: software manual, hardware manual, user manual.
	CMAKE_BUILD_TYPE     Release | Debug  Debug mode is for developers only !
	DOC_LANGUAGE         en | pt-br | es  The language used to build documents and the GoDonnie interpreter. Future work !