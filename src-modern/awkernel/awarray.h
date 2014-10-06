#ifndef __AW_ARRAY__
#define __AW_ARRAY__


namespace aw
{

	// template <class T>
	// class Array : public Container<T>
	// {
	// public:

	// 	typedef T value_type;
	// 	typedef T& reference;
	// 	typedef T* pointer;

	// 	// return const
	// 	const value_type operator[]( size_t i ) const
	// 	{
	// 		if ( i < this->size_ )
	// 			return this->data_[i];
	// 		else
	// 			return (const value_type)value_type();
	// 	}

	// 	// return reference
	// 	reference operator[]( size_t i )
	// 	{
	// 		if ( i < this->size_ )
	// 			return this->data_[i];
	// 		else {
	// 			this->resize(i);
	// 			for (size_t j = 0; j < this->size_; ++j)
	// 				this->data_[i] = value_type();
	// 			return this->data_[i];
	// 		}
	// 	}
	// };




		// typedef std::pair<Handle<Value> > __keyValuePair__;

	// key value store
	// class KeyValueArray : public Array< __keyValuePair__ >
	// {
	// public:

	// 	// KeyValueArray( const Array& array )
	// 	// KeyValueArray( const Array& array )

	// 	typedef __keyValuePair__ value_type;
	// 	typedef const char* key_type;
	// 	typedef Handle<Value> mapped_type;

	// 	// return const
	// 	const mapped_type operator[]( key_type field ) const
	// 	{
	// 		return mapped_type();
	// 	}

	// 	// Handle<Value>& operator[]( size_t i )
	// 	// {
	// 	// 	return Handle<Value>();
	// 	// }

	// private:
	// 	value_type* data_;
	// };

}

#endif