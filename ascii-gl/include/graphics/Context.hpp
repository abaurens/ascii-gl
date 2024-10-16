#pragma once

#include "graphics/primitives/Primitives.hpp"
#include "graphics/FrameBuffer.hpp"
#include "graphics/Buffer.hpp"
#include "graphics/Shader.hpp"

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

  std::vector<glm::vec4> &GetGeometryBuffer(size_t vertexCount);
  std::vector<glm::vec4> &GetGeometryBuffer() { return m_geometryBuffer; }
  const std::vector<glm::vec4> &GetGeometryBuffer() const { return m_geometryBuffer; }

  PrimitiveBuffer &GetPrimitiveBuffer() { return m_primitives; }
  const PrimitiveBuffer &GetPrimitiveBuffer() const { return m_primitives; }

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

  void SetViewport(float x, float y, float width, float height);
  glm::vec4 GetViewport() const { return m_viewport; }

private:
  static Scope<Context> m_instance;

private:
  int m_bound_buffer = 0;
  int m_bound_program = 0;
  std::map<int, Buffer> m_vertexBuffers; // VAO array
  std::map<int, Program> m_programs;
  FrameBuffer m_framebuffer;

  PrimitiveBuffer m_primitives;
  std::vector<glm::vec4> m_geometryBuffer;

  glm::vec4 m_viewport = { 0.0f, 0.0f, 1.0f, 1.0f };
};

template<class Vertex>
  requires IsVertex<Vertex>
void Context::BufferData(size_t size, const Vertex *data)
{
  if (!m_bound_buffer)
    return;
  m_vertexBuffers[m_bound_buffer].Set(size, data);
}