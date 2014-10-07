/*
//////////////////////////////////////////////////////////////////////////////////////////

                   __          __            
     /\            \ \        / /            
    /  \   _ __  _ _\ \  /\  / /_ _ _ __ ___ 
   / /\ \ | '_ \| '_ \ \/  \/ / _` | '__/ _ \
  / ____ \| |_) | |_) \  /\  / (_| | | |  __/
 /_/    \_\ .__/| .__/ \/  \/ \__,_|_|  \___|
          | |   | |                          
          |_|   |_|    

//////////////////////////////////////////////////////////////////////////////////////////

The Objective C part of the kernel (for OSX)

//////////////////////////////////////////////////////////////////////////////////////////
*/

#ifndef __COCOA_OBJC__
#define __COCOA_OBJC__

#import "../appware.h"

#import <stdlib.h>
#import <Cocoa/Cocoa.h>

namespace aw {



	class CocoaApp
	{
	public:
		CocoaApp()
		{
			pool_ = [[NSAutoreleasePool alloc] init];
			[NSApplication sharedApplication];
			// appDelegate_ = [[AppDelegate alloc] init];
			// [NSApp setDelegate:appDelegate_];
			
		}

		~CocoaApp() { [pool_ release]; }

		void start() { [NSApp run]; }

	private:
		NSAutoreleasePool *pool_;
		// AppDelegate *appDelegate_;

	};


	Application::Application() : ptr_(new CocoaApp()) {}

	Application::~Application(){}

	void Application::start() const { ((CocoaApp*)ptr_)->start(); }
	
	// callback()

	// Application

	// typedef Function<void(self_type self)> callback;
	


	




	// class CocoaWindow
	// {
	// public:
	// 	// CocoaWindow(Rect)
	// 	CocoaWindow()
	// 	{
	// 		frame_ = NSMakeRect(0, 0, 200, 200);
	// 		window_  = [[[NSWindow alloc] initWithContentRect:frame_
	// 			styleMask:NSBorderlessWindowMask
	// 			backing:NSBackingStoreBuffered
	// 			defer:NO] autorelease];
	// 		[window_ setBackgroundColor:[NSColor blueColor]];
	// 		[window_ makeKeyAndOrderFront:NSApp];
	// 	}

	// 	~CocoaWindow()
	// 	{

	// 	}

	// private:
	// 	NSRect frame_;
	// 	NSWindow* window_;
		
	// };








}

#endif