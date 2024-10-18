#include "core/Log.hpp"

#include "graphics/gl.hpp"
#include "graphics/Context.hpp"
#include "graphics/primitives/PrimitiveAssemblers.hpp"
#include "graphics/primitives/PrimitiveProcessors.hpp"
#include "graphics/primitives/PrimitiveRenderers.hpp"

#include "Dialogs.hpp"

#include <execution>
#include <algorithm>

namespace gl
{
  void Viewport(float x, float y, float width, float height)
  {
    return Context::Instance()->SetViewport(x, y, width, height);
  }

  void Clear()
  {
    Context::Instance()->GetFrameBuffer().Clear();
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
      ///TODO: implement geometry shader ?

      // clip the vertices
      PrimitiveProcessor::ProcessPrimitives(mode, context.GetPrimitiveBuffer());

      // convert from clip space to normalized device coordinates and then to screen space
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
          pos *= inv_w;
          pos.w = inv_w;

          // viewport transform
          pos.x = ( pos.x + 1.0f) * (0.5f * viewport.z) + viewport.x;
          pos.y = (-pos.y + 1.0f) * (0.5f * viewport.w) + viewport.y;
      });
    }

    // draw the primitives
    {
      /// TODO: implement the rasterizer step
      PrimitiveRenderer::RenderPrimitives(context.GetPrimitiveBuffer());
    }

    return;
  }
}


