#pragma once
#include <vendor/config.hpp>

#include <types.hpp>

extern u32 checksum_xcrc32( const char *buffer, usize size, u32 seed );
extern u32 checksum_xcrc32( const char *buffer, const usize offset, int size, u32 seed );
