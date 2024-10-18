#pragma once

#include "graphics/FrameBuffer.hpp"
#include "core/types.h"

#include <glm/glm.hpp>
#include <string_view>

class TermBuffer : public FrameBuffer
{
public:
  TermBuffer();
  TermBuffer(unsigned int width, unsigned int height);

  virtual unsigned int Width() const override { return m_width; }
  virtual unsigned int Height() const override { return m_height; }

  virtual void Resize(unsigned int width, unsigned int height) override;

  virtual void Clear(glm::vec3 color) override;
  virtual void Clear(glm::vec4 color) override;
  virtual void Clear(uint32_t color = 0x00000000) override;
  virtual void Clear(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha = 0xff) override;

  virtual void SetPixel(unsigned int x, unsigned int y, uint32_t color) override;
  virtual void SetPixel(unsigned int x, unsigned int y, glm::vec3 color) override;
  virtual void SetPixel(unsigned int x, unsigned int y, glm::vec4 color) override;
  virtual void SetPixel(unsigned int x, unsigned int y, uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha = 0xff) override;

  // returns the number of bytes to be written onto the terminal buffer
  unsigned int length() const { return m_size - 1; }

  // returns the byte array to be written onto the terminal buffer
  const char *Data() const { return m_buffer.get(); }

private:
  void RecreateBuffer();

private:
  unsigned int m_width;
  unsigned int m_height;

  unsigned int m_size;
  unsigned int m_lsize;

  Scope<char[]> m_buffer;
};