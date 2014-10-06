
#ifndef __AW_CONTAINER__
#define __AW_CONTAINER__

#include "awtypes.h"

#include <stdint.h>
#include <stdlib.h>

namespace aw {


	/*
	//////////////////////////////////////////////////////////////////////////////////////////

	Basic container base class

	//////////////////////////////////////////////////////////////////////////////////////////
	*/
	
	template <class T>
	class Container : public Value
	{
	public:

		typedef T value_type;
		typedef T& reference;
		typedef T* pointer;

		Container() : size_(0), capacity_(0) {}

		void resize( size_t newsize )
		{
			size_ = newsize;
			capacity_ = size_;
			data_ = (value_type*)malloc( capacity_ * sizeof(value_type) );
		}

		size_t capacity() { return capacity_; }
		size_t size() { return size_; }

	protected:
		size_t size_;
		size_t capacity_;
		value_type* data_;
		
	};


}

#endif