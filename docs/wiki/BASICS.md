<!---
# Chapter 2 - The Basics
--->

## 2.1 - Important File Types

In Player/Stage there are 3 kinds of file that you need to understand to get going with Player/Stage:

* a .world file
* a .cfg (configuration) file
* a .inc (include) file

The .world file tells Player/Stage what things are available to put in the
world. In this file you describe your robot, any items which populate the
world and the layout of the world. The .inc file follows the same syntax
and format of a .world file but it can be *included*. So if there is an
object in your world that you might want to use in other worlds, such as a
model of a robot, putting the robot description in a .inc file just makes
it easier to copy over, it also means that if you ever want to change your
robot description then you only need to do it in one place and your
multiple simulations are changed too.

The .cfg file is what Player reads to get all the information about the
robot that you are going to use.This file tells Player which drivers it
needs to use in order to interact with the robot, if you're using a real
robot these drivers are built in to Player (or you can download or
write your own drivers, but I'm not going to talk about how to do this
here.)  Alternatively, if you want to make a simulation, the driver is
always Stage (this is how Player uses Stage in the same way it uses a
robot: it thinks that it is a hardware driver and communicates with it as
such). The .cfg file tells Player how to talk to the driver, and how to
interpret any data from the driver so that it can be presented to your
code. Items described in the .world file should be described in the .cfg
file if you want your code to be able to interact with that item (such as a
robot), if you don't need your code to interact with the item then this
isn't necessary. The .cfg file does all this specification using interfaces
and drivers, which will be discussed in the following section.

## 2.2 - Interfaces, Drivers and Devices

* Drivers are pieces of code that talk directly to hardware. These are
  built in to Player so it is not important to know how to write these as
  you begin to learn Player/Stage. The drivers are specific to a piece of
  hardware so, say, a laser driver will be different to a camera driver,
  and also different to a driver for a different brand of laser. This is
  the same as the way that drivers for graphics cards differ for each make
  and model of card. Drivers produce and read information which conforms to
  an ``interface''.
* Interfaces are a set way for a driver to send and receive information
  from Player. Like drivers, interfaces are also built in to Player and
  there is a big list of them in the 
  [Player manual](http://playerstage.sourceforge.net/doc/Player-3.0.2/player/group__interfaces.html).
  They specify the syntax and semantics of how drivers and Player interact.
* A device is a driver that is bound to an interface so that Player can
  talk to it directly. This means that if you are working on a real robot
  that you can interact with a real device (laser, gripper, camera etc) on
  the real robot, in a simulated robot you can interact with their
  simulations. 

The official documentation actually describes these three things quite well
with an [example](http://playerstage.sourceforge.net/doc/Player-3.0.2/player/group__tutorial__devices.html).
(Actually, the official documentation still refers to the depreciated laser
interface, but I've updated all the references in this manual to use the
new ranger interface.)

>Consider the ranger interface. This interface defines a format in which a planar range-sensor can return range readings (basically a list of ranges, with some meta-data). The ranger interface is just that: an interface. You can't do anything with it.
>
>Now consider the sicklms200 driver. This driver controls a SICK LMS200, which is particular planar range sensor that is popular in mobile robot applications. The sicklms200 driver knows how to communicate with the SICK LMS200 over a serial line and retrieve range data from it. But you don't want to access the range data in some SICK-specific format. So the driver also knows how to translate the retrieved data to make it conform to the format defined by the ranger interface.
>
>	The sicklms200 driver can be bound to the ranger interface ... to create a device, which might have the following address:
>
>	`localhost:6665:ranger:0`
>	
>The fields in this address correspond to the entries in the
> `player_devaddr_t` structure: host, robot, interface, and index. The host and robot fields (localhost and 6665) indicate where the device is located. The interface field indicates which interface the device supports, and thus how it can be used. 
>Because you might have more than one ranger, the index field allows you to pick among the devices that support the given interface and are located on the given host:robot Other lasers on the same host:robot would be assigned different indexes.

The last paragraph there gets a bit technical, but don't worry. Player talks to parts of the robot using ports (the default port is 6665), if you're using Stage then Player and Stage communicate through these ports (even if they're running on the same computer). 
All this line does is tell Player which port to listen to and what kind of data to expect. In the example it's laser data which is being transmitted on port 6665 of the computer that Player is running on (localhost). 
You could just as easily connect to another computer by using its IP
address instead of ``localhost''. The specifics of writing a device
address in this way will be described in 
[Chapter 4](CFGFILES.md)

![img](http://nojsstats.appspot.com/UA-66082425-1/player-stage-manual.readthedocs.org)
