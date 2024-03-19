#pragma once

#include <debug.hpp>
#include <traits.hpp>

#include <manta/memory.hpp>

#include <vendor/vendor.hpp>
#include <vendor/new.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define HASHMAP_LOAD_FACTOR 0.75

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Types used as keys need to have the following functions:
//
// u32 hash( T key );
// bool equals( T a, T b );
// bool is_null( T key );
// bool set_null( T &key );
//
// See traits.hpp

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename K, typename V>
struct HashMap
{
	struct KeyValue
	{
		K key;
		V value;
	};

	void init( const u32 reserve, const V defaultValue )
	{
		Assert( reserve != 0 );
		capacity = align_pow2( reserve );
		Assert( data == nullptr );
		data = reinterpret_cast<KeyValue *>( memory_alloc( capacity * sizeof( KeyValue ) ) );
		ErrorIf( data == nullptr, "Failed to allocate memory for HashMap" );
		size = 0;

		this->defaultValue = defaultValue;
		for( u32 i = 0; i < capacity; i++ )
		{
			KeyValue &keyValue = data[i];
			set_null( keyValue.key );
			keyValue.value = defaultValue;
		}
	}

	void init( const u32 reserve = 32 )
	{
		// No default value for KeyValue pairs
		init( reserve, this->defaultValue );
	}

	void free()
	{
		Assert( data != nullptr );
		memory_free( data );
		data = nullptr;
	}

	void copy( const HashMap<K, V> &other )
	{
		Assert( other.data != nullptr );
		if( data != nullptr ) { free(); }
		init( other.capacity );
		defaultValue = other.defaultValue;
		capacity = other.capacity;
		size = other.stride;
		memory_copy( data, other.data, capacity * sizeof( KeyValue ) );
	}

	void move( HashMap<K, V> &&other )
	{
		if( data != nullptr ) { free(); }
		data = other.data;
		defaultValue = other.defaultValue;
		capacity = other.capacity;
		size = other.stride;
		other.data = nullptr;
		other.capacity = 0;
		other.size = 0;
	}

	void grow()
	{
		ErrorIf( ( static_cast<u64>( capacity ) << 1 ) > U32_MAX, "Exceeded maximum HashTable size" );

		u32 capacityOld = capacity;
		KeyValue *dataOld = data;

		capacity <<= 1;
		data = reinterpret_cast<KeyValue *>( memory_alloc( capacity * sizeof( KeyValue ) ) );
		ErrorIf( data == nullptr, "Failed to allocate memory to grow HashMap" );
		for( u32 i = 0; i < capacity; i++ ) { set_null( data[i].key ); }

		// Rehash keys from the old table
		for( u32 i = 0; i < capacityOld; i++ )
		{
			// Skip null Buckets
			KeyValue &src = dataOld[i];
			if( is_null( src.key ) ) { continue; }

			KeyValue &dst = find( src.key );
			dst.key = src.key;
			dst.value = src.value;
			src.key.~K();
			src.value.~V();
		}

		// Reset old data
		memory_free( dataOld );
	}

	bool add( const K &key, const V &value )
	{
		if( UNLIKELY( size + 1 > capacity * HASHMAP_LOAD_FACTOR ) ) { grow(); }

		KeyValue &keyValue = find( key );

		if( is_null( keyValue.key ) )
		{
			keyValue.key = key;
			keyValue.value = value;
			size++;
			return true;
		}

		return false;
	}

	bool set( const K &key, const V &value )
	{
		if( UNLIKELY( size + 1 > capacity * HASHMAP_LOAD_FACTOR ) ) { grow(); }

		KeyValue &keyValue = find( key );

		bool added = is_null( keyValue.key );
		keyValue.key = key;
		keyValue.value = value;
		size += added;

		return added;
	}

	KeyValue &find( const K &key )
	{
		u32 index = hash( key ) & ( capacity - 1 );

		for( ;; )
		{
			KeyValue &keyValue = data[index];
			if( is_null( keyValue.key ) || equals( key, keyValue.key ) ) { return keyValue; }
			index = ( index + 1 ) & ( capacity - 1 );
		}
	}

	inline V &get( const K &key ) { return find( key ).value; }
	inline bool contains( const K &key ) { return !is_null( find( key ).key ); }

	V &operator[]( const K &key ) { return get( key ); }
	const V &operator[]( const K &key ) const { return get( key ); }

	struct forward_iterator
	{
		forward_iterator( KeyValue *ptr, KeyValue *end ) : end{ end } { this->ptr = find_next( ptr ); } // begin();
		forward_iterator( KeyValue *end ) : end{ end }, ptr{ end } { } // end();

		KeyValue *find_next( KeyValue *ptr )
		{
			while( ptr != end )
			{
				if( !is_null( ptr->key ) ) { break; }
				ptr++;
			}
			return ptr;
		}

		V &operator*() { return ptr->value; }
		forward_iterator &operator++() { ptr = find_next( ptr + 1 ); return *this; }
		bool operator!=( const forward_iterator &other ) const { return ptr != other.ptr; }

		KeyValue *end;
		KeyValue *ptr;
	};

	forward_iterator begin() { return forward_iterator( &data[0], &data[capacity] ); }
	forward_iterator end() { return forward_iterator( &data[capacity] ); }

private:
	V defaultValue;
	KeyValue *data = nullptr;
	u32 capacity;
	u32 size;
};