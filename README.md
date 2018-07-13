
<!---
[//]: # incluir travis
[//]: #| Linux                           |
[//]: #|---------------------------------|
[//]: #| [![Build Status][master]][repo] |
[//]: # https://github.com/forexample/package-example/blob/master/CMakeLists.txt

[//]: #[master]: https://travis-ci.org/forexample/package-example.svg?branch=master
[//]: #[repo]: https://travis-ci.org/forexample/package-example
-->

[![Build Status](https://travis-ci.org/lsa-pucrs/donnie-assistive-robot-sw.svg?branch=devel)](https://travis-ci.org/lsa-pucrs/donnie-assistive-robot-sw)

# Donnie Software Stack

Robotics has been used to teach young students the basics of programming. However,
most of the programming environments for kids are high visual, based on grab and drag blocks.
**Blind students or students with some visual disability cannot use these teaching resources**.

Donnie project proposes an inclusive robotic programming environment where all students
(with or without visual disabilities) can use.

Donnie comes with two usage options: simulated robot and physical robot. It is recommend to start with the simulated since it does not require building the robot. Moreover, the physical robot is functional, but still under test.

Contents
--------

1. [Features](#features)
2. [Getting Started with the Simulated Donnie Robot](#getting-started-with-the-simulated-donnie-robot)
3. [Building Your Donnie Robot](#building-your-donnie-robot)
4. [Contributors](#contributors)
5. [Contributing](#contributing)
6. [Support](#support)
7. [Papers](#papers)

# Features
---------------

* Robot programming environment for young students with or without visual impairment;
* Assistive programming language called GoDonnie. GoDonnie is TTS and screen reader friendly;
* Integration with a Arduino-based robot with Player robotic middleware;
* Extension of Stage simulator to generate sound clues while the robot is moving;
* Software developed for the simulated robot is compatible with the real Donnie robot;


# Getting Started with the Simulated Donnie Robot
---------------

<!---
TODO colocar um link do Donnie no youtube
<a href="http://www.youtube.com/watch?feature=player_embedded&v=YOUTUBE_VIDEO_ID_HERE
" target="_blank"><img src="http://img.youtube.com/vi/YOUTUBE_VIDEO_ID_HERE/0.jpg"
alt="IMAGE ALT TEXT HERE" width="240" height="180" border="10" /></a>
-->

  The simulation is recommended if you want to known about Donnie but you dont have the required resources to build your own Donnie robot.

## Run Donnie Software from a Virtual Machine

 Download this [VM](link) all Donnie software pre-installed and ready to run.

## Install Donnie Software on a Desktop Computer Using the Packages

 [**Ubuntu 14.04**](http://releases.ubuntu.com/14.04/) (Trusty Tahr) is the recommended OS distribution.
 For older computers or VMs, [Lubuntu 14.04](http://cdimage.ubuntu.com/lubuntu/releases/14.04/release/) or [Ubuntu Mate 14.04](https://ubuntu-mate.org/trusty/) are recommended.

 Download the [package](releases/latest) and execute the following steps in the terminal:

 * It is recommended to update your packages before the instalation:
> sudo apt-get update
* To install, you can double-click it or execute:
> sudo dpkg -i donnie_1.0.0_amd64.deb
* In case of missing dependencies, try:
> sudo apt-get install -f
* To unistall:
> sudo dpkg -r donnie

## Compile and Install Donnie Software on a Desktop Computer

  This option is recommended only for experienced Linux programmers. If it is not your case, please install Donnie using the pre-compiled packages.

 [**Ubuntu 14.04**](http://releases.ubuntu.com/14.04/) (Trusty Tahr) is the recommended OS distribution.
 For older computers or VMs, [Lubuntu 14.04](http://cdimage.ubuntu.com/lubuntu/releases/14.04/release/) or [Ubuntu Mate 14.04](https://ubuntu-mate.org/trusty/) are recommended.

 Open a terminal, download this script, and execute it

	mkdir ~/donnie; cd ~/donnie
	wget https://github.com/lsa-pucrs/donnie-assistive-robot-sw/raw/devel/install.sh
	chmod +x ./install.sh
	./install.sh

### Configuring Donnie

The installation scripts make a standard instalation we believe is the most appropriated for the average user.
However, advanced parameters can be set if the user has experience with the appropriate tools.

The build system is based on CMake, so it is required to have some experience with Linux, Make, and CMake.
Every part of Donnie Software Stack is also based on CMake. These are the software parts that can be customized:

- raspicam driver
- Player
- Stage
- Donnie Software

each of these packages have their one sets of parameters.

Developers interested in customization might want to read the following files:

- [install.sh](install.sh). For desktop setup procedure.
- [install/setup.sh](install/setup.sh)
- [install-rpi.sh](install-rpi.sh). For embedded computer setup procedure.
- [install/setup-rpi.sh](install/setup-rpi.sh)
- and all the CMakeLists.txt files


### Parameter for Donnie Software

The following list explains Donnie's main compilation parameters:

	BUILD_DOCS           OFF              Generate Donnie's documents.
	BUILD_DOXYGEN        ON               This is the default document in HTML, meant only for developers.
	BUILD_DOXYGEN_PDF    OFF              The same document before, but in PDF.
	BUILD_EXAMPLES       OFF              Build the examples for each part of Donnie.
	BUILD_MANUAL         OFF              Build the manuals: software manual, hardware manual, user manual.
	CMAKE_BUILD_TYPE     Release | Debug  Debug mode is for developers only !
	DOC_LANGUAGE         en | pt-br | es  The language used to build documents and the GoDonnie interpreter. Future work !


# Building Your Donnie Robot
---------------

To build your own Donnie robot, please refer to the following repositories (github login and read acess required!):

* [Donnie eletronics](https://github.com/lsa-pucrs/donnie-assistive-robot-hw)
* [Donnie 3D printing](https://github.com/lsa-pucrs/donnie-assistive-robot-3d)


## Install Donnie Software on a Donnie's Embedded Computer (Raspberry Pi)

  Once the eletronics and the 3D printing were done, one must load the operating system and Donnie's software into SD Card.

### Install the Donnie Image into the SD Card

  The easiest way to setup Donnie's embedded computer is to use the pre-built image. Please follow these steps to burn the SD card:

  	wget xxxxxxxxx.img
	continue ...


### Compile, Configure the OS, and Install the Donnie Image into the SD Card

  This option is for programmers with experience with Raspberry Pi.

 [**Raspbian 8.0**](https://www.raspberrypi.org/downloads/raspbian/) (Jessie) is the recommended OS distribution for Donnie robot.
 Logon the Donnie's embedded computer, open a terminal, download this script, and execute it

	mkdir ~/donnie; cd ~/donnie
	wget https://github.com/lsa-pucrs/donnie-assistive-robot-sw/raw/devel/install-rpi.sh
	chmod +x ./install-rpi.sh
	./install-rpi.sh

 Experienced programmers can configure the same paramerters presented before for the Raspberry Pi.


# Contributors
------------

We thank the following people for contributing to **Donnie**:

* [Alexandre Amory](https://github.com/amamory): Hardware design, Embedded software, software stack, robotic middleware, build system
* [Augusto Bergamin](https://github.com/AugustoPB): Hardware design, Arduino programming, Embedded software, GoDonnie parser
* [Camila Kolling](https://github.com/camilakolling): assistive evaluation, testing, testing with blind users
* [Daniel Einloft](https://github.com/DanielEinlof): odometry, blobfinder, raspicam driver
* [Guilherme Marques](https://github.com/marquesgh2m): Hardware design, Arduino programming, Embedded software, software stack, robotic middleware
* [Henry Nunes](https://github.com/HenryNunes): assistive evaluation, testing, testing with blind users
* [Isabel Manssour](https://github.com/isabelhm): Computer vision
* [Juliana Damasio](https://github.com/julianadamasio): assistive evaluation, testing, testing with blind users
* [Joice Marek](https://github.com/joiceMarek): visual odometry, raspicam driver
* [Marcia Campos](https://github.com/marciabcampos): assistive evaluation, testing, testing with blind users
* [Renan Maidana](https://github.com/rgmaidana): Hardware design, Embedded software, Arduino programming, software stack, robotic middleware, build system

# Contributing
------------

Donnie project has a small core team and it relies on external collaboration to continue developing this assistive system.
This is a list of itens the core team wish to have some help:

* Translattion to other languages. Both the GoDonnie commands and Donnie's documents have to be translated. In the near future we wish to support en, pt-br, es;
* More documentation and tutorials. Currently we are building English docs;
* More examples using GoDonnie language;
* Porting to Windows. It currently works only on Linux, however, Windows has great assitive tools such as NVDA;
* Web-based frontend. so it can be used without much installation and setup;
* Integrate more assistive tools for blind users such as ORCA, NVDA, among others;
* Feature requests and testing. Please use the [Issue](https://github.com/lsa-pucrs/donnie-assistive-robot-sw/issues) system to request new features and to report bugs;

# Demonstration
------------
This [video](https://www.youtube.com/watch?v=BRXAqvGPGmU) demonstrates the functionality of Donnie Robot and Donnie simulation. In this task the robot is asked to move forward until hitting a wall, then it must turn to the right.


# Support
------------

Please use the github [Issue](https://github.com/lsa-pucrs/donnie-assistive-robot-sw/issues) for support.

# Papers
------------

If you are using Donnie on your research projects, please cite our papers:



	@inproceedings{oliveira2017teaching,
	  title={Teaching Robot Programming Activities for Visually Impaired Students: A Systematic Review},
	  author={Oliveira, Juliana Damasio and de Borba Campos, M{\'a}rcia and de Morais Amory, Alexandre and Manssour, Isabel Harb},
	  booktitle={International Conference on Universal Access in Human-Computer Interaction},
	  pages={155--167},
	  year={2017},
	  organization={Springer}
	}


	@inproceedings{guilherme2017donnie,
	  title={Donnie Robot: Towards an Accessible And Educational Robot for Visually Impaired People},
	  author={Guilherme H. M. Marques, Daniel C. Einloft, Augusto C. P. Bergamin, Joice A. Marek, Renan G. Maidana Marcia B. Campos, Isabel H. Manssour, Alexandre M. Amory},
	  booktitle={Latin American Robotics Symposium (LARS)},
	  year={2017}
	}
