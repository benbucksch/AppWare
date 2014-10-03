
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

Main C++ Kernel to AppWare

//////////////////////////////////////////////////////////////////////////////////////////
*/

#ifndef __AW_KERNEL__
#define __AW_KERNEL__

/*

	AppWare essential definitions

*/

#define __AW__
#define __AW_VERSION__ 0.0.1

#include <stdint.h>
#include <stdlib.h>
#include <map>


// public static NSMakeRect(float x, float y, float w, float h)




/*
//////////////////////////////////////////////////////////////////////////////////////////

the all encompassing AppWare namespace

//////////////////////////////////////////////////////////////////////////////////////////
*/

namespace aw {

	namespace kernel {


		const char* test();


	}

	/*
	//////////////////////////////////////////////////////////////////////////////////////////

	Application

	//////////////////////////////////////////////////////////////////////////////////////////
	*/
	class Application
	{
	public:
		Application() {}
		~Application() {}
		
	};

	class Window
	{
	public:
		Window() {}
		~Window() {}


		// Model
		// closed
		// width
		// height
		// fullScreen
		// scrollbars
		// scrollX
		// scrollY
		// scrollMaxX
		// scrollMaxY

		// View -- (Cocoa?)

		// Controller -- (Cocoa?)


		
	};


	/*
	//////////////////////////////////////////////////////////////////////////////////////////

	Object

	//////////////////////////////////////////////////////////////////////////////////////////
	*/
	class Object
	{
	public:
		Object()
		{

		}

		// 
		void set(const char* name, Object obj)
		{
			properties_[name] = obj;
		}

		// cast

	private:
		std::map<const char*,Object> properties_;
	};



	/*
	//////////////////////////////////////////////////////////////////////////////////////////

	wrap Objective C functions and present them to C++
	or...
	wrap C++ functions and present them to V8


	variadic template class for wrapping any generic function

	//////////////////////////////////////////////////////////////////////////////////////////
	*/

	template <class RType, class... ArgType>
	class Function : public Object
	{
	public:

		Function( RType func(ArgType... args) ) : func_(func) {}

		RType operator() (ArgType... args)
		{
			return func_(args...);
		}

	private:
		RType (*func_)(ArgType...); // function pointer
	};



	/*
	//////////////////////////////////////////////////////////////////////////////////////////

	wrap Objective C Objects

	variadic template class for wrapping any generic object

	//////////////////////////////////////////////////////////////////////////////////////////
	*/

	template <class ObjType>
	class WrappedObject
	{
	public:

		WrappedObject(ObjType obj) : obj_(obj)
		{

		}

	private:
		ObjType* obj_;
	};

	// class Function
	// {
	// public:
	// 	Function();
		
	// 	template <class RType, class... ArgType>
	// 	void wrap(FunctionWrap<RType, ArgType...> func)
	// 	{

	// 	}
		
	// };




	// aw::Application : public aw::Object()
	// aw::Window : public aw::Object()

}




/*


aw::Kernel
aw::System
	io/read/write
aw::Application
	start
	end

aw::Window
aw::Event
	- trigger?


*/






#endif