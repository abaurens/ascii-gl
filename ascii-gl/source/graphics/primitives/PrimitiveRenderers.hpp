#pragma once

#include "graphics/gl.hpp"
#include "graphics/FrameBuffer.hpp"
#include "graphics/primitives/Primitives.hpp"

namespace PrimitiveRenderer
{
  void RenderPoint(FrameBuffer &framebuffer, const glm::vec4 *geometryBuffer, const Point &primitive);
  void RenderLine(FrameBuffer &framebuffer, const glm::vec4 *geometryBuffer, const Line &primitive);
  void RenderTriangle(FrameBuffer &framebuffer, const glm::vec4 *geometryBuffer, const Triangle &primitive);

  void RenderPrimitive(FrameBuffer &framebuffer, const glm::vec4 *geometryBuffer, const IPrimitive &primitive);

  void RenderPrimitives(const PrimitiveBuffer &primitives);
}