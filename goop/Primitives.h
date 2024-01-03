#pragma once

#include <variant>
namespace goop
{
struct Box
{
	float x;
	float y;
	float z;

	Box(float x, float y, float z) : x(x), y(y), z(z) {}
	Box() : x(1), y(1), z(1) {}
};

struct Sphere
{
	float radius;
	float resolution;
};

#define PRIMITIVES Box, Sphere

using Primitive = std::variant<std::monostate, PRIMITIVES>;
} // namespace goop
