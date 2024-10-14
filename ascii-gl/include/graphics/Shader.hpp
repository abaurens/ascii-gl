#pragma once

#include "core/types.h"

#include "graphics/IVertex.hpp"

#include <glm/vec4.hpp>

#include <map>
#include <stdexcept>
#include <string_view>

class IVertexShader;
class IFragmentShader;

class InvalidUniformException : public std::runtime_error
{
public:
  InvalidUniformException(const std::string_view name) : std::runtime_error("Invalid uniform: " + std::string(name)) {}
  InvalidUniformException(const std::string_view error, const std::string_view name) : std::runtime_error(std::string(error) + ": " + std::string(name)) {}
};


class Program
{
  template<class T>
  class Uniform;

  struct IUniform
  {
    IUniform(size_t size) : size(size) {}
    virtual ~IUniform() = default;

    size_t size;

    template<class T>
    Uniform<T> *Cast() { return dynamic_cast<Uniform<T>*>(this); }
    template<class T>
    const Uniform<T> *Cast() const { return dynamic_cast<Uniform<T>*>(this); }

    template<class T> Uniform<T> &As() { return *Cast<T>(); }
    template<class T> const Uniform<T> &As() const { return *Cast<T>(); }
  };

  template<class T>
  class Uniform : public IUniform
  {
  public:
    Uniform(const T &value) : IUniform{ sizeof(T) }, m_value(value) {}
    Uniform(T &&value) : IUniform{ sizeof(T) }, m_value(std::move(value)) {}
    virtual ~Uniform() = default;

    void Set(const T &value) { m_value = value; }
    void Set(T &&value) { m_value = std::move(value); }

    const T &Get() { return m_value; }
    const T &Get() const { return m_value; }

  private:
    T m_value;
  };

public:
  void Attach(IVertexShader *vertexShader);
  void Attach(IFragmentShader *fragmentShader);
  bool IsValid();

  IVertexShader &GetVertexShader() { return *m_vertexShader; }

  template<class T>
  void UploadUniform(const std::string_view name, const T &value)
  {
    using U = std::remove_cvref_t<T>;
    Uniform<U> *uniform = m_uniforms[name]->Cast<U>();

    if (!uniform)
    {
      m_uniforms[name].reset(new Uniform<U>(value));
      return;
    }
    else
    {
      uniform->Set(value);
    }
  }

  template<class T>
  void UploadUniform(const std::string_view name, T &&value)
  {
    using U = std::remove_cvref_t<T>;
    Uniform<U> *uniform = m_uniforms[name]->Cast<U>();

    if (!uniform)
    {
      m_uniforms[name].reset(new Uniform<U>(std::forward<U>(value)));
      return;
    }
    else
    {
      uniform->Set(std::forward<U>(value));
    }
  }

  template<class T>
  const T &GetUniform(const std::string_view name) const
  {
    using U = std::remove_cvref_t<T>;

    std::map<std::string_view, Scope<IUniform>>::const_iterator it;

    it = m_uniforms.find(name);
    if (it == m_uniforms.end() || !it->second->Cast<U>())
      throw InvalidUniformException(name);

    return it->second->As<U>().Get();
  }

private:
  friend class IShader;
  friend class IVertexShader;
  friend class IFragmentShader;

  std::map<std::string_view, Scope<IUniform>> m_uniforms;

  Scope<IVertexShader>   m_vertexShader;
  Scope<IFragmentShader> m_fragmentShader;
};


class IShader
{
protected:
  IShader(Program &parent) : m_parent(parent) {}
  virtual ~IShader() = default;

protected:
  template<class T>
  const T &Uniform(const std::string_view name) const
  {
    try
    {
      return m_parent.GetUniform<std::remove_cvref_t<T>>(name);
    }
    catch (const InvalidUniformException &)
    {
      throw InvalidUniformException("Missmatching uniform type in shader", name);
    }
  }

private:
  const Program &m_parent;
};


class IVertexShader : public IShader
{
public:
  IVertexShader(Program &parent) : IShader(parent) {}

  virtual glm::vec4 operator()(const IVertex &vertex, size_t idx) const = 0;
};


class IFragmentShader : public IShader
{
public:
  IFragmentShader(Program &parent) : IShader(parent) {}
};


template<class VertexShader>
concept IsVertexShader = std::is_base_of<IVertexShader, VertexShader>::value;

template<class VertexShader>
concept IsValidVertexShader = IsVertexShader<VertexShader> && requires (const VertexShader &s, IVertex &v)
{
  s(v, 0);
};


template<class FragmentShader>
concept IsFragmentShader = std::is_base_of<IFragmentShader, FragmentShader>::value;

//template<class FragmentShader>
//concept IsValidFragmentShader = requires (const FragmentShader &s){};

template<class Shader>
concept IsShader = IsVertexShader<Shader> || IsFragmentShader<Shader>;

