#include "core/Log.hpp"

#include "graphics/gl.hpp"
#include "graphics/Context.hpp"
#include "graphics/primitives/PrimitiveAssemblers.hpp"
#include "graphics/primitives/PrimitiveProcessors.hpp"

#include "Dialogs.hpp"

#include <execution>
#include <algorithm>

namespace gl
{
  void Viewport(float x, float y, float width, float height)
  {
    return Context::Instance()->SetViewport(x, y, width, height);
  }

  void CreateBuffers(size_t size, int *buffers)
  {
    const Context &c = *Context::Instance();

    int last_buffer = 0;
    for (size_t i = 0; i < size; i++)
    {
      last_buffer = c.CreateBuffer(last_buffer);
      buffers[i] = last_buffer;
    }
  }

  void DeleteBuffers(size_t size, int *buffers)
  {
    Context &c = *Context::Instance();

    for (size_t i = 0; i < size; i++)
    {
      c.DeleteBuffer(buffers[i]);
    }
  }

  void BindBuffer(int bufferId)
  {
    return Context::Instance()->BindBuffer(bufferId);
  }

  int CreateProgram()
  {
    return Context::Instance()->CreateProgram();
  }

  void DeleteProgram(int programId)
  {
    return Context::Instance()->DeleteProgram(programId);
  }

  void UseProgram(int programId)
  {
    return Context::Instance()->UseProgram(programId);
  }

  bool LinkProgram(int programId)
  {
    std::optional<Program*> program;
    program = Context::Instance()->GetProgram(programId);

    return program.has_value() && program.value()->IsValid();
  }


  void DrawElements(const std::vector<int> &indices)
  {
    return DrawElements(RenderMode::TRIANGLES, indices.size(), indices.data());
  }

  void DrawElements(size_t indicesCount, const int *indices)
  {
    return DrawElements(RenderMode::TRIANGLES, indicesCount, indices);
  }


  void DrawElements(RenderMode mode, const std::vector<int> &indices)
  {
    return DrawElements(mode, indices.size(), indices.data());
  }

  void DrawElements(RenderMode mode, size_t indicesCount, const int *indices)
  {
    std::optional<Buffer*> buffer;
    std::optional<Program*> program;

    Context &context = *Context::Instance();
    buffer = context.GetBoundBuffer();
    program = context.GetBoundProgram();

    if (!buffer.has_value() || !program.has_value())
      return;

    std::vector<glm::vec4> &geometryBuffer = context.GetGeometryBuffer();

    // Vertex shader
    {
      IVertexShader &shader   = program.value()->GetVertexShader();
      Buffer        &vertices = *buffer.value();

      glm::vec4 *geometry = context.GetGeometryBuffer(vertices.Count()).data();

      std::for_each(
       #ifndef SINGLE_THREADED
        std::execution::par,
       #endif
        geometry, geometry + vertices.Count(), [geometry, &shader, &vertices](glm::vec4 &pos) {
          const size_t i = (&pos - geometry);
          pos = shader(vertices.Vertex(i), i);
      });
    }

    //Primitives assembly
    {
      std::vector<glm::vec4> &geometryBuffer = context.GetGeometryBuffer();
      PrimitiveBuffer &primitives = context.GetPrimitiveBuffer();

      primitives.Clear();

      LOG_TRACE("Assembling Primitives:");
      PrimitiveAssembler::AssemblePrimitive(mode, primitives, indicesCount, indices);
    }

    LOG_TRACE("Process Primitives:");
    // Prepare vertices for rendering
    {
      // geometry shader runs here
      


      // clip the vertices
      PrimitiveProcessor::ProcessPrimitive(mode, context.GetPrimitiveBuffer(), indicesCount, indices);

      // convert from clip space to normalized device coordinates
      const glm::vec4 viewport = context.GetViewport();
      std::for_each(
        #ifndef SINGLE_THREADED
        std::execution::par,
        #endif
        geometryBuffer.data(),
        geometryBuffer.data() + geometryBuffer.size(),
        [&context, &viewport](glm::vec4 &pos) {
          // perspective division
          const float inv_w = 1.0f / pos.w;
          pos /= pos.w;
          pos.w = inv_w;

          // viewport transform
          pos.x = ( pos.x + 1.0f) * (0.5f * viewport.z) + viewport.x;
          pos.y = (-pos.y + 1.0f) * (0.5f * viewport.w) + viewport.y;
      });
    }

    // draw the primitives
    {

    }



    // Debug rendered primitives
    {
      PrimitiveBuffer &primitives = context.GetPrimitiveBuffer();

      LOG_TRACE("[DEBUG] Final primitives:");
      unsigned i = 0;
      for (const IPrimitive &prim : primitives)
      {
        switch (prim.vertexCount)
        {
        case 1:
        {
          const Point &point = static_cast<const Point &>(prim);
          const glm::vec4 &pos = geometryBuffer[point.indices[0]];

          LOG_TRACE("  Point[{0}] = {{ {1:5.2}, {2:5.2}, {3:5.2}, {4:5.2} }}", i, pos.x, pos.y, pos.z, pos.w);

          context.GetFrameBuffer().SetPixel((size_t)pos.x, (size_t)pos.y, 0xffffffff);

          break;
        }
        case 2:
        {
          const Line &line = static_cast<const Line &>(prim);
          const glm::vec4 &p1 = geometryBuffer[line.indices[0]];
          const glm::vec4 &p2 = geometryBuffer[line.indices[1]];

          LOG_TRACE("  Point[{0}] = [\n"
            "    {{ {1:5.2}, {2:5.2}, {3:5.2}, {4:5.2} }}"
            "    {{ {5:5.2}, {6:5.2}, {7:5.2}, {8:5.2} }}"
            "  ]",
            i,
            p1.x, p1.y, p1.z, p1.w,
            p2.x, p2.y, p2.z, p2.w
          );
          break;
        }
        case 3:
        {
          const Triangle &triangle = static_cast<const Triangle &>(prim);
          const glm::vec4 &p1 = geometryBuffer[triangle.indices[0]];
          const glm::vec4 &p2 = geometryBuffer[triangle.indices[1]];
          const glm::vec4 &p3 = geometryBuffer[triangle.indices[2]];

          LOG_TRACE("  Point[{0}] = [\n"
            "    {{ {1:5.2}, {2:5.2}, {3:5.2}, {4:5.2} }}"
            "    {{ {5:5.2}, {6:5.2}, {7:5.2}, {8:5.2} }}"
            "    {{ {9:5.2}, {10:5.2}, {11:5.2}, {12:5.2} }}"
            "  ]",
            i,
            p1.x, p1.y, p1.z, p1.w,
            p2.x, p2.y, p2.z, p2.w,
            p3.x, p3.y, p3.z, p3.w
          );
          break;
        }
        default:
        {
          LOG_CRITICAL("Critical error: Unsupported primitive type encountered !");
          dial::Critical("Unsupported primitive type !");
        }
        }
        ++i;
      }
    }


    return;
  }
}


