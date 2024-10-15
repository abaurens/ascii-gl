#include "App.hpp"

#include "graphics/gl.hpp"
#include "graphics/Primitives.hpp"

#include <iostream>
#include <iomanip>
#include <chrono>
#include <cmath>

#include <glm/glm.hpp>
#include <glm/ext.hpp>

struct Vertex : public IVertex
{
  Vertex(float i) : position(i) {}
  Vertex(float x, float y, float z) : position(x, y, z) {}
  Vertex(const glm::vec3 &position) : position(position) {}

  virtual ~Vertex() = default;


  glm::vec3 position;
};

class VertexShader : public IVertexShader
{
public:
  VertexShader(Program &parent) : IVertexShader(parent) {}

  virtual glm::vec4 operator()(const IVertex &vertex, size_t idx) const override
  {
    return operator()(static_cast<const Vertex&>(vertex), idx);
  }

  glm::vec4 operator()(const Vertex &vertex, size_t idx) const
  {
    const glm::mat4 &u_viewProjection = Uniform<glm::mat4>("u_viewProjection");
    const glm::mat4 &u_transform      = Uniform<glm::mat4>("u_transform");

    glm::vec4 &&result = u_viewProjection *u_transform *glm::vec4(vertex.position, 1);

    printf("%s%llu", idx ? ", " : "", idx);
    return result;
  }
};

class FragmentShader : public IFragmentShader
{
public:
  FragmentShader(Program &parent) : IFragmentShader(parent) {}
};

#include <array>

