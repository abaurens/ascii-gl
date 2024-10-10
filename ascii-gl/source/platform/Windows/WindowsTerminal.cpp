#include "WindowsTerminal.hpp"
#include "Dialogs.hpp"
#include "core/Core.hpp"

# include <io.h>
#define write _write

Ref<Terminal> Terminal::Create()
{
  return std::make_shared<WindowsTerminal>();
}

WindowsTerminal::WindowsTerminal()
{
  dial::Message("Restoring Windows console mode ...");

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

  // switch to alternate buffer
  write(1, "\033[?1049h", 8);

  // hide the cursor
  write(1, "\033[?25l", 6);
}

WindowsTerminal::~WindowsTerminal()
{
  // reverts to default buffer
  write(1, "\033[?1049l", 8);

  // hide the cursor
  write(1, "\033[?25h", 6);

  

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
  m_height = static_cast<size_t>(event.dwSize.Y) - 1;

  if (m_resizeCallback)
    m_resizeCallback(*this, m_width, m_height);
}

void WindowsTerminal::OnMenuEvent(const MENU_EVENT_RECORD &event)
{}

void WindowsTerminal::OnFocusEvent(const FOCUS_EVENT_RECORD &event)
{}
