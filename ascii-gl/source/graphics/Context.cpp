#include "graphics/Context.hpp"

Scope<Context> Context::m_instance = nullptr;

Scope<Context> &Context::Instance()
{
  if (!m_instance)
    m_instance = std::make_unique<Context>();
  return m_instance;
}

std::optional<Program*> Context::GetBoundProgram()
{
  return GetProgram(m_bound_program);
}

std::optional<Program*> Context::GetProgram(int programId)
{
  if (!IsProgram(programId))
    return std::nullopt;

  return &m_programs[programId];
}


std::optional<Buffer*> Context::GetBuffer(int bufferId)
{
  if (!IsBuffer(bufferId))
    return std::nullopt;

  return &m_vertexBuffers[bufferId];
}

std::optional<Buffer*> Context::GetBoundBuffer()
{
  return GetBuffer(m_bound_buffer);
}

glm::vec4 *Context::GetGeometryBuffer(size_t vertexCount)
{
  if (m_geometryBuffer.size() < vertexCount)
    m_geometryBuffer.resize(vertexCount);
  return m_geometryBuffer.data();
}

bool Context::IsBuffer(int bufferId) const
{
  return bufferId && m_vertexBuffers.contains(bufferId);
}

int Context::CreateBuffer(int minBufferId) const
{
  int bufferId = std::max(1, minBufferId);
  while (IsBuffer(bufferId))
    ++bufferId;
  return bufferId;
}

void Context::DeleteBuffer(int bufferId)
{
  if (!IsBuffer(bufferId))
    return;
  m_vertexBuffers.erase(bufferId);
}

void Context::BindBuffer(int bufferId)
{
  if (!IsBuffer(bufferId))
    m_vertexBuffers[bufferId] = Buffer();
  m_bound_buffer = bufferId;
}


bool Context::IsProgram(int programId) const
{
  return programId && m_programs.contains(programId);
}

int Context::CreateProgram() const
{
  int programId = 1;
  while (IsProgram(programId))
    ++programId;
  return programId;
}

void Context::DeleteProgram(int programId)
{
  if (!IsProgram(programId))
    return;
  m_programs.erase(programId);
}

void Context::UseProgram(int programId)
{
  if (!IsProgram(programId))
    m_programs[programId] = Program();
  m_bound_program = programId;
}
