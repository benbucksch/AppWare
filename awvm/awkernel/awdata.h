#ifndef __AW_DATA__
#define __AW_DATA__	

#include <stdint.h>
#include <stdlib.h>

namespace aw 
{

	/*
	//////////////////////////////////////////////////////////////////////////////////////////

	Abstract Base class for generic AppWare data types

	//////////////////////////////////////////////////////////////////////////////////////////
	*/
	class Data
	{
	public:
	protected:
		
		/**
		 *
		 * constructor is protected so that Data can't be instantiated
		 *
		 * isUndefined == true by default
		 */
		Data() : isUndefined_(true), isNull_(false) {}

		const bool isUndefined() const { return isUndefined_; }
		const bool isNull() const { return isNull_; }

	private:
		bool isUndefined_;
		bool isNull_;
	};

}

#endif