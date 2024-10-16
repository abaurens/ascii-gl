#pragma once

#include "core/types.h"
#include <array>
#include <concepts>

template<size_t VertexCount>
struct Primitive;

#pragma pack(push, 1)
struct IPrimitive
{
protected:
  using index_type = unsigned;

public:
  uint8_t vertexCount;

  size_t Size() const { return vertexCount * sizeof(index_type); }

  template<class Primitive>
  Primitive &As() { return *reinterpret_cast<Primitive*>(this); }

  template<class Primitive>
  const Primitive &As() const { return *reinterpret_cast<const Primitive*>(this); }

private:
  friend class PrimitiveBuffer;
};

template<size_t VertexCount>
struct Primitive : public IPrimitive
{
public:
  static constexpr size_t VERTEX_COUNT = VertexCount;

  static constexpr size_t Size() { return VertexCount * sizeof(index_type); }

  Primitive(std::array<index_type, VertexCount> indices) : IPrimitive{ VertexCount }, indices(indices) {}
  Primitive() : Primitive(nullptr) {}

public:
  std::array<index_type, VertexCount> indices;
};
#pragma pack(pop)

using Point    = Primitive<1>;
using Line     = Primitive<2>;
using Triangle = Primitive<3>;
//using Quad     = Primitive<4>;

class PrimitiveBuffer
{
private:
  template <typename Primitive>
  struct fixation_wrapper
  {
    PrimitiveBuffer &iterable;

    auto begin() { return iterable.pbegin<Primitive>(); }
    auto end() { return iterable.pend<Primitive>(); }
  };
public:
  class iterator;

  template<class Primitive>
    requires std::is_base_of_v<IPrimitive, Primitive>
  class piterator;

public:
  PrimitiveBuffer() : m_data(new IPrimitive[1024]), m_size(1024) {}
  PrimitiveBuffer(size_t size) : m_data(new IPrimitive[size]), m_size(size) {}
  PrimitiveBuffer(const PrimitiveBuffer &other) = delete;
  PrimitiveBuffer(PrimitiveBuffer &&other) = delete;

  ~PrimitiveBuffer() { Clear(); }

  iterator begin();
  iterator begin() const;

  iterator end();
  iterator end() const;

  template<class Primitive>
  piterator<Primitive> pbegin() { return piterator<Primitive>(m_data.get()); }

  template<class Primitive>
  piterator<Primitive> pbegin() const { return piterator<Primitive>(m_data.get()); }

  template<class Primitive>
  piterator<Primitive> pend() { return piterator<Primitive>(m_data.get() + m_pos); }

  template<class Primitive>
  piterator<Primitive> pend() const { return piterator<Primitive>(m_data.get() + m_pos); }

  template <class Primitive>
  fixation_wrapper<Primitive> fixed() { return { *this }; }

  size_t Size() const { return m_count; }

  void Clear() { m_pos = 0; m_count = 0; }

  iterator Erase(iterator it);
  iterator Erase(iterator begin, iterator end);

  template<class Primitive, class... Args>
  void Insert(const Args&... indices)
  {
    return Insert<Primitive>(Primitive{ { indices... }});
  }

  template<class Primitive>
  void Insert(const Primitive &primitive)
  {
    while (m_size < m_pos + sizeof(Primitive))
      Grow();

    memcpy(m_data.get() + m_pos, &primitive, sizeof(Primitive));
    m_pos += sizeof(Primitive);
    ++m_count;
  }

  template<class Primitive>
  void Reserve(size_t size) { return GrowTo(size * sizeof(Primitive)); }

  IPrimitive &operator[](size_t idx);
  const IPrimitive &operator[](size_t idx) const;

private:
  void Grow() { return Grow(m_size / 2); }
  void Grow(size_t additional);
  void GrowTo(size_t targetSize);

private:
  size_t m_pos = 0;
  size_t m_size = 0;
  size_t m_count = 0;
  Scope<IPrimitive[]> m_data;
};

class PrimitiveBuffer::iterator
{
public:
  iterator operator+(int i);
  iterator operator+(int i) const;

  iterator &operator++();
  iterator operator++(int);

  IPrimitive &operator*() { return *pos; }
  const IPrimitive &operator*() const { return *pos; }

  IPrimitive *operator->() { return pos; }
  const IPrimitive *operator->() const { return pos; }

  bool operator==(const iterator &other) const { return pos == other.pos; }
  bool operator!=(const iterator &other) const { return pos != other.pos; }

  size_t operator-(const iterator &other) const
  {
    const IPrimitive *p = pos;

    size_t dist = 0;
    for (dist = 0; p != other.pos; ++dist)
      p += (1 + p->Size());

    return dist;
  }

private:
  friend class PrimitiveBuffer;
  constexpr iterator(IPrimitive *pos) : pos(pos) {}

private:
  IPrimitive *pos;
};

template<class Primitive>
  requires std::is_base_of_v<IPrimitive, Primitive>
class PrimitiveBuffer::piterator
{
public:
  constexpr piterator(iterator it) : pos(static_cast<Primitive*>(it.pos)) {}

  piterator operator+(int i) { return (pos + i); }
  piterator operator-(int i) { return (pos - i); }
  piterator operator+(int i) const { return (pos + i); }
  piterator operator-(int i) const { return (pos - i); }
  
  piterator &operator++() { ++pos; return *this; }
  piterator &operator--() { --pos; return *this; }
  piterator operator++(int) { return pos++; }
  piterator operator--(int) { return pos--; }
  
  Primitive &operator*() { return *pos; }
  const Primitive &operator*() const { return *pos; }
  
  Primitive *operator->() { return pos; }
  const Primitive *operator->() const { return pos; }

  bool operator==(const piterator &other) const { return pos == other.pos; }
  bool operator!=(const piterator &other) const { return pos != other.pos; }
  
  bool operator==(const iterator &other) const { return pos == other.pos; }
  bool operator!=(const iterator &other) const { return pos != other.pos; }

  size_t operator-(const iterator &other) const { return pos - other.pos; }

  operator iterator() { return iterator(pos); }

private:
  friend class PrimitiveBuffer;
  constexpr piterator(IPrimitive *pos) : pos(static_cast<Primitive *>(pos)) {}
  constexpr piterator(Primitive *pos) : pos(pos) {}

private:
  Primitive *pos;
};

size_t distance(PrimitiveBuffer::iterator first, PrimitiveBuffer::iterator last);

template<class Primitive>
size_t distance(PrimitiveBuffer::piterator<Primitive> first, PrimitiveBuffer::piterator<Primitive> last)
{
  return last - first;
}