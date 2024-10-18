#include "TermBuffer.hpp"

#define PIXEL_STRING      "\033[48;2;000;000;000m  "
#define CLEARPIXEL_STRING "\033[0000000000000000m  "
#define NEWLINE_STRING    "\033[0m\n"

union Color
{
  uint32_t value;
  struct
  {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
  };
};

#pragma pack(push, 1)
struct Pixel
{
  char _code_bgn[7]; // "\033[48;2;"
  char red[3];       // "rrr"
  char _sep_1;       // ";"
  char green[3];     // "ggg"
  char _sep_2;       // ";"
  char blue[3];      // "bbb"
  char _sep_3;       // "m"
  char chrs[2];      // "  "
  char _code_end[4]; // "\033[0m"
};
#pragma pack(pop)

consteval unsigned int string_size(std::string_view str) { return unsigned int(str.size()); }

static constexpr unsigned int pixel_size = string_size(PIXEL_STRING);
static constexpr unsigned int newline_size = string_size(NEWLINE_STRING);

// one line is composed of it's pixels and a newline character
constexpr unsigned int line_size(unsigned int width)
{
  return (width * pixel_size) + newline_size;
}

// the buffer is composed of all the lines
static constexpr unsigned int buffer_size(unsigned int width, unsigned int height)
{
  return line_size(width) * height;
}

static constexpr void inject_digits(char *buffer, uint8_t value)
{
  buffer[0] = '0' + ((value / 100) % 10);
  buffer[1] = '0' + ((value / 10 ) % 10);
  buffer[2] = '0' + ((value / 1  ) % 10);
}



TermBuffer::TermBuffer() : TermBuffer(0, 0) {}

TermBuffer::TermBuffer(unsigned int width, unsigned int height) :
  m_width(width),
  m_height(height),
  m_size(buffer_size(width, height)), m_lsize(line_size(width))
{
  RecreateBuffer();
}

void TermBuffer::Clear(glm::vec3 color)
{
  return Clear(glm::vec4(color, 1.0f));
}

void TermBuffer::Clear(glm::vec4 color)
{
  constexpr glm::vec4 min_color{ 0 };
  constexpr glm::vec4 max_color{ 1 };

  color = glm::clamp(color, min_color, max_color) * 255.0f;
  return Clear(uint8_t(color.r), uint8_t(color.g), uint8_t(color.b), uint8_t(color.a));
}

void TermBuffer::Clear(uint32_t color)
{
  Color c{ color };
  return Clear(c.r, c.g, c.b);
}

void TermBuffer::Clear(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha)
{
  // generate the first line
  for (unsigned int x = 0; x < m_width; ++x)
    SetPixel(x, 0, red, green, blue, alpha);

  // copy the first line to the next ones
  for (unsigned int y = 1; y < m_height; ++y)
  {
    unsigned int off = m_lsize * y;
    memcpy(m_buffer.get() + off, m_buffer.get(), m_lsize);
  }
  m_buffer[m_size - 1] = '\0';
}

void TermBuffer::SetPixel(unsigned int x, unsigned int y, uint32_t color)
{
  Color c{ color };
  return SetPixel(x, y, c.r, c.g, c.b, c.a);
}

void TermBuffer::SetPixel(unsigned int x, unsigned int y, glm::vec4 color)
{
  constexpr glm::vec4 min_color{ 0 };
  constexpr glm::vec4 max_color{ 1 };

  color = glm::clamp(color, min_color, max_color) * 255.0f;
  return SetPixel(x, y, uint8_t(color.r), uint8_t(color.g), uint8_t(color.b), uint8_t(color.a));
}

void TermBuffer::SetPixel(unsigned int x, unsigned int y, glm::vec3 color)
{
  return SetPixel(x, y, glm::vec4(color, 1.0f));
}

void TermBuffer::SetPixel(unsigned int x, unsigned int y, uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha)
{
  const size_t off = (x * pixel_size) + (y * line_size(m_width));
  Pixel *position = reinterpret_cast<Pixel*>(m_buffer.get() + off);

  const bool is_color = position->_sep_1 == ';';

  // if alpha is 0% we use the clear pixel string instead
  if (alpha == 0)
  {
    if (is_color)
      memcpy(position, CLEARPIXEL_STRING, pixel_size);
    return;
  }

  if (!is_color)
    memcpy(position, PIXEL_STRING, pixel_size);

  const float blend = float(alpha) / 255.0f;

  inject_digits(position->red,   uint8_t(red   * blend));
  inject_digits(position->green, uint8_t(green * blend));
  inject_digits(position->blue,  uint8_t(blue  * blend));
}

void TermBuffer::Resize(unsigned int width, unsigned int height)
{
  m_width = width;
  m_height = height;

  m_size  = buffer_size(width, height);
  m_lsize = line_size(width);

  RecreateBuffer();
}

void TermBuffer::RecreateBuffer()
{
  if (m_width == 0 || m_height == 0)
    return;

  m_buffer = std::make_unique<char[]>(m_size);

  // generate the first line
  for (unsigned int x = 0; x < m_width; ++x)
    memcpy(m_buffer.get() + (x * pixel_size), PIXEL_STRING, pixel_size);
  memcpy(m_buffer.get() + m_lsize - newline_size, NEWLINE_STRING, newline_size);

  // copy the first line to the next ones
  for (unsigned int y = 1; y < m_height; ++y)
  {
    unsigned int off = m_lsize * y;
    memcpy(m_buffer.get() + off, m_buffer.get(), m_lsize);
  }
  m_buffer[m_size - 1] = '\0';
}
