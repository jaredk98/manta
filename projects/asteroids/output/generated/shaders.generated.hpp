#pragma once

#include <types.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct DiskShader
{
	u32 offsetVertex;
	u32 sizeVertex;
	u32 offsetFragment;
	u32 sizeFragment;
	u32 offsetCompute;
	u32 sizeCompute;
};

enum
{
	SHADER_DEFAULT,
	sh_green,
};

namespace shaders
{
	constexpr u32 shadersCount = 2;
	extern const DiskShader diskShaders[];
}

