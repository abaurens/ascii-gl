#pragma once

#include "FrameBuffer.hpp"
#include "Buffer.hpp"
#include "Shader.hpp"

#include <glm/vec4.hpp>

#include <map>
#include <optional>

class Context
{
public:
  static Scope<Context> &Instance();

  std::optional<Program*> GetProgram(int programId);
  std::optional<Program*> GetBoundProgram();

  std::optional<Buffer*> GetBuffer(int bufferId);
  std::optional<Buffer*> GetBoundBuffer();

  glm::vec4 *GetGeometryBuffer(size_t vertexCount);
  const glm::vec4 *GetGeometryBuffer() const { return m_geometryBuffer.data(); };

  FrameBuffer &GetFrameBuffer() { return m_framebuffer; }

  bool IsBuffer(int bufferId) const;
  int CreateBuffer(int minBufferId) const;
  void DeleteBuffer(int bufferId);
  void BindBuffer(int bufferId);

  bool IsProgram(int programId) const;
  int CreateProgram() const;
  void DeleteProgram(int programId);
  void UseProgram(int programId);

  template<class Vertex>
    requires IsVertex<Vertex>
  void BufferData(size_t size, const Vertex *vertices);

private:
  static Scope<Context> m_instance;

private:
  int m_bound_buffer = 0;
  int m_bound_program = 0;
  std::map<int, Buffer> m_vertexBuffers; // VAO array
  std::map<int, Program> m_programs;
  FrameBuffer m_framebuffer;

  std::vector<glm::vec4> m_geometryBuffer;
};

template<class Vertex>
  requires IsVertex<Vertex>
void Context::BufferData(size_t size, const Vertex *data)
{
  if (!m_bound_buffer)
    return;
  m_vertexBuffers[m_bound_buffer].Set(size, data);
}