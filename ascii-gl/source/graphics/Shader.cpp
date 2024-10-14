#include "graphics/Shader.hpp"

void Program::Attach(IVertexShader *vertexShader)
{
  m_vertexShader.reset(vertexShader);
}

void Program::Attach(IFragmentShader *fragmentShader)
{
  m_fragmentShader.reset(fragmentShader);
}

bool Program::IsValid()
{
  return (m_vertexShader && m_fragmentShader);
}

/// TODO: Implement the uniform system
/// TODO: Implement the in/out system (variable interpolation)
