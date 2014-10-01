### AppWare Target

This is a purely hypothetical description of what AppWare could be; it does not actually exist as such.

####Kernel

- the kernel provides a platform agnostic interface to low-level core operations that tend to be platform specific. __Anything that calls a native function must be part of the kernel__.
- eg, the kernel might provide a single interface to native operations in C++ (for Windows) and Obj-C (for OSX)
- the kernel needs to be written in C, so it can be used from C++ and ObjC

	file reading/writing, streaming
	access/permissions/security

	


####Node.js Runtime

- the runtime is the interface between the kernel and Node.js

####MVC

- the MVC (Model View Controller) provides an abstraction (on top of the Kernel) 

abstraction into Node.js


####Web Kernel

	???


####Graphics
	- operators

<!-- 
####Window

####
	- events
		-

 -->

####ALMs

The ALMs provided by legacy AppWare are:

Application
Browser
Bundle
Button
Calc
Cursor
Date
File
Fileops
General
Group
Loop
Menu
Menubar
Menuitem
Notify
Picture
Radgroup
Sub
Text
Textfact
Window



	


	



