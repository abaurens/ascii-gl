#pragma once

#include "graphics/gl.hpp"
#include "graphics/primitives/Primitives.hpp"

namespace PrimitiveProcessor
{
  PrimitiveBuffer::iterator ProcessPoint(std::vector<glm::vec4> &geometryBuffer, PrimitiveBuffer &primitives, PrimitiveBuffer::piterator<Point> point);
  PrimitiveBuffer::iterator ProcessLine(std::vector<glm::vec4> &geometryBuffer, PrimitiveBuffer &primitives, PrimitiveBuffer::piterator<Line> line);
  PrimitiveBuffer::iterator ProcessTriangle(std::vector<glm::vec4> &geometryBuffer, PrimitiveBuffer &primitives, PrimitiveBuffer::piterator<Triangle> triangle);

  PrimitiveBuffer::iterator ProcessPrimitive(std::vector<glm::vec4> &geometryBuffer, PrimitiveBuffer &primitives, PrimitiveBuffer::iterator primitive);

  void ProcessPrimitives(gl::RenderMode mode, PrimitiveBuffer &primitives);
}