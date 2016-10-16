
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

## Install Donnie Software on a Desktop computer

	./install

## Install Donnie Software on a Donnie's Embedded Computer (Raspberry Pi)

	./install-rpi

## Customizing Donnie Instalation

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

