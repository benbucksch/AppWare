
#ifndef __AW_ARGS__
#define __AW_ARGS__

#include "awvalue.h"
#include "awarray.h"

#include <list> // for initializer_list

namespace aw
{
	
	class Arguments : Array<Value>
	{
	public:
		
		Arguments(){}

		// construct from initializer_list
		Arguments( std::initializer_list<Value> list )
		{
			std::copy(list.begin(),list.end(),this->data_);
		}

	private:
	};


}


#endif