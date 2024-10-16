#include "PrimitiveProcessors.hpp"
#include "graphics/Context.hpp"

#include "core/Log.hpp"

#include <frozen/map.h>

namespace PrimitiveProcessor
{

  using ProcessFunction = void (*)(PrimitiveBuffer &, size_t, const int *);

  static constexpr frozen::map<gl::RenderMode, ProcessFunction, 7> functions = {
    { gl::POINTS, ProcessPoints },            // 0b0000

    { gl::LINES,      ProcessLines },         // 0b0001
    { gl::LINE_LOOP,  ProcessLines },         // 0b0010
    { gl::LINE_STRIP, ProcessLines },         // 0b0011

    { gl::TRIANGLES,      ProcessTriangles }, // 0b0100
    { gl::TRIANGLE_STRIP, ProcessTriangles }, // 0b0101
    { gl::TRIANGLE_FAN,   ProcessTriangles }, // 0b0110
  };

  void ProcessPoints(PrimitiveBuffer &primitives, size_t indicesCount, const int *indices)
  {
    PrimitiveBuffer::piterator<Point> it = primitives.fixed<Point>().begin();

    const glm::vec4 *geometryBuffer = Context::Instance()->GetGeometryBuffer().data();

    while (it != primitives.end())
    {
      const glm::vec4 &pos = geometryBuffer[it->indices[0]];

      if (pos.x < -pos.w || pos.x >= pos.w || pos.y < -pos.w || pos.y >= pos.w)
      {
        LOG_TRACE("  Point = {{ {0:5.2}, {1:5.2}, {2:5.2}, {3:5.2} }} [CLIPED]", pos.x, pos.y, pos.z, pos.w);
        it = primitives.Erase(it);
        continue;
      }

      LOG_TRACE("  Point = {{ {0:5.2}, {1:5.2}, {2:5.2}, {3:5.2} }}", pos.x, pos.y, pos.z, pos.w);
      ++it;
    }
  }

  void ProcessLines(PrimitiveBuffer &primitives, size_t indicesCount, const int *indices)
  {
  }

  void ProcessTriangles(PrimitiveBuffer &primitives, size_t indicesCount, const int *indices)
  {
  }

  void ProcessPrimitive(gl::RenderMode mode, PrimitiveBuffer &primitives, size_t indicesCount, const int *indices)
  {
    functions.at(mode)(primitives, indicesCount, indices);
  }
}

