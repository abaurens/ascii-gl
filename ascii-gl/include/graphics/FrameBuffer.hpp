#pragma once

#include "core/types.h"
#include <glm/glm.hpp>

class FrameBuffer
{
public:
  virtual ~FrameBuffer() = default;

  virtual unsigned int Width() const = 0;
  virtual unsigned int Height() const = 0;

  virtual void Resize(unsigned int width, unsigned int height) = 0;

  virtual void Clear(glm::vec3 color) = 0;
  virtual void Clear(glm::vec4 color) = 0;
  virtual void Clear(uint32_t color = 0x00000000) = 0;
  virtual void Clear(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha = 0xff) = 0;

  virtual void SetPixel(unsigned int x, unsigned int y, uint32_t color) = 0;
  virtual void SetPixel(unsigned int x, unsigned int y, glm::vec3 color) = 0;
  virtual void SetPixel(unsigned int x, unsigned int y, glm::vec4 color) = 0;
  virtual void SetPixel(unsigned int x, unsigned int y, uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha = 0xff) = 0;
};