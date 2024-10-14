#pragma once

#include "ascii-gl.hpp"

class Terminal
{
public:
  static Ref<Terminal> Create();

  virtual void PollEvents() = 0;

  virtual size_t Width() const = 0;
  virtual size_t Height() const = 0;

  virtual void SetUserPointer(void *ptr) = 0;
  virtual void *GetUserPointer() const = 0;

  virtual void SetCursorPos(unsigned int x, unsigned int y) = 0;

  virtual void Display() = 0;

protected:
  using TermResizeFunc = void (*)(Terminal &term, size_t width, size_t height);

  using MouseMoveFunc = void (*)(Terminal &term, float x, float y);
  using MouseButtonFunc = void (*)(Terminal &term, int button, bool pressed);
  using MouseScrollFunc = void (*)(Terminal &term, float dx, float dy);
  //using MouseClickFunc = void (*)(Terminal &term, int button);

  using KeyPressedFunc = void (*)(Terminal &term, uint32_t key, uint32_t scancode);
  using KeyReleasedFunc = void (*)(Terminal &term, uint32_t key, uint32_t scancode);

public:
  // callbacks
  virtual void SetResizeCallback(TermResizeFunc callback) = 0;

  virtual void SetMouseMoveCallback(MouseMoveFunc callback) = 0;
  virtual void SetMouseButtonCallback(MouseButtonFunc callback) = 0;
  virtual void SetMouseScrollCallback(MouseScrollFunc callback) = 0;
  //virtual void SetMouseClickCallback(MouseClickFunc callback) = 0;

  virtual void SetKeyPressedCallback(KeyPressedFunc callback) = 0;
  virtual void SetKeyReleasedCallback(KeyReleasedFunc callback) = 0;
};
