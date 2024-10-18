#pragma once

#include "core/types.h"

#include "graphics/IVertex.hpp"
#include "graphics/Shader.hpp"

#include <vector>
#include <optional>

class Context;

namespace gl
{
  enum class RenderMode
  {
    POINTS,
    
    LINES,
    LINE_LOOP,
    LINE_STRIP,

    TRIANGLES,
    TRIANGLE_STRIP,
    TRIANGLE_FAN
  };

  using enum RenderMode;

  void Viewport(float x, float y, float width, float height);
  void Clear();

  // Buffer API
  void CreateBuffers(size_t size, int *buffers);
  void DeleteBuffers(size_t size, int *buffers);
  void BindBuffer(int bufferId);

  template<class Vertex>
    requires IsVertex<Vertex>
  void BufferData(size_t size, const Vertex *data)
  {
    return Context::Instance()->BufferData<Vertex>(size, data);
  }

  template<class Vertex>
    requires IsVertex<Vertex>
  void BufferData(const std::vector<Vertex> &vertices)
  {
    return Context::Instance()->BufferData<Vertex>(vertices.size(), vertices.data());
  }

  template<class Vertex>
    requires IsVertex<Vertex>
  void BufferData(std::initializer_list<Vertex> vertices)
  {
    return Context::Instance()->BufferData<Vertex>(vertices.size(), vertices.begin());
  }

  // shader API
  int CreateProgram();
  void DeleteProgram(int programId);
  void UseProgram(int programId);
  bool LinkProgram(int programId);

  template<class Shader>
    requires IsValidVertexShader<Shader>
  consteval void CompileShader() {}

  template<class Shader>
    requires IsFragmentShader<Shader>
  consteval void CompileShader() {}

  template<class Shader>
    requires IsShader<Shader>
  void AttachShader(int programId)
  {
    std::optional<Program*> program_opt;

    program_opt = Context::Instance()->GetProgram(programId);
    if (!program_opt.has_value())
      return;

    Program &program = *program_opt.value();
    program.Attach(new Shader(program));
  }

  template<class T>
  void Uniform(int programId, const std::string_view name, const T &value)
  {
    using U = std::remove_cvref_t<T>;
    std::optional<Program *> program_opt;

    program_opt = Context::Instance()->GetProgram(programId);
    if (!program_opt.has_value())
      return;

    Program &program = *program_opt.value();
    return program.UploadUniform<U>(name, value);
  }

  template<class T>
  void Uniform(int programId, const std::string_view name, T &&value)
  {
    using U = std::remove_cvref_t<T>;
    std::optional<Program *> program_opt;

    program_opt = Context::Instance()->GetProgram(programId);
    if (!program_opt.has_value())
      return;

    Program &program = *program_opt.value();
    return program.UploadUniform<U>(name, std::forward<U>(value));
  }

  // Draw calls API
  void DrawElements(const std::vector<int> &indices);
  void DrawElements(size_t indicesCount, const int *indices);

  void DrawElements(RenderMode mode, const std::vector<int> &indices);
  void DrawElements(RenderMode mode, size_t indicesCount, const int *indices);
}