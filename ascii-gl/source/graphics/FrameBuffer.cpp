#include "graphics/FrameBuffer.hpp"

FrameBuffer::FrameBuffer() : FrameBuffer(0, 0) {}

FrameBuffer::FrameBuffer(int width, int height) : m_width(width), m_height(height), m_pixels(nullptr)
{
  Resize(width, height);
}

FrameBuffer::~FrameBuffer()
{
  //delete[] m_pixels;
}

void FrameBuffer::Resize(size_t width, size_t height)
{
  if (width == m_width && height == m_height)
    return;

  Scope<Pixel[]> new_pixels = nullptr;

  if (width >= 0 && height >= 0)
  {
    new_pixels = std::make_unique<Pixel[]>(width * height);

    for (size_t y = 0; y < height; ++y)
    {
      for (size_t x = 0; x < width; ++x)
      {
        if (x < m_width && y < m_height)
          new_pixels[x + y * width] = m_pixels[x + y * m_width];
        else
          new_pixels[x + y * width] = DEFAULT_PIXEL;
      }
    }
  }
  else
  {
    height = 0;
    width = 0;
  }

  //delete[] m_pixels;
  //m_pixels = new_pixels;
  m_pixels.swap(new_pixels);
  m_height = height;
  m_width  = width;
}

void FrameBuffer::Clear(char character, Color color)
{
  for (size_t i = 0; i < m_width * m_height; ++i)
  {
    m_pixels[i].color = color;
    m_pixels[i].character = character;
  }
}

void FrameBuffer::SetPixel(size_t x, size_t y, const Color &color, char character)
{
  if (x < m_width && y < m_height)
  {
    m_pixels[x + y * m_width].color = color;
    m_pixels[x + y * m_width].character = character;
  }
}

void FrameBuffer::SetPixel(size_t x, size_t y, uint8_t r, uint8_t g, uint8_t b, char character)
{
  SetPixel(x, y, Color{ .rgb = { r, g, b, 255 } }, character);
}

void FrameBuffer::SetPixel(size_t x, size_t y, uint32_t color, char character)
{
  SetPixel(x, y, Color{ .value = color }, character);
}

void FrameBuffer::SetPixel(size_t x, size_t y, char character)
{
  SetPixel(x, y, DEFAULT_COLOR, character);
}

Pixel &FrameBuffer::GetPixel(size_t x, size_t y)
{
  return m_pixels[x + y * m_width];
}

const Pixel &FrameBuffer::GetPixel(size_t x, size_t y) const
{
  return m_pixels[x + y * m_width];
}

Pixel *FrameBuffer::operator[](size_t y)
{
  return &m_pixels[y * m_width];
}

const Pixel *FrameBuffer::operator[](size_t y) const
{
  return &m_pixels[y * m_width];
}
