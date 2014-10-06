
#ifndef __AW_FUNCTION__
#define __AW_FUNCTION__

namespace aw
{

	/*
	//////////////////////////////////////////////////////////////////////////////////////////

	variadic template class for wrapping any generic function

	TODO - function should always returns Value, not RType

	//////////////////////////////////////////////////////////////////////////////////////////
	*/


	template<class> class Function; // undefined, this is required for the partial specialization below

	template<class RType, class... ArgType> class Function<RType(ArgType...)>
	{
	public:

		// operator=
		// operator&=

		Function() {}
		Function( RType func(ArgType... args) ) : func_(func) {}

		RType operator() (ArgType... args)
		{
			return func_(args...);
		}

	private:
		RType (*func_)(ArgType...); // function pointer
	};


	// class Function : public Value
	// {
	// public:
	// 	Function()
	// 	{

	// 	}

	// private:

	// };
	
}

#endif