#include "graphics/Primitives.hpp"

using iterator = PrimitiveBuffer::iterator;

iterator PrimitiveBuffer::begin()
{
  return iterator(m_data.get());
}

iterator PrimitiveBuffer::begin() const
{
  return iterator(m_data.get());
}

iterator PrimitiveBuffer::end()
{
  return iterator(m_data.get() + m_pos);
}

iterator PrimitiveBuffer::end() const
{
  return iterator(m_data.get() + m_pos);
}

IPrimitive &PrimitiveBuffer::operator[](size_t idx)
{
  IPrimitive *primitive = reinterpret_cast<IPrimitive *>(m_data.get());

  iterator it = begin();
  for (size_t i = 0; i < idx; i++)
    ++it;

  return *it;
}

const IPrimitive &PrimitiveBuffer::operator[](size_t idx) const
{
  IPrimitive *primitive = reinterpret_cast<IPrimitive *>(m_data.get());

  iterator it = begin();
  for (size_t i = 0; i < idx; i++)
    ++it;

  return *it;
}

void PrimitiveBuffer::Grow(size_t additional)
{
  const size_t new_size = m_size + additional;

  IPrimitive *data = new IPrimitive[new_size];

  memcpy(data, m_data.get(), m_size);
  m_data.reset(data);
  m_size = new_size;
}

// iterators
iterator iterator::operator+(int i)
{
  IPrimitive *p = pos;

  for (int j = 0; j < i; j++)
    p = p + (1 + p->Size());

  return iterator(p);
}

iterator iterator::operator+(int i) const
{
  IPrimitive *p = pos;

  for (int j = 0; j < i; j++)
    p = p + (1 + p->Size());

  return iterator(p);
}

iterator &iterator::operator++()
{
  const size_t off = (1 + pos->Size());

  pos += off;
  return *this;
}

iterator iterator::operator++(int)
{
  iterator tmp = *this;
  pos += (1 + pos->Size());
  return tmp;
}