
<!---
[//]: # incluir travis
[//]: #| Linux                           |
[//]: #|---------------------------------|
[//]: #| [![Build Status][master]][repo] |
[//]: # https://github.com/forexample/package-example/blob/master/CMakeLists.txt

[//]: #[master]: https://travis-ci.org/forexample/package-example.svg?branch=master
[//]: #[repo]: https://travis-ci.org/forexample/package-example
-->

# Donnie Software Stack

Robotics has been used to teach young students the basics of programming. However, 
most of the programming environments for kids are high visual, based on grab and drag blocks.
**Blind students or students with some visual disability cannot use these teaching resources**. 

Donnie project proposes an inclusive robotic programming environment where all students 
(with or without visual disabilities) can use. 

Contents
--------

1. [Getting Started](#getting-started)
2. [Features](#features)
3. [Building Your Donnie Robot](#building-your-donnie-robot)
4. [Configuring Donnie](#configuring-donnie)
5. [Contributors](#contributors)
6. [Contributing](#contributing)
7. [Papers](#papers)


# Getting Started
---------------

<!---
TODO colocar um link do Donnie no youtube
<a href="http://www.youtube.com/watch?feature=player_embedded&v=YOUTUBE_VIDEO_ID_HERE
" target="_blank"><img src="http://img.youtube.com/vi/YOUTUBE_VIDEO_ID_HERE/0.jpg" 
alt="IMAGE ALT TEXT HERE" width="240" height="180" border="10" /></a>
-->

### Run Donnie Software from a Virtual Machine

 Download this [VM](link) all Donnie software pre-installed and ready to run.


### Install Donnie Software on a Desktop Computer

 On a Linux computer, download this repository 
 
	sudo apt-get install git
	cd Downloads
 	git clone  https://github.com/lsa-pucrs/donnie-assistive-robot-sw.git
	<!---
	wget https://raw.github.com/lsa-pucrs/donnie-assistive-robot-sw/feature-scripts/install.sh
	-->
	
 and type the following command:

	cd donnie-assistive-robot-sw.git
	chmod +x ./install.sh 
	./install.sh




### Install Donnie Software on a Donnie's Embedded Computer (Raspberry Pi)

 Logon the Donnie's embedded computer, download this repository 
 
	sudo apt-get install git
	cd Downloads
 	git clone  https://github.com/lsa-pucrs/donnie-assistive-robot-sw.git
	
 and type the following command:

	cd donnie-assistive-robot-sw.git
	chmod +x ./install-rpi.sh 
	./install-rpi.sh

# Features
---------------

* Robot programming environment for young students with or without visual impairment; 
* Assistive programming language called GoDonnie;
* Integration with a Arduino-based robot with Player robotic middleware;
* Extension of Stage simulator to generate sound clues while the robot is moving;
* Software developed for the simulated robot is compatible with the real Donnie robot;

# Building Your Donnie Robot
---------------

To build your own Donnie robot, please refer to the following repositories:

* [Donnie eletronics](https://github.com/lsa-pucrs/donnie-assistive-robot-hw)
* [Donnie 3D printing](https://github.com/lsa-pucrs/donnie-assistive-robot-3d)


# Configuring Donnie
---------------

The installation scripts make a standard instalation we believe is the most appropriated for the average user.
However, advanced parameters can be set if the user has experience with the appropriate tools.

The build system is based on CMake, so it is required to have some experience with Make and CMake.
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

	BUILD_DOCS                       OFF    Generate Donnie's documents
	BUILD_DOXYGEN                    ON     This is the default document in HTML, meant only for developers.
	BUILD_DOXYGEN_PDF                OFF    The same document before, but in PDF
	BUILD_EXAMPLES                   OFF    Build the examples for each part of Donnie
	BUILD_MANUAL                     OFF    Build the manuals: software manual, hardware manual, user nanual
	CMAKE_BUILD_TYPE                 Release 
	DOC_LANGUAGE                     en     The language used to build documents and the GoDonnie interpreter. Future work !

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

# Contributing
------------

Donnie project has a small core team and it relies on external collaboration to continue developing this assistive system.
This is a list of itens the core team wish to have some help:

* Translattion to other languages. Both the GoDonnie commands and Donnie's documents have to be translated. In the near future we wish to support en, pt-br, es;
* More documentation and tutorials. Currently we are building English docs;
* More examples using GoDonnie language;
* Porting to Windows. It currently works only on Linux, however, Windows has great assitive tools such as NVDA;
* Integrate more assistive tools for blind users such as ORCA, NVDA, among others;
* Feature requests and testing. Please use the Issue system to request new features and to report bugs;


# Papers
------------

If you are using Donnie on your research projects, please cite our papers:

* To Appear


