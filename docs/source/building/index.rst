.. _arduino:

===============
Building Your Donnie Robot 
===============

Introduction
-------------

This manual has all files required to understand how to build the Donnie's body with a 3D printer.

Required Material
-----------------

Donnie requires about 500g of PLA. We use PLA because its low retraction
factor in large pieces.

Production Phase
----------------

1. The 3D printer requires the stl files, in the stl\_files folder.
2. We use `Slicer (1.2.9) <http://slic3r.org/>`__ to slice and 3d
   printing the robot. We use the following configs on slicer:

-  Infill: 20%;
-  Layer height: 0.3mm;
-  Without support (parts that need support have it in the model).

Modifying Donnie's Body
-----------------------

We used the Solidworks 2014 to model the robot. All the source files are
in the solidworks directory.

Visualization
-------------

You can visualize the 3D PDF files with `Adobe Reader
9 <http://www.adobe.com/>`__ or above. You just need to click in "Enable
3D View" when open the 3D PDF.

.. figure:: screenshot.png
   :alt: Meet Donnie !!!

   Meet Donnie !!!

explica o shield, e como conecta ele no arduino. colocar diagrama e descreve as pinagens.
como carregar o firmware

Assembly the Arduino Part
-------------

Arduino Firmware
-------------

explicar brevemente a organizacao do firmware

