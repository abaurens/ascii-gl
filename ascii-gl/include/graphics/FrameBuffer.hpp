#pragma once

#include "core/types.h"

union Color 
{
  uint32_t value;
  uint8_t  rgb[4];
  struct
  {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
  };
};

struct Pixel
{
  Color color;
  char  character;
};

class FrameBuffer
{
private:
  static constexpr char  DEFAULT_CHARACTER = ' ';
  static constexpr Color DEFAULT_COLOR = { .rgb = { 0, 0, 0, 0 } };
  static constexpr Pixel DEFAULT_PIXEL = { DEFAULT_COLOR, DEFAULT_CHARACTER };

public:
  FrameBuffer();
  FrameBuffer(int width, int height);
  ~FrameBuffer();

  FrameBuffer(const FrameBuffer &) = delete;
  FrameBuffer &operator=(const FrameBuffer &) = delete;

  size_t Width() const { return m_width; }
  size_t Height() const { return m_height; }

  void Resize(size_t width, size_t height);

  inline void Clear(Color color) { return Clear(DEFAULT_CHARACTER, color); }
  void Clear(char character = DEFAULT_CHARACTER, Color color = DEFAULT_COLOR);

  void SetPixel(size_t x, size_t y, const Color &color, char character = DEFAULT_CHARACTER);
  void SetPixel(size_t x, size_t y, uint8_t r, uint8_t g, uint8_t b, char character = DEFAULT_CHARACTER);
  void SetPixel(size_t x, size_t y, uint32_t color, char character = DEFAULT_CHARACTER);
  void SetPixel(size_t x, size_t y, char character);

  Pixel &GetPixel(size_t x, size_t y);
  const Pixel &GetPixel(size_t x, size_t y) const;

  Pixel *operator[](size_t y);
  const Pixel *operator[](size_t y) const;

private:
  size_t         m_width;
  size_t         m_height;
  Scope<Pixel[]> m_pixels;
};