#ifndef __AW_ARRAY__
#define __AW_ARRAY__

#include "awcontainer.h"

#include <stdlib.h>
#include <stdint.h>

namespace aw
{


	/*
	//////////////////////////////////////////////////////////////////////////////////////////

	extend Container with array operators 

	//////////////////////////////////////////////////////////////////////////////////////////
	*/

	template <class T>
	class Array : public Container<T>
	{
	public:

		typedef T value_type;
		typedef T& reference;
		typedef T* pointer;

		// return const
		const value_type operator[]( size_t i ) const
		{
			if ( i < this->size_ )
				return this->data_[i];
			else
				return (const value_type)value_type();
		}

		// return reference
		reference operator[]( size_t i )
		{
			if ( i < this->size_ )
				return this->data_[i];
			else {
				this->resize(i);
				for (size_t j = 0; j < this->size_; ++j)
					this->data_[i] = value_type();
				return this->data_[i];
			}
		}
	};

}

#endif