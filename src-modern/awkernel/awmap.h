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

Associative Array data structure

//////////////////////////////////////////////////////////////////////////////////////////
*/

#ifndef __AW_MAP__
#define __AW_MAP__	

#define TRIE_ALPHABET_SIZE 52 // each Trie has 52 children (26 lowercase + 26 uppercase)

#include <stdint.h>
#include <string.h> // for strlen

namespace aw {

	/*
	//////////////////////////////////////////////////////////////////////////////////////////

	Node in a Radix Tree

	//////////////////////////////////////////////////////////////////////////////////////////
	*/
	template <class T>
	class RadixTreeNode
	{
	public:

		typedef RadixTreeNode self_type;
		typedef T value_type;
		typedef T& reference;
		typedef T* pointer;

		// default constructor 
		RadixTreeNode() : isKey_(false), isRoot_(false), value_(NULL), nodes_()
		{}

		// construct from a string
		RadixTreeNode( const char* key ) : RadixTreeNode()
		{
			this->addChild(key);
		}

		/**
		 *
		 * TODO generalize for 'key_type' instead of 'const char*'
		 * TODO this could be operator[]
		 *
		 * :return - returns the located node
		 */
		self_type* getNode ( const char* key )
		{
			self_type* node = this;
			for ( size_t i = 0; i < strlen(key); i++ )
			{
				self_type* child = node->childAt(alphabet_index(key[i]));
				if ( !child ) return NULL;
				node = child;
			}
			return node;
		}

		/**
		 *
		 * add a new node with the given key and value to the radix tree
		 *
		 * :return - returns the added node
		 */
		self_type* addChild( const char* key )
		{
			self_type* node = this;
			for ( size_t i = 0; i < strlen(key); i++ )
			{
				self_type* child = node->childAt(alphabet_index(key[i]));
				if ( !child ) child = new self_type();
				node = child;
			}
			node->setValue(new value_type());
			node->isKey(true);
			return node;
		}

		pointer setValue( value_type* value ) { value_ = value; return value_; }
		pointer getValue() { return value_; }
		const bool isKey () const { return isKey_; }
		const bool isKey ( const bool truth ) { isKey_ = truth; return isKey_; }
		const bool isRoot () const { return isRoot_; }
		self_type* childAt( size_t index ) const { return nodes_[index]; }

	private:

		/**
		 *
		 * return the index of the letter in the standard english alphabet
		 * a-z => 0-25, A-Z => 26-51
		 *
		 */
		static const size_t alphabet_index( const char letter )
		{
			size_t a = (size_t)letter;
			return a < 97 ? 26 + (a - 65) : a - 97;
		}

		bool isKey_;
		bool isRoot_; // TODO
		value_type* value_;
		self_type* nodes_[TRIE_ALPHABET_SIZE];
		
	};



	/*
	//////////////////////////////////////////////////////////////////////////////////////////

	Radix Tree (Trie) data structure

	- a container of templated RadixTreeNodes

	//////////////////////////////////////////////////////////////////////////////////////////
	*/
	template<class T>
	class RadixTree
	{
	public:

		typedef const char* key_type;
		typedef T value_type;
		typedef T& reference;
		typedef T* pointer;

		RadixTree()
		{
			// TODO
		}

		// 
		const value_type operator[]( key_type field ) const
		{
			RadixTreeNode<value_type>* node = root_.getNode( field );
			return node ? *node->value() : value_type();
		}

		reference operator[]( key_type field )
		{
			RadixTreeNode<value_type>* node = root_.getNode( field );
			if ( !node ) // there is no field by that name, so create one
				node = root_.addChild( field );
			return *node->getValue();
		}

	private:
		RadixTreeNode<value_type> root_;
	};


	/*
	//////////////////////////////////////////////////////////////////////////////////////////

	Map (Associative Array) data structure, implementing a Radix Tree

	//////////////////////////////////////////////////////////////////////////////////////////
	*/
	template<class T>
	class Map : public RadixTree<T>
	{
	public:

		Map() : size_(0) {}

		// Map( Array array ) : size_(0) {} // TODO

		typedef const char* key_type;
		typedef T value_type;

	private:
		size_t size_; // TODO
	};

}

#endif