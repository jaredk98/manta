#pragma once

#include <types.hpp>
#include <debug.hpp>
#include <vendor/new.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Notes: Untyped
// Used to reduce code generated of templated Lists as the member functions can be
// defined in a single translation unit

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ilist
{
	struct List
	{
		// Default Constructor
		List( const usize stride ) : stride( stride ) { }

		// Data & State
		byte *data = nullptr;
		usize stride = 0;
		usize current = 0;
		usize capacity = 0;

		// Memory Handling
		void init( const usize reserve );
		void free();
		void grow();
		void copy( const List &other );
		void move( List &&other );

		// List API
		byte *add_at();
		byte *insert_at( const usize index );
		byte *remove_at( const usize index );
		byte *remove_swap_at( const usize index );
		inline byte *at( const usize index ) { Assert( data != nullptr ); Assert( index < current ); return data + index * stride; }
		inline const byte *const at( const usize index ) const { Assert( data != nullptr ); Assert( index < current ); return data + index * stride; }
		inline usize size() { Assert( data != nullptr ); return current; }
		inline byte *begin() { return data; }
		inline byte *end() { return data + current * stride; }
	};
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
class List
{
public:
	// Memory Handling
	inline void init( const usize reserve = 1 )
	{
		list.init( reserve );
	}

	inline void free()
	{
		if( list.data == nullptr ) { return; }
		free_elements();
		list.free();
	}

	List<T> &copy( const List<T> &other )
	{
		if( list.data != nullptr ) { free_elements(); }
		list.copy( other.list );
		return *this;
	}

	List<T> &move( List<T> &&other )
	{
		if( list.data != nullptr ) { free_elements(); }
		list.move( static_cast<ilist::List &&>( other.list ) );
		return *this;
	}

	// API
	inline T &at( const usize index ) { return *reinterpret_cast<T *>( list.at( index ) ); }
	inline const T &at( const usize index ) const { return *reinterpret_cast<const T *>( list.at( index ) ); }

	inline T &add( const T &element )
	{
		byte * const ptr = list.add_at();
		new ( ptr ) T( element );
		return *reinterpret_cast<T *>( ptr );
	}

	inline T &add( T &&element )
	{
		byte * const ptr = list.add_at();
		new ( ptr ) T( static_cast<T &&>( element ) );
		return *reinterpret_cast<T *>( ptr );
	}

	inline T &insert( const usize index, const T &element )
	{
		byte * const ptr = list.insert_at( index );
		new ( ptr ) T( element );
		return *reinterpret_cast<T *>( ptr );
	}

	inline T &insert( const usize index, T &&element )
	{
		byte * const ptr = list.insert_at( index );
		new ( ptr ) T( static_cast<T &&>( element ) );
		return *reinterpret_cast<T *>( ptr );
	}

	inline void remove( const usize index ) { list.remove_at( index ); }
	inline void remove_swap( const usize index ) { list.remove_swap_at( index ); }

	inline usize size() { return list.current; }

	inline void clear()
	{
		if( list.data != nullptr ) { free_elements(); }
		list.current = 0;
	}

	// Indexer
	inline T &operator[]( const usize index ) { return at( index ); }
	inline const T &operator[]( const usize index ) const { return at( index ); }

	// Forward Iterator
	struct forward_iterator
	{
		forward_iterator( T *ptr ) : ptr( ptr ) { }

		T &operator*() { return *ptr; }
		forward_iterator &operator++() { ptr++; return *this; }
		bool operator!=( const forward_iterator &other ) const { return ptr != other.ptr; }

		T *ptr;
	};

	forward_iterator begin() { return forward_iterator( reinterpret_cast<T *>( list.begin() ) ); }
	forward_iterator end() { return forward_iterator( reinterpret_cast<T *>( list.end() ) ); }

	// Reverse Iterator
	struct reverse_iterator
	{
		reverse_iterator( T *ptr ) : ptr( ptr ) { }

		T &operator*() { return *ptr; }
		reverse_iterator &operator++() { ptr--; return *this; }
		bool operator!=( const reverse_iterator &other ) const { return ptr != other.ptr; }

		T *ptr;
	};

	reverse_iterator rbegin() { return reverse_iterator( reinterpret_cast<T *>( list.begin() ) ); }
	reverse_iterator rend() { return reverse_iterator( reinterpret_cast<T *>( list.end() ) ); }

	// Memory Leak Detection
	#if COMPILE_DEBUG
	~List()
	{
		if( error::code == 0 ) { AssertMsg( list.data == nullptr, "ERROR: Memory leak in unscoped List (%p) (size: %f kb)", this, list.capacity * sizeof( T ) / 1024.0f ); }
	}
	#endif

private:
	// Data
	ilist::List list { sizeof( T ) };

	// Cleanup
	inline void free_elements()
	{
		for( usize i = 0; i < list.current; i++ ) { reinterpret_cast<T *>( list.at( i ) )->~T(); }
	}
};