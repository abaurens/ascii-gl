#include "WindowsTerminal.hpp"

#include "Dialogs.hpp"
#include "core/Core.hpp"

#include "core/Log.hpp"
#include "graphics/Context.hpp"

#include "TermBuffer.hpp"

# include <io.h>
#define write _write

//#define STRING_SIZE(str_literal) (sizeof(str_literal) - 1)
#define WRITE_CODE(str_literal_code) write(1, str_literal_code, string_size(str_literal_code))

consteval size_t string_size(std::string_view str) { return str.size(); }

//namespace Sizes
//{
//  static constexpr size_t color_code = string_size("\033[x8;2;rrr;ggg;bbbm");
//  static constexpr size_t pixel_char = string_size("  ");
//  static constexpr size_t reset_code = string_size("\033[0m");
//
//  static constexpr size_t clear_code = string_size("\033[2J");
//  static constexpr size_t newline = string_size("\n");
//
//  static constexpr size_t pixel = color_code + pixel_char + reset_code;
//
//  // sn_printf needs space for null terminator '\0'
//  consteval size_t snprintf(size_t size) { return size + 1; }
//}

//constexpr bool SLOW_MODE = true;

//unsigned int inject_pixel(char *buffer, char character)
//{
//  constexpr unsigned int size = Sizes::clear_code + Sizes::pixel_char;
//  char tmp[] = "\033[0m  ";
//
//  tmp[4] = character;
//
//  memcpy(buffer, tmp, size);
//  return size;
//}

//unsigned int inject_pixel(char *buffer, uint8_t color_type, Color color, char character)
//{
//  constexpr unsigned int size = Sizes::pixel;
//  char tmp[] = "\033[48;2;255;255;255m  \033[0m";
//  
//  //tmp[3] = '0' + (color_type % 10);
//  //color_type /= 10;
//  //tmp[2] = '0' + (color_type % 10);
//  
//  // red
//  //tmp[9] = '0' + (color.r % 10);
//  //color.r /= 10;
//  //tmp[8] = '0' + (color.r % 10);
//  //color.r /= 10;
//  //tmp[7] = '0' + (color.r % 10);
//  //
//  //// green
//  //tmp[13] = '0' + (color.g % 10);
//  //color.g /= 10;
//  //tmp[12] = '0' + (color.g % 10);
//  //color.g /= 10;
//  //tmp[11] = '0' + (color.g % 10);
//  //
//  //// blue
//  //tmp[17] = '0' + (color.b % 10);
//  //color.b /= 10;
//  //tmp[16] = '0' + (color.b % 10);
//  //color.b /= 10;
//  //tmp[15] = '0' + (color.b % 10);
//  
//  //tmp[19] = character;
//  
//  memcpy(buffer, tmp, size);
//  return size;
//}


Ref<Terminal> Terminal::Create()
{
  return std::make_shared<WindowsTerminal>();
}

WindowsTerminal::WindowsTerminal()
{
  // get the inpout and output handles
  m_inputHandle  = GetStdHandle(STD_INPUT_HANDLE);
  m_outputHandle = GetStdHandle(STD_OUTPUT_HANDLE);

  if (m_inputHandle == INVALID_HANDLE_VALUE || m_outputHandle == INVALID_HANDLE_VALUE)
    dial::Critical("Failed to get Windows console handles !");

  // save current mode to be restored on exit
  if (!GetConsoleMode(m_inputHandle, &m_savedMode))
    dial::Critical("Failed to get Windows console input mode !");

  constexpr DWORD flags = ENABLE_EXTENDED_FLAGS |
                          ENABLE_WINDOW_INPUT   |
                          ENABLE_MOUSE_INPUT    |
                          ENABLE_PROCESSED_INPUT;
  if (!SetConsoleMode(m_inputHandle, flags))
    dial::Critical("Failed to set Windows console input mode !");

  // get current size
  CONSOLE_SCREEN_BUFFER_INFO info;
  if (!GetConsoleScreenBufferInfo(m_outputHandle, &info))
    dial::Error("Failed to get Windows initial size !");

  m_width  = static_cast<size_t>(info.srWindow.Right  - info.srWindow.Left + 1);
  m_height = static_cast<size_t>(info.srWindow.Bottom - info.srWindow.Top  + 1);

  ResizeOutputBuffer();

  /// TODO: Move this. It should be in the CreateContext function
  Context::Instance()->SetFrameBuffer(m_outputBuffer);

  // switch to alternate buffer
  WRITE_CODE("\033[?1049h");

  // hide the cursor
  WRITE_CODE("\033[?25l");

  // set terminal to be 80 columns wide
  WRITE_CODE("\033[?3l");
}

WindowsTerminal::~WindowsTerminal()
{
  WRITE_CODE("\033[?3h");

  // reverts to default buffer
  WRITE_CODE("\033[?1049l");

  // show the cursor
  WRITE_CODE("\033[?25h");

  SetConsoleMode(m_inputHandle, m_savedMode);
}

