
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

		// 
		// void set(const char* name, Object obj)
		// {
		// 	properties_[name] = obj;
		// }

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

		Object init ( const Arguments& args )
		{
			return Object();
		}

	private:
		
	};

}


#endif