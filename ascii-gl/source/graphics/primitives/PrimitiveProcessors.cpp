#include "PrimitiveProcessors.hpp"
#include "Dialogs.hpp"

#include "core/Log.hpp"
#include "core/Core.hpp"
#include "graphics/Context.hpp"

#include <cstdint>

using iter = PrimitiveBuffer::iterator;
template<class Primitive>
using piter = PrimitiveBuffer::piterator<Primitive>;

constexpr uint8_t CENTER_REGION = 0;
constexpr uint8_t NEAR_REGION = BIT(0);
constexpr uint8_t FAR_REGION = BIT(1);
constexpr uint8_t TOP_REGION = BIT(2);
constexpr uint8_t DOWN_REGION = BIT(3);
constexpr uint8_t RIGHT_REGION = BIT(4);
constexpr uint8_t LEFT_REGION = BIT(5);

constexpr uint8_t GetRegions(const glm::vec4 &pos)
{
  uint8_t regions = CENTER_REGION;

  if (pos.x < -pos.w)
    regions |= LEFT_REGION;
  else if (pos.x >= pos.w)
    regions |= RIGHT_REGION;

  if (pos.y < -pos.w)
    regions |= DOWN_REGION;
  else if (pos.y >= pos.w)
    regions |= TOP_REGION;

  if (pos.z < -pos.w)
    regions |= FAR_REGION;
  else if (pos.z >= pos.w)
    regions |= NEAR_REGION;

  return regions;
};

namespace PrimitiveProcessor
{
  // Removes points that are outside of the clipping volume
  iter ProcessPoint(std::vector<glm::vec4> &geometryBuffer, PrimitiveBuffer &primitives, piter<Point> point)
  {
    const glm::vec4 &pos = geometryBuffer[point->indices[0]];

    if (GetRegions(pos) != CENTER_REGION)
    {
      LOG_TRACE("  Point = {{ {:5.2}, {:5.2}, {:5.2}, {:5.2} }} [PRUNED]", pos.x, pos.y, pos.z, pos.w);
      return primitives.Erase(point);
    }

    LOG_TRACE("  Point = {{ {:5.2}, {:5.2}, {:5.2}, {:5.2} }}", pos.x, pos.y, pos.z, pos.w);
    return point.next();
  }

  // Clip the line inside of the clipping volume using 3D versions of the following algorithms:
  // Cohen–Sutherland algorithm (for complete line clipping and early accepting)
  //   https://en.wikipedia.org/wiki/Cohen%E2%80%93Sutherland_algorithm
  //   https://www.mdpi.com/1999-4893/16/4/201
  //
  // 
  iter ProcessLine(std::vector<glm::vec4> &geometryBuffer, PrimitiveBuffer &primitives, piter<Line> line)
  {
    glm::vec4 p1 = geometryBuffer[line->indices[0]];
    glm::vec4 p2 = geometryBuffer[line->indices[1]];

    const uint8_t p1_regions = GetRegions(p1);
    const uint8_t p2_regions = GetRegions(p2);

    if (p1_regions & p2_regions) // line is entirely outside of the screen
    {
      LOG_TRACE("  Line = [\n"
        "    {{ {:5.2}, {:5.2}, {:5.2}, {:5.2} }}\n"
        "    {{ {:5.2}, {:5.2}, {:5.2}, {:5.2} }}\n"
        "  ] [PRUNED]",
        p1.x, p1.y, p1.z, p1.w,
        p2.x, p2.y, p2.z, p2.w
      );
      return primitives.Erase(line);
    }

    if ((p1_regions | p2_regions) == CENTER_REGION) // line is entirely inside of the screen
    {
      LOG_TRACE("  Line = [\n"
        "    {{ {:5.2}, {:5.2}, {:5.2}, {:5.2} }}\n"
        "    {{ {:5.2}, {:5.2}, {:5.2}, {:5.2} }}\n"
        "  ] [ACCEPTED]",
        p1.x, p1.y, p1.z, p1.w,
        p2.x, p2.y, p2.z, p2.w
      );
      return line.next();
    }

    // line needs to be cliped


    return line.next();
  }

  iter ProcessTriangle(std::vector<glm::vec4> &geometryBuffer, PrimitiveBuffer &primitives, piter<Triangle> triangle)
  {
    return triangle.next();
  }

  iter ProcessPrimitive(std::vector<glm::vec4> &geometryBuffer, PrimitiveBuffer &primitives, iter primitive)
  {
    switch (primitive->vertexCount)
    {
    case 1:
      return ProcessPoint(geometryBuffer, primitives, piter<Point>(primitive));
    case 2:
      return ProcessLine(geometryBuffer, primitives, piter<Line>(primitive));
    case 3:
      return ProcessTriangle(geometryBuffer, primitives, piter<Triangle>(primitive));
    default:
      LOG_CRITICAL("Critical error: Unsupported primitive type encountered !");
      dial::Critical("Unsupported primitive type !");
    }
    return primitive + 1;
  }

  void ProcessPrimitives(gl::RenderMode mode, PrimitiveBuffer &primitives)
  {
    std::vector<glm::vec4> &geometryBuffer = Context::Instance()->GetGeometryBuffer();

    iter it = primitives.begin();

    while (it != primitives.end())
    {
      it = ProcessPrimitive(geometryBuffer, primitives, it);
    }
  }
}

