AppWare
====

- framework for building platform-agnostic apps that work anywhere
- visual programming, WYSIWYG layout builder
- create once and use anywhere, on any platform.
- cloud integration?


UniNav/AppWare

####Browser-like:
A browser alternative that navigates websites in a 3D context, perhaps aided by 3D interaction with RealSense.
Precaution: this could just become a silly gimmick.

- __Platform__ : 
	- use [Chromium Embedded Framework (CEF)](https://code.google.com/p/chromiumembedded/) to create a custom drawing framework on top of the Blink rendering engine in Chromium.
	- or use node-webkit like Breach Browser. Scriptble in Javascript, but with potential for C++ extras?
- __Distribution__ : browser-like executable that could potentially run on desktop and mobile.

####Plugin/Frame
A browser plugin, or portal/frame that takes normal websites and allows html elements to make normal website elements resizeable, draggable, stackable, etc.

- __Platform__ : websites are viewed through an iFrame
- __Distribution__ : mobile browsers don't support plugins.

