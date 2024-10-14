#pragma once


#include "graphics/IVertex.hpp"

#include "core/types.h"
#include <vector>
#include <limits>

struct Buffer
{
public:
  Buffer() = default;

  Buffer(const Buffer &other, size_t vertexLimit = std::numeric_limits<size_t>::max())
  {
    CopyFrom(other, vertexLimit);
  }

  ~Buffer()
  {
    m_vertexCount = 0;
    m_vertexSize = 0;
    m_bufferSize = 0;

    delete[] m_bufferData;
    m_bufferData = nullptr;
  };

  void Clear()
  {
    m_vertexCount = 0;
    m_vertexSize = 0;
  }

  size_t Size() const { return m_vertexSize; }
  size_t Count() const { return m_vertexCount; }

  void CopyFrom(const Buffer &other, size_t vertexLimit = std::numeric_limits<size_t>::max())
  {
    m_vertexCount = std::min(other.m_vertexCount, vertexLimit);
    m_vertexSize = other.m_vertexSize;

    size_t dataSize = m_vertexCount * m_vertexSize;
    if (m_bufferSize < dataSize)
    {
      m_bufferSize = dataSize;
      delete[] m_bufferData;
      m_bufferData = new byte_t[m_bufferSize];
    }

    std::memcpy(m_bufferData, other.m_bufferData, dataSize);
  }

  template<class Vertex>
    requires IsVertex<Vertex>
  void Set(const std::vector<Vertex> &vertices) { return Set(vertices.size(), vertices.data()); }

  template<class Vertex>
    requires IsVertex<Vertex>
  void Set(std::initializer_list<Vertex> vertices) { return Set(vertices.size(), vertices.begin()); }

  template<class Vertex>
    requires IsVertex<Vertex>
  void Set(size_t size, const Vertex *vertices)
  {
    m_vertexSize = sizeof(Vertex);
    m_vertexCount = size;

    const size_t dataSize = m_vertexSize * m_vertexCount;
    if (m_bufferSize < dataSize)
    {
      m_bufferSize = dataSize;

      delete[] m_bufferData;
      m_bufferData = new byte_t[m_bufferSize];
    }

    std::memcpy(m_bufferData, vertices, dataSize);
  }


  template<class Vertex>
    requires IsVertex<Vertex>
  Vertex &Get(size_t index)
  {
    return *reinterpret_cast<Vertex *>(Data(index));
  }
  template<class Vertex>
    requires IsVertex<Vertex>
  const Vertex &Get(size_t index) const
  {
    return *reinterpret_cast<Vertex *>(Data(index));
  }

  void *Data(size_t index = 0) { return m_bufferData + (m_vertexSize * index); }
  const void *Data(size_t index = 0) const { return m_bufferData + (m_vertexSize * index); }

  IVertex &Vertex(size_t index) { return *static_cast<IVertex *>(Data(index)); }
  const IVertex &Vertex(size_t index) const { return *static_cast<const IVertex *>(Data(index)); }

  byte_t *operator*() { return m_bufferData; }
  const byte_t *operator*() const { return m_bufferData; }

private:
  size_t m_vertexCount = 0;
  size_t m_vertexSize = 0;

  size_t m_bufferSize = 0;
  byte_t *m_bufferData = nullptr;
};
