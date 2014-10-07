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

data structues for AWVM Objects

//////////////////////////////////////////////////////////////////////////////////////////
*/

#ifndef __AW_OBJECT__
#define __AW_OBJECT__

#include "awargs.h"
#include "awmap.h"
#include "awvalue.h"
#include "awfunction.h"

namespace aw
{

	/*
	//////////////////////////////////////////////////////////////////////////////////////////

	Object

	//////////////////////////////////////////////////////////////////////////////////////////
	*/

	class Object
	{
	public:
		Object();
		~Object();
		
	};






	template<class T>
	class ObjectWrap
	{
	public:

		typedef ObjectWrap self_type;
		typedef T value_type;
		typedef T& reference;
		typedef T* pointer;

		// iterate over keys ************************************
		// TODO

		ObjectWrap( pointer object ) : handle_(object) {}
		ObjectWrap() : handle_() {}


		template <class RType, class ... ArgType>
		void bindMethod(const char*, Function<RType(ArgType...)>& method)
		{

		}
		// object.bindProperty(const char*,T property);

		// bind
		// bindMethod
		// bindProperty
		// cast

	private:
		pointer handle_;
	};



	/*
	//////////////////////////////////////////////////////////////////////////////////////////

	Template for an AppWare class

	//////////////////////////////////////////////////////////////////////////////////////////
	*/
	class ClassTemplate
	{
	public:
		ClassTemplate()
		{

		}

		~ClassTemplate()
		{

		}

		// ClassTemplate::setProperty("propertyName",Virtual<Object>);
		// ClassTemplate::setMethod("methodName",Function<Rtype,ArgType...>);
		// ClassTemplate::rmProperty("propertyName");
		// ClassTemplate::rmMethod("methodName");
		// ClassTemplate::setConstructor("",Function<Rtype,ArgType...>);

		void defConstructor( const char* name )

		// template<class ArgType...>
		// void defConstructor( const char* name, const Function )
		{

		}

		void defMethod( const char* name )
		{

		}

		void defProperty( const char* name )
		{
			
		}

		Object create( const Arguments& args ) { return Object(); }
		Object create() { return Object(); }

	private:
		
	};

}


#endif