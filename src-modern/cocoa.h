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

#import "appware.h"

#import <stdlib.h>
#import <Cocoa/Cocoa.h>

namespace aw {

	class Proxy
	{
	public:
		Proxy()
		{

		}

		~Proxy()
		{

		}
	};
	
	class Window : Proxy
	{
	public:
		Window();
	};

	// window.set()




}

#endif