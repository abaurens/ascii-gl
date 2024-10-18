#pragma once

#include "Terminal.hpp"
#include "TermBuffer.hpp"

#include <Windows.h>
#undef near
#undef far
#undef min
#undef max

class WindowsTerminal : public Terminal
{
public:
  WindowsTerminal();
  ~WindowsTerminal();

  virtual void PollEvents() override;
  virtual size_t Width() const override { return m_width; }
  virtual size_t Height() const override { return m_height; }

  //virtual void SetWidth(size_t width) override;
  //virtual void SetHeight(size_t height) override;
  //virtual void SetSize(size_t width, size_t height) override;

  virtual void SetCursorPos(unsigned int x, unsigned int y) override;

  virtual void SetUserPointer(void *ptr) override { m_userData = ptr; }
  virtual void *GetUserPointer() const override { return m_userData; }

  virtual void Display() override;

  // callbacks
public:
  virtual void SetResizeCallback(TermResizeFunc callback) override { m_resizeCallback = callback; }

  virtual void SetMouseMoveCallback(MouseMoveFunc callback) override { m_mouseMoveCallback = callback; }
  virtual void SetMouseButtonCallback(MouseButtonFunc callback) override { m_mouseButtonCallback = callback; }
  virtual void SetMouseScrollCallback(MouseScrollFunc callback) override { m_mouseScrollCallback = callback; }
  //virtual void SetMouseClickCallback(MouseClickFunc callback) override { m_mouseClickCallback = callback; }

  virtual void SetKeyPressedCallback(KeyPressedFunc callback) override { m_keyPressedCallback = callback; }
  virtual void SetKeyReleasedCallback(KeyReleasedFunc callback) override { m_keyReleasedCallback = callback; }

private:
  void OnKeyEvent(const KEY_EVENT_RECORD &event);
  void OnMenuEvent(const MENU_EVENT_RECORD &event);
  void OnMouseEvent(const MOUSE_EVENT_RECORD &event);
  void OnFocusEvent(const FOCUS_EVENT_RECORD &event);
  void OnResizeEvent(const WINDOW_BUFFER_SIZE_RECORD &event);

  void ResizeOutputBuffer();

private:
  size_t m_width = 0;
  size_t m_height = 0;

  HANDLE m_inputHandle;
  HANDLE m_outputHandle;
  DWORD  m_savedMode;

  TermBuffer m_outputBuffer;

  void *m_userData = nullptr;

  TermResizeFunc m_resizeCallback = nullptr;

  MouseMoveFunc   m_mouseMoveCallback   = nullptr;
  MouseButtonFunc m_mouseButtonCallback = nullptr;
  MouseScrollFunc m_mouseScrollCallback = nullptr;
  //MouseClickFunc  m_mouseClickCallback  = nullptr;

  KeyPressedFunc  m_keyPressedCallback = nullptr;
  KeyReleasedFunc m_keyReleasedCallback = nullptr;
};