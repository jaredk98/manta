#pragma once

#pragma once

#include <types.hpp>
#include <debug.hpp>
#include <vendor/new.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ilist
{
	struct List
	{
		List( const usize stride ) : stride( stride ) { }

		void init( const usize reserve );
		void free();
		void grow();
		void copy( const List &other );
		void move( List &&other );

		byte *add_at();
		byte *insert_at( const usize index );
		byte *remove_at( const usize index );
		byte *remove_swap_at( const usize index );

		inline byte *at( const usize index ) { Assert( data != nullptr ); Assert( index < current ); return data + index * stride; }
		inline const byte *const at( const usize index ) const { Assert( data != nullptr ); Assert( index < current ); return data + index * stride; }
		inline usize size() { Assert( data != nullptr ); return current; }
		inline byte *begin() { return data; }
		inline byte *end() { return data + current * stride; }

		byte *data = nullptr;
		usize stride = 0;
		usize current = 0;
		usize capacity = 0;
	};
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
class List
{
public:
	List( const usize reserve = 1 ) { Assert( reserve > 0 ); init( reserve ); }
	List( const List<T> &other ) { copy( other ); }
	List( List<T> &&other ) { move( static_cast<List<T> &&>( other ) ); }
	~List() { free(); }

	List &operator=( const List<T> &other ) { copy( other ); return *this; }
	List &operator=( List<T> &&other ) { move( static_cast<List<T> &&>( other ) ); return *this; }

	inline T &at( const usize index ) { return *reinterpret_cast<T *>( list.at( index ) ); }
	inline const T &at( const usize index ) const { return *reinterpret_cast<const T *>( list.at( index ) ); }

	inline T &add( const T &element )
	{
		byte *ptr = list.add_at();
		new ( ptr ) T( element );
		return *reinterpret_cast<T *>( ptr );
	}

	inline T &add( T &&element )
	{
		byte *ptr = list.add_at();
		new ( ptr ) T( static_cast<T &&>( element ) );
		return *reinterpret_cast<T *>( ptr );
	}

	inline T &insert( const usize index, const T &element )
	{
		byte *ptr = list.insert_at( index );
		new ( ptr ) T( element );
		return *reinterpret_cast<T *>( ptr );
	}

	inline T &insert( const usize index, T &&element )
	{
		byte *ptr = list.insert_at( index );
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

	inline bool contains( const T &element ) const
	{
		for( usize index = 0; index < list.current; index++ ) { if( at( index ) == element ) { return true; } }
		return false;
	}

	inline T &operator[]( const usize index ) { return at( index ); }
	inline const T &operator[]( const usize index ) const { return at( index ); }

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

private:
	void init( const usize reserve = 1 )
	{
		list.init( reserve );
	}

	void free()
	{
		if( list.data == nullptr ) { return; }
		free_elements();
		list.free();
	}

	void copy( const List<T> &other )
	{
		if( list.data != nullptr ) { free_elements(); }
		list.copy( other.list );
	}

	void move( List<T> &&other )
	{
		if( list.data != nullptr ) { free_elements(); }
		list.move( static_cast<ilist::List &&>( other.list ) );
	}

	void free_elements()
	{
		for( usize i = 0; i < list.current; i++ ) { reinterpret_cast<T *>( list.at( i ) )->~T(); }
	}

	ilist::List list { sizeof( T ) };
};