AppWare Virtual Machine
====


### AppWare Virtual Machine

AWVM is an experimental new kernel for AppWare. 

The idea is to provide an interface between system-specific operations (eg, Cocoa and .NET) and Node.js.

####Kernel

- the kernel provides a platform agnostic interface to low-level core operations that tend to be platform specific. __Anything that calls a native function must be part of the kernel__.
- eg, the kernel provides a single interface to GUI operations in C++ (for Windows) and Objective C (for OSX)

	Application
	Window
	GUI
	Events

####Engine

- the engine is the interface between the kernel and the front end. The engine is mostly the translation layer between C++ and Node.js.
- written in V8 (C++)

####SDK

- written in Node.js
- the runtime is available to the user as a series of API tools

####Application

- the front end GUI application to AppWare
- written in Node.js using the AppWare Javascript SDK.

####ALMs (AppWare Loadable Modules)

- ALMs are 'plugins' for Appware that can introduce extra functionality and tools
- these are basically just Node.js modules


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



	


	



<!-- 

Types

Number
Map
Object
Function
Array

Object extends Map


 -->