#pragma once

#include "graphics/gl.hpp"
#include "graphics/primitives/Primitives.hpp"

namespace PrimitiveProcessor
{
  void ProcessPoints(PrimitiveBuffer &primitives, size_t indicesCount, const int *indices);
  void ProcessLines(PrimitiveBuffer &primitives, size_t indicesCount, const int *indices);
  void ProcessTriangles(PrimitiveBuffer &primitives, size_t indicesCount, const int *indices);

  void ProcessPrimitive(gl::RenderMode mode, PrimitiveBuffer &primitives, size_t indicesCount, const int *indices);
}