App::App()
{
  PrimitiveBuffer buffer;

  buffer.InsertPoint(Point{ { 10 } });
  buffer.InsertTriangle(Triangle{ {  5, 17, 14 } });
  buffer.InsertLine(Line{ { 21,  8 } });
  buffer.InsertTriangle(Triangle{ { 71, 12, 20 } });

  int i = 0;
  for (IPrimitive &prim : buffer)
  {
    switch (prim.vertexCount)
    {
    case 1:
    {
      Point &point = static_cast<Point &>(prim);
      std::cout << "Point[" << i++ << "] = { "
        << point.indices[0] << " }\n";
      break;
    }
    case 2:
    {
      Line &line = static_cast<Line &>(prim);
      std::cout << "Line[" << i++ << "] = { "
        << line.indices[0] << ", "
        << line.indices[1] << " }\n";
     break;
    }
    case 3:
    {
      Triangle &triangle = static_cast<Triangle &>(prim);
      std::cout << "Triangle[" << i++ << "] = { "
        << triangle.indices[0] << ", "
        << triangle.indices[1] << ", "
        << triangle.indices[2] << " }\n";
     break;
    }
    }
  }

  //m_terminal = Terminal::Create();
  //
  //m_terminal->SetUserPointer(this);
  //
  //m_terminal->SetResizeCallback([](Terminal &term, size_t width, size_t height) {
  //  FrameBuffer &framebuffer = Context::Instance()->GetFrameBuffer();
  //
  //  width /= 2;
  //  framebuffer.Resize(width, height);
  //  framebuffer.Clear();
  //
  //  framebuffer.SetPixel(0,         0,          0xffffffff);
  //  framebuffer.SetPixel(0,         height - 1, 0xffff0000);
  //  framebuffer.SetPixel(width - 1, 0,          0xff00ff00);
  //  framebuffer.SetPixel(width - 1, height - 1, 0xff0000ff);
  //});

  int program = gl::CreateProgram();
  gl::UseProgram(program);

  gl::CompileShader<VertexShader>();   // triggers a compile error if the VertexShader lacks the call operator
  gl::CompileShader<FragmentShader>(); // triggers a compile error if the FragmentShader doesn't inherit from IFragmentShader

  gl::AttachShader<VertexShader>(program);
  gl::AttachShader<FragmentShader>(program);

  gl::LinkProgram(program); // only returns false if the program lacks a vertex or fragment shader

  glm::mat4 proj = glm::perspective(70.0f, 16.0f / 9.0f, 0.1f, 100.0f);
  glm::mat4 view = glm::lookAt(glm::vec3{ 0, 0, 5 }, glm::vec3{ 0, 0, 0 }, glm::vec3{ 0, 1, 0 });
  gl::Uniform(program, "u_viewProjection", proj * view);

  gl::Uniform(program, "u_transform", glm::identity<glm::mat4>());

  int vao;
  gl::CreateBuffers(1, &vao);
  gl::BindBuffer(vao);

  gl::BufferData<Vertex>({
    {  0.0f,  0.0f, 0.0f },
    {  0.0f, 10.0f, 0.0f },
    { 10.0f,  0.0f, 0.0f },

    {  0.0f,  0.0f, 0.0f },
    {  0.0f, 10.0f, 0.0f },
    { 10.0f,  0.0f, 0.0f },

    {  0.0f,  0.0f, 0.0f },
    {  0.0f, 10.0f, 0.0f },
    { 10.0f,  0.0f, 0.0f },
  });

  gl::BindBuffer(0);

  //m_terminal->SetKeyPressedCallback([](Terminal &term, uint32_t key, uint32_t scancode) {
  //  /// TODO: Implement a proper key/button mapping
  //
  //  // check if the escape key is pressed
  //  if (key == 27)
  //  {
  //    static_cast<App *>(term.GetUserPointer())->Stop();
  //  }
  //});

  gl::BindBuffer(vao);

  std::cout << "Points\n";
  gl::DrawElements(gl::POINTS, { 0, 1, 2,   3, 4, 5,   6, 7, 8 });

  std::cout << "\nLines\n";
  gl::DrawElements(gl::LINES,      { 0, 1, 2,   3, 4, 5,   6, 7, 8 });
  std::cout << "\nLine loop\n";
  gl::DrawElements(gl::LINE_LOOP,  { 0, 1, 2,   3, 4, 5,   6, 7, 8 });
  std::cout << "\nLine strip\n";
  gl::DrawElements(gl::LINE_STRIP, { 0, 1, 2,   3, 4, 5,   6, 7, 8 });

  std::cout << "\nTriangles\n";
  gl::DrawElements(gl::TRIANGLES,      { 0, 1, 2,   3, 4, 5,   6, 7, 8 });
  std::cout << "\nTriangle strip\n";
  gl::DrawElements(gl::TRIANGLE_STRIP, { 0, 1, 2,   3, 4, 5,   6, 7, 8 });
  std::cout << "\nTriangle fan\n";
  gl::DrawElements(gl::TRIANGLE_FAN,   { 0, 1, 2,   3, 4, 5,   6, 7, 8 });

  exit(0);
}

App::App(int ac, char **av) : App()
{

}

void App::Run()
{
  m_running = true;

  FrameBuffer &framebuffer = Context::Instance()->GetFrameBuffer();

  std::chrono::steady_clock::time_point cl;
  std::chrono::steady_clock::time_point st = std::chrono::steady_clock::now();


  float x = 0;
  float y = 0;
  while (m_running)
  {
    //m_framebuffer.Clear();

    cl = std::chrono::steady_clock::now();
    float tm = float(std::chrono::duration_cast<std::chrono::milliseconds>(cl - st).count()) / 1000.0f;

    float angle = float(M_PI) / 2.0f * tm;

    float radius = float(framebuffer.Height() / 3);
    x = std::cos(angle) * radius + (framebuffer.Width() / 2);
    y = std::sin(angle) * radius + (framebuffer.Height() / 2);

    framebuffer.SetPixel(size_t(round(x)), size_t(round(y)), 0xffffffff, '@');

    m_terminal->PollEvents();



    gl::DrawElements({ 0, 1, 2 });

    m_terminal->Display();

    framebuffer.SetPixel(size_t(round(x)), size_t(round(y)), 0xff4f4f4f, 'x');
  }
}

void App::Stop() { m_running = false; }
