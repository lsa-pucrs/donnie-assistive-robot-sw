
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


Contents
--------

1. `Getting Started`_
2. `Configuring Donnie`_
3. `Contributors`_
4. `Contributing`_
5. `Papers`_


Getting Started
---------------

<!---
TODO colocar um link do Donnie no youtube
<a href="http://www.youtube.com/watch?feature=player_embedded&v=YOUTUBE_VIDEO_ID_HERE
" target="_blank"><img src="http://img.youtube.com/vi/YOUTUBE_VIDEO_ID_HERE/0.jpg" 
alt="IMAGE ALT TEXT HERE" width="240" height="180" border="10" /></a>
-->

### Install Donnie Software on a Desktop Computer

	./install

### Install Donnie Software on a Donnie's Embedded Computer (Raspberry Pi)

	./install-rpi

Configuring Donnie
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

Contributors
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

Contributing
------------

Donnie project has a small core team and it relies on external collaboration to continue developing this assistive system.
This is a list of itens the core team wish to have some help:

* Translattion to other languages. Both the GoDonnie commands and Donnie's documents have to be translated. In the near future we wish to support en, pt-br, es;
* More documentation and tutorials. Currently we are building English docs;
* More examples using GoDonnie language;
* Porting to Windows. It currently works only on Linux, however, Windows has great assitive tools such as NVDA;
* Integrate more assistive tools for blind users such as ORCA, NVDA, among others;
* Feature requests and testing. Please use the Issue system to request new features and to report bugs;


Papers
------------

If you are using Donnie on your research projects, please cite our papers:

* To Appear


