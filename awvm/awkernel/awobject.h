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

namespace aw
{

	/*
	//////////////////////////////////////////////////////////////////////////////////////////

	Object

	inherits 

	//////////////////////////////////////////////////////////////////////////////////////////
	*/
	class Object : public Map<Value>
	{
	public:

		typedef Object self_type;
		typedef Value value_type;
		typedef value_type& reference;
		typedef value_type* pointer;

		// iterate over keys ************************************
		// TODO

		// cast

	private:
		// std::map<const char*,Object> properties_;
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