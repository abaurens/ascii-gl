#pragma once

#include <concepts>

struct IVertex
{
  virtual ~IVertex() = default;
  // common vertex code here
};

template<class Vertex>
concept IsVertex = std::is_base_of<IVertex, Vertex>::value;
