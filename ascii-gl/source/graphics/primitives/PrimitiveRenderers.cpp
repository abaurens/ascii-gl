#include "PrimitiveRenderers.hpp"
#include "Dialogs.hpp"

#include "core/Log.hpp"
#include "graphics/Context.hpp"

#include <glm/glm.hpp>

// Draw a line using the Bresenham algorithm
//template<class Func>
void DrawLine(FrameBuffer &framebuffer, glm::vec2 p1, glm::vec2 p2, uint32_t color)
{
  p1.x = float((int)p1.x);
  p1.y = float((int)p1.y);
  p2.x = float((int)p2.x);
  p2.y = float((int)p2.y);

  const float length = glm::length(p2 - p1);

  const glm::ivec2 dir = {
    (p1.x < p2.x ? 1 : -1),
    (p1.y < p2.y ? 1 : -1)
  };

  const glm::vec2  delta = {
     abs(p1.x - p2.x),
    -abs(p1.y - p2.y)
  };

  int err = int(delta.x + delta.y);
  int err2;

  while (true)
  {
    LOG_DEBUG("  Drawing Line: {{ {}, {} }}", (size_t)p1.x, (size_t)p1.y);
    framebuffer.SetPixel((size_t)p1.x, (size_t)p1.y, color);

    if (p1 == p2)
      break;

    err2 = err * 2;
    if (err2 >= delta.y)
    {
      err += (int)delta.y;
      p1.x += dir.x;
    }
    if (err2 <= delta.x)
    {
      err += (int)delta.x;
      p1.y += dir.y;
    }
  }
}

namespace PrimitiveRenderer
{
  void RenderPoint(FrameBuffer &framebuffer, const glm::vec4 *geometryBuffer, const Point &point)
  {
    const glm::vec4 &pos = geometryBuffer[point.indices[0]];

    LOG_TRACE("  Drawing Point: {{ {:5.2}, {:5.2}, {:5.2}, {:5.2} }}", pos.x, pos.y, pos.z, pos.w);
    framebuffer.SetPixel((size_t)pos.x, (size_t)pos.y, 0xffffffff);
  }

  void RenderLine(FrameBuffer &framebuffer, const glm::vec4 *geometryBuffer, const Line &line)
  {
    glm::vec2 p1 = geometryBuffer[line.indices[0]];
    glm::vec2 p2 = geometryBuffer[line.indices[1]];

    LOG_TRACE("  Drawing Line: [\n"
      "    {{ {:5.2}, {:5.2} }}\n"
      "    {{ {:5.2}, {:5.2} }}\n"
      "  ]",
      p1.x, p1.y,
      p2.x, p2.y
    );
    DrawLine(framebuffer, p1, p2, 0xffffffff);
  }

  void RenderTriangle(FrameBuffer &framebuffer, const glm::vec4 *geometryBuffer, const Triangle &triangle)
  {
    const glm::vec4 &p1 = geometryBuffer[triangle.indices[0]];
    const glm::vec4 &p2 = geometryBuffer[triangle.indices[1]];
    const glm::vec4 &p3 = geometryBuffer[triangle.indices[2]];

    LOG_TRACE("  Drawing Triangle: [\n"
      "    {{ {:5.2}, {:5.2}, {:5.2}, {:5.2} }}"
      "    {{ {:5.2}, {:5.2}, {:5.2}, {:5.2} }}"
      "    {{ {:5.2}, {:5.2}, {:5.2}, {:5.2} }}"
      "  ]",
      p1.x, p1.y, p1.z, p1.w,
      p2.x, p2.y, p2.z, p2.w,
      p3.x, p3.y, p3.z, p3.w
    );
  }

  void RenderPrimitive(FrameBuffer &framebuffer, const glm::vec4 *geometryBuffer, const IPrimitive &primitive)
  {
    switch (primitive.vertexCount)
    {
    case 1:
      return RenderPoint(framebuffer, geometryBuffer, static_cast<const Point &>(primitive));
    case 2:
      return RenderLine(framebuffer, geometryBuffer, static_cast<const Line &>(primitive));
    case 3:
      return RenderTriangle(framebuffer, geometryBuffer, static_cast<const Triangle &>(primitive));
    default:
      LOG_CRITICAL("Critical error: Unsupported primitive type encountered !");
      dial::Critical("Unsupported primitive type !");
    }
  }

  void RenderPrimitives(const PrimitiveBuffer &primitives)
  {
    Context &context = *Context::Instance();

    FrameBuffer &framebuffer = context.GetFrameBuffer();
    const glm::vec4 *geometryBuffer = context.GetGeometryBuffer().data();

    for (const IPrimitive &primitive : primitives)
    {
      RenderPrimitive(framebuffer, geometryBuffer, primitive);
    }
  }
}