void WindowsTerminal::PollEvents()
{
  INPUT_RECORD input_buffer[128];

  DWORD signaled = WaitForSingleObject(m_inputHandle, 0);

  if (signaled == WAIT_OBJECT_0)
  {
    DWORD read;
    ReadConsoleInput(m_inputHandle, input_buffer, 128, &read);

    for (DWORD i = 0; i < read; ++i)
    {
      switch (input_buffer[i].EventType)
      {
      case KEY_EVENT: // keyboard input
        OnKeyEvent(input_buffer[i].Event.KeyEvent);
        break;

      case MOUSE_EVENT: // mouse input
        OnMouseEvent(input_buffer[i].Event.MouseEvent);
        break;

      case WINDOW_BUFFER_SIZE_EVENT: // resizing
        OnResizeEvent(input_buffer[i].Event.WindowBufferSizeEvent);
        break;

      case FOCUS_EVENT:
        OnFocusEvent(input_buffer[i].Event.FocusEvent);
        break;

      case MENU_EVENT:   // disregard menu events
        OnMenuEvent(input_buffer[i].Event.MenuEvent);
        break;

      default:
        // Error: unknown event
        break;
      }
    }
  }
}

//void WindowsTerminal::SetWidth(size_t width)
//{
//
//}
//
//void WindowsTerminal::SetHeight(size_t height)
//{
//
//}
//
//void WindowsTerminal::SetSize(size_t width, size_t height)
//{
//
//}

void WindowsTerminal::SetCursorPos(unsigned int x, unsigned int y)
{
  // windows terminal coordinates are 1 based
  ++x; ++y;

  printf("\033[%u;%uH", y, x);
}

void WindowsTerminal::Display()
{
  // set cursor to top left
  SetCursorPos(0, 0);

  // write the framebuffer to the terminal
  write(1, m_outputBuffer.Data(), m_outputBuffer.length());
}




void WindowsTerminal::OnKeyEvent(const KEY_EVENT_RECORD &event)
{
  if (event.bKeyDown)
  {
    if (m_keyPressedCallback)
      return m_keyPressedCallback(*this, event.wVirtualKeyCode, event.wVirtualScanCode);
  }
  else
  {
    if (m_keyReleasedCallback)
      return m_keyReleasedCallback(*this, event.wVirtualKeyCode, event.wVirtualScanCode);
  }
}

void WindowsTerminal::OnMouseEvent(const MOUSE_EVENT_RECORD &event)
{
  constexpr int MAX_BTN_COUNT = sizeof(WORD) * 8;

  static bool buttons[MAX_BTN_COUNT] = { false };

  switch (event.dwEventFlags)
  {
  case MOUSE_MOVED:
  {
    if (m_mouseMoveCallback)
      return m_mouseMoveCallback(*this, (float)event.dwMousePosition.X, (float)event.dwMousePosition.Y);
    break;
  }
  
  case DOUBLE_CLICK:
  {
    break;
  }
  
  case MOUSE_WHEELED:
  {
    if (m_mouseScrollCallback)
    {
      const short delta = GET_WHEEL_DELTA_WPARAM(event.dwButtonState);
      return m_mouseScrollCallback(*this, 0.0f, (delta < 0 ? -1.0f : 1.0f));
    }
    break;
  }
  
  case MOUSE_HWHEELED:
  {
    if (m_mouseScrollCallback)
    {
      const short delta = GET_WHEEL_DELTA_WPARAM(event.dwButtonState);
      return m_mouseScrollCallback(*this, (delta < 0 ? -1.0f : 1.0f), 0.0f);
    }
    break;
  }
  
  // default case is button pressed/released events
  default:
  {
    const WORD btns = event.dwButtonState & 0xffff;

    for (int i = 0; i < MAX_BTN_COUNT; ++i)
    {
      const WORD mask = BIT(i);

      bool state = (btns & mask);
      if (state != buttons[i])
      {
        buttons[i] = state;

        if (m_mouseButtonCallback)
          m_mouseButtonCallback(*this, i + 1, state);
      }
    }
    break;
  }
  }
}

void WindowsTerminal::OnResizeEvent(const WINDOW_BUFFER_SIZE_RECORD &event)
{
  m_width  = static_cast<size_t>(event.dwSize.X);
  m_height = static_cast<size_t>(event.dwSize.Y);

  ResizeOutputBuffer();

  if (m_resizeCallback)
    m_resizeCallback(*this, m_width, m_height);
}

void WindowsTerminal::ResizeOutputBuffer()
{
  m_outputBuffer.Resize(unsigned int(m_width / 2), unsigned int(m_height));
}

void WindowsTerminal::OnMenuEvent(const MENU_EVENT_RECORD &event)
{}

void WindowsTerminal::OnFocusEvent(const FOCUS_EVENT_RECORD &event)
{}
