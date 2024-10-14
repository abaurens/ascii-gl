#pragma once

#include "core/types.h"

#include "graphics/IVertex.hpp"

#include <glm/vec4.hpp>

class IVertexShader;
class IFragmentShader;

class Program
{
public:
  void Attach(IVertexShader *vertexShader);
  void Attach(IFragmentShader *fragmentShader);
  bool IsValid();

  IVertexShader &GetVertexShader() { return *m_vertexShader; }

private:
  friend class IVertexShader;
  friend class IFragmentShader;

  Scope<IVertexShader>   m_vertexShader;
  Scope<IFragmentShader> m_fragmentShader;
};


class IVertexShader
{
public:
  IVertexShader(Program &parent) : m_parent(parent) {}

  virtual glm::vec4 operator()(const IVertex &vertex) const = 0;

private:
  const Program &m_parent;
};

class IFragmentShader
{
public:
  IFragmentShader(Program &parent) : m_parent(parent) {}

private:
  const Program &m_parent;
};


template<class VertexShader>
concept IsVertexShader = std::is_base_of<IVertexShader, VertexShader>::value;

template<class VertexShader>
concept IsValidVertexShader = IsVertexShader<VertexShader> && requires (const VertexShader &s, IVertex &v)
{
  s(v);
};


template<class FragmentShader>
concept IsFragmentShader = std::is_base_of<IFragmentShader, FragmentShader>::value;

//template<class FragmentShader>
//concept IsValidFragmentShader = requires (const FragmentShader &s){};

template<class Shader>
concept IsShader = IsVertexShader<Shader> || IsFragmentShader<Shader>;

