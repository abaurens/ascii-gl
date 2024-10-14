#include "graphics/gl.hpp"

#include "graphics/Context.hpp"

#include <frozen/map.h>

#include <execution>
#include <algorithm>

namespace gl
{
  namespace draw_implementation
  {
    using DrawFunction = void (*)(Program &program, Buffer &buffer, size_t, const int *);

    void DrawPoints(Program &program, Buffer &buffer, size_t indicesCount, const int *indices);

    void DrawLines(Program &program, Buffer &buffer, size_t indicesCount, const int *indices);
    void DrawLineLoop(Program &program, Buffer &buffer, size_t indicesCount, const int *indices);
    void DrawLineStrip(Program &program, Buffer &buffer, size_t indicesCount, const int *indices);

    void DrawTriangles(Program &program, Buffer &buffer, size_t indicesCount, const int *indices);
    void DrawTriangleStrip(Program &program, Buffer &buffer, size_t indicesCount, const int *indices);
    void DrawTriangleFan(Program &program, Buffer &buffer, size_t indicesCount, const int *indices);

    frozen::map<RenderMode, DrawFunction, 7> functions = {
      { RenderMode::POINTS, DrawPoints },

      { RenderMode::LINES,      DrawLines     },
      { RenderMode::LINE_LOOP,  DrawLineLoop  },
      { RenderMode::LINE_STRIP, DrawLineStrip },

      { RenderMode::TRIANGLES,      DrawTriangles     },
      { RenderMode::TRIANGLE_STRIP, DrawTriangleStrip },
      { RenderMode::TRIANGLE_FAN,   DrawTriangleFan   },
    };
  };
}

namespace gl
{

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
    std::optional<std::reference_wrapper<Program>> program;
    program = Context::Instance()->GetProgram(programId);

    return program.has_value() && program.value().get().IsValid();
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
    std::optional<std::reference_wrapper<Buffer>> buffer;
    std::optional<std::reference_wrapper<Program>> program;

    Context &context = *Context::Instance();
    buffer = context.GetBoundBuffer();
    program = context.GetBoundProgram();

    if (!buffer.has_value() || !program.has_value())
      return;

    // Vertex shader
    {
      IVertexShader &shader   = program.value().get().GetVertexShader();
      Buffer        &vertices = buffer.value();

      glm::vec4 *geometryBuffer = Context::Instance()->GetGeometryBuffer(vertices.Count());

      printf("  Vertex shader: [ ");
      std::for_each(
        std::execution::par_unseq,
        geometryBuffer,
        geometryBuffer + vertices.Count(),
        [geometryBuffer, &shader, &vertices](glm::vec4 &pos) {
        size_t i = &pos - geometryBuffer;
        pos = shader(vertices.Vertex(i), i);
      });
      printf(" ]\n");
    }

    return draw_implementation::functions.at(mode)(program.value(), buffer.value(), indicesCount, indices);
  }
}

namespace gl
{
  namespace draw_implementation
  {
    void DrawPoints(Program &program, Buffer &buffer, size_t indicesCount, const int *indices)
    {
      if (indicesCount == 0)
        return;
      
      const glm::vec4 *geometryBuffer = Context::Instance()->GetGeometryBuffer();

      // geometry creation
      for (size_t i = 0; i < indicesCount; i++)
      {
        int index = indices[i];
        const glm::vec4 &pos = geometryBuffer[index];

        //printf("  Point[%llu] = { %5.2f, %5.2f, %5.2f, %5.2f }\n", i, pos.x, pos.y, pos.z, pos.w);
      }
    }

    void DrawLines(Program &program, Buffer &buffer, size_t indicesCount, const int *indices)
    {
      indicesCount -= (indicesCount % 2);

      const glm::vec4 *geometryBuffer = Context::Instance()->GetGeometryBuffer();

      for (size_t i = 0; i < indicesCount; i += 2)
      {
        int i1 = indices[i + 0];
        int i2 = indices[i + 1];
        const glm::vec4 &p1 = geometryBuffer[i1];
        const glm::vec4 &p2 = geometryBuffer[i2];

        //printf("  line[%llu] = [\n"
        //  "    { %5.2f, %5.2f, %5.2f, %5.2f },\n"
        //  "    { %5.2f, %5.2f, %5.2f, %5.2f }\n"
        //  "  ]\n",
        //  i / 2,
        //  p1.x, p1.y, p1.z, p1.w,
        //  p2.x, p2.y, p2.z, p2.w
        //);
      }
    }

    void DrawLineLoop(Program &program, Buffer &buffer, size_t indicesCount, const int *indices)
    {
      if (indicesCount < 2)
        return;

      const glm::vec4 *geometryBuffer = Context::Instance()->GetGeometryBuffer();

      for (size_t i = 1; i <= indicesCount; ++i)
      {
        int i1 = indices[i - 1];
        int i2 = indices[i % indicesCount];
        const glm::vec4 &p1 = geometryBuffer[i1];
        const glm::vec4 &p2 = geometryBuffer[i2];

        //printf("  line[%llu] = [\n"
        //  "    { %5.2f, %5.2f, %5.2f, %5.2f },\n"
        //  "    { %5.2f, %5.2f, %5.2f, %5.2f }\n"
        //  "  ]\n",
        //  i - 1,
        //  p1.x, p1.y, p1.z, p1.w,
        //  p2.x, p2.y, p2.z, p2.w
        //);
      }
    }

