#include "PrimitiveAssemblers.hpp"
#include "graphics/Context.hpp"
#include <frozen/map.h>

namespace PrimitiveAssembler
{
  using AssembleFunction = void (*)(PrimitiveBuffer &, size_t, const int *);

  static constexpr frozen::map<gl::RenderMode, AssembleFunction, 7> functions = {
    { gl::POINTS, AssemblePoints },

    { gl::LINES,      AssembleLines     },
    { gl::LINE_LOOP,  AssembleLineLoop  },
    { gl::LINE_STRIP, AssembleLineStrip },

    { gl::TRIANGLES,      AssembleTriangles     },
    { gl::TRIANGLE_STRIP, AssembleTriangleStrip },
    { gl::TRIANGLE_FAN,   AssembleTriangleFan   },
  };

  void AssemblePoints(PrimitiveBuffer &primitives, size_t indicesCount, const int *indices)
  {
    if (indicesCount == 0)
      return;

    primitives.Reserve<Point>(indicesCount);

    // geometry creation
    for (size_t i = 0; i < indicesCount; i++)
    {
      const unsigned index = reinterpret_cast<const unsigned *>(indices)[i];

      primitives.Insert<Point>(index);
    }
  }

  void AssembleLines(PrimitiveBuffer &primitives, size_t indicesCount, const int *indices)
  {
    indicesCount -= (indicesCount % 2);

    /// TODO: change this loop so it's primitive based
    primitives.Reserve<Line>(indicesCount / 2);

    for (size_t i = 0; i < indicesCount; i += 2)
    {
      const unsigned i1 = reinterpret_cast<const unsigned *>(indices)[i + 0];
      const unsigned i2 = reinterpret_cast<const unsigned *>(indices)[i + 1];

      primitives.Insert<Line>(i1, i2);
    }
  }

  void AssembleLineLoop(PrimitiveBuffer &primitives, size_t indicesCount, const int *indices)
  {
    if (indicesCount < 2)
      return;

    if (indicesCount == 2)
      indicesCount = 1;

    /// TODO: change this loop so it's primitive based
    primitives.Reserve<Line>(indicesCount);

    for (size_t i = 1; i <= indicesCount; ++i)
    {
      const unsigned i1 = reinterpret_cast<const unsigned *>(indices)[i - 1];
      const unsigned i2 = reinterpret_cast<const unsigned *>(indices)[i % indicesCount];

      primitives.Insert<Line>(i1, i2);
    }
  }

  void AssembleLineStrip(PrimitiveBuffer &primitives, size_t indicesCount, const int *indices)
  {
    if (indicesCount < 2)
      return;

    /// TODO: change this loop so it's primitive based
    primitives.Reserve<Line>(indicesCount - 1);

    for (size_t i = 1; i < indicesCount; ++i)
    {
      const unsigned i1 = reinterpret_cast<const unsigned *>(indices)[i - 1];
      const unsigned i2 = reinterpret_cast<const unsigned *>(indices)[i - 0];

      primitives.Insert<Line>(i1, i2);
    }
  }

  void AssembleTriangles(PrimitiveBuffer &primitives, size_t indicesCount, const int *indices)
  {
    indicesCount -= (indicesCount % 3);

    /// TODO: change this loop so it's primitive based
    primitives.Reserve<Triangle>(indicesCount / 3);

    for (size_t i = 0; i < indicesCount; i += 3)
    {
      const unsigned i1 = reinterpret_cast<const unsigned *>(indices)[i + 0];
      const unsigned i2 = reinterpret_cast<const unsigned *>(indices)[i + 1];
      const unsigned i3 = reinterpret_cast<const unsigned *>(indices)[i + 2];

      primitives.Insert<Triangle>(i1, i2, i3);
    }
  }

  void AssembleTriangleStrip(PrimitiveBuffer &primitives, size_t indicesCount, const int *indices)
  {
    if (indicesCount < 3)
      return;

    /// TODO: change this loop so it's primitive based
    primitives.Reserve<Triangle>(indicesCount - 2);

    for (size_t i = 2; i < indicesCount; ++i)
    {
      const unsigned i1 = reinterpret_cast<const unsigned *>(indices)[i - 2];
      const unsigned i2 = reinterpret_cast<const unsigned *>(indices)[i - 1];
      const unsigned i3 = reinterpret_cast<const unsigned *>(indices)[i - 0];

      primitives.Insert<Triangle>(i1, i2, i3);
    }
  }

  void AssembleTriangleFan(PrimitiveBuffer &primitives, size_t indicesCount, const int *indices)
  {
    if (indicesCount < 3)
      return;

    /// TODO: change this loop so it's primitive based
    primitives.Reserve<Triangle>(indicesCount - 2);

    const unsigned i1 = reinterpret_cast<const unsigned *>(indices)[0];
    //const glm::vec4 &p1 = geometryBuffer[i1];
    for (size_t i = 2; i < indicesCount; ++i)
    {
      const unsigned i2 = reinterpret_cast<const unsigned *>(indices)[i - 1];
      const unsigned i3 = reinterpret_cast<const unsigned *>(indices)[i - 0];

      primitives.Insert<Triangle>(i1, i2, i3);
    }
  }
  
  void AssemblePrimitive(gl::RenderMode mode, PrimitiveBuffer &primitives, size_t indicesCount, const int *indices)
  {
    return functions.at(mode)(primitives, indicesCount, indices);
  }
}