#pragma once

#include "graphics/gl.hpp"
#include "graphics/primitives/Primitives.hpp"

namespace PrimitiveAssembler
{
  void AssemblePoints(PrimitiveBuffer &primitives, size_t indicesCount, const int *indices);

  void AssembleLines(PrimitiveBuffer &primitives, size_t indicesCount, const int *indices);
  void AssembleLineLoop(PrimitiveBuffer &primitives, size_t indicesCount, const int *indices);
  void AssembleLineStrip(PrimitiveBuffer &primitives, size_t indicesCount, const int *indices);

  void AssembleTriangles(PrimitiveBuffer &primitives, size_t indicesCount, const int *indices);
  void AssembleTriangleStrip(PrimitiveBuffer &primitives, size_t indicesCount, const int *indices);
  void AssembleTriangleFan(PrimitiveBuffer &primitives, size_t indicesCount, const int *indices);

  void AssemblePrimitive(gl::RenderMode mode, PrimitiveBuffer &primitives, size_t indicesCount, const int *indices);
}