    void DrawLineStrip(Program &program, Buffer &buffer, size_t indicesCount, const int *indices)
    {
      if (indicesCount < 2)
        return;

      const glm::vec4 *geometryBuffer = Context::Instance()->GetGeometryBuffer();

      for (size_t i = 1; i < indicesCount; ++i)
      {
        int i1 = indices[i - 1];
        int i2 = indices[i - 0];
        const glm::vec4 &p1 = geometryBuffer[i1];
        const glm::vec4 &p2 = geometryBuffer[i2];

        //printf("  line[%llu] = [\n"
        //  "    { %5.2f, %5.2f, %5.2f, %5.2f },\n"
        //  "    { %5.2f, %5.2f, %5.2f, %5.2f }\n"
        //  "  ]\n",
        //  i - 1,
        //  p1.x, p1.y, p1.z, p1.w,
        //  p2.x, p2.y, p2.z, p2.w
        //);
      }
    }

    void DrawTriangles(Program &program, Buffer &buffer, size_t indicesCount, const int *indices)
    {
      indicesCount -= (indicesCount % 3);

      const glm::vec4 *geometryBuffer = Context::Instance()->GetGeometryBuffer();

      for (size_t i = 0; i < indicesCount; i += 3)
      {
        int i1 = indices[i + 0];
        int i2 = indices[i + 1];
        int i3 = indices[i + 2];
        const glm::vec4 &p1 = geometryBuffer[i1];
        const glm::vec4 &p2 = geometryBuffer[i2];
        const glm::vec4 &p3 = geometryBuffer[i3];

        //printf("  triangle[%llu] = [\n"
        //  "    { %5.2f, %5.2f, %5.2f, %5.2f },\n"
        //  "    { %5.2f, %5.2f, %5.2f, %5.2f },\n"
        //  "    { %5.2f, %5.2f, %5.2f, %5.2f }\n"
        //  "  ]\n",
        //  i / 3,
        //  p1.x, p1.y, p1.z, p1.w,
        //  p2.x, p2.y, p2.z, p2.w,
        //  p3.x, p3.y, p3.z, p3.w
        //);
      }
    }

    void DrawTriangleStrip(Program &program, Buffer &buffer, size_t indicesCount, const int *indices)
    {
      if (indicesCount < 3)
        return;

      const glm::vec4 *geometryBuffer = Context::Instance()->GetGeometryBuffer();

      for (size_t i = 2; i < indicesCount; ++i)
      {
        int i1 = indices[i - 2];
        int i2 = indices[i - 1];
        int i3 = indices[i - 0];
        const glm::vec4 &p1 = geometryBuffer[i1];
        const glm::vec4 &p2 = geometryBuffer[i2];
        const glm::vec4 &p3 = geometryBuffer[i3];

        //printf("  triangle[%llu] = [\n"
        //  "    { %5.2f, %5.2f, %5.2f, %5.2f },\n"
        //  "    { %5.2f, %5.2f, %5.2f, %5.2f },\n"
        //  "    { %5.2f, %5.2f, %5.2f, %5.2f }\n"
        //  "  ]\n",
        //  i - 2,
        //  p1.x, p1.y, p1.z, p1.w,
        //  p2.x, p2.y, p2.z, p2.w,
        //  p3.x, p3.y, p3.z, p3.w
        //);
      }
    }

    void DrawTriangleFan(Program &program, Buffer &buffer, size_t indicesCount, const int *indices)
    {
      if (indicesCount < 3)
        return;

      const glm::vec4 *geometryBuffer = Context::Instance()->GetGeometryBuffer();

      int i1 = indices[0];
      const glm::vec4 &p1 = geometryBuffer[i1];
      for (size_t i = 2; i < indicesCount; ++i)
      {
        int i2 = indices[i - 1];
        int i3 = indices[i - 0];
        const glm::vec4 &p2 = geometryBuffer[i2];
        const glm::vec4 &p3 = geometryBuffer[i3];

        //printf("  triangle[%llu] = [\n"
        //  "    { %5.2f, %5.2f, %5.2f, %5.2f },\n"
        //  "    { %5.2f, %5.2f, %5.2f, %5.2f },\n"
        //  "    { %5.2f, %5.2f, %5.2f, %5.2f }\n"
        //  "  ]\n",
        //  i - 2,
        //  p1.x, p1.y, p1.z, p1.w,
        //  p2.x, p2.y, p2.z, p2.w,
        //  p3.x, p3.y, p3.z, p3.w
        //);
      }
    }
  }
};
