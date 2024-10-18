#include "App.hpp"

#include "core/Log.hpp"

#include "graphics/gl.hpp"
#include "graphics/Context.hpp"
#include "graphics/primitives/Primitives.hpp"

#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <iostream>
#include <iomanip>
#include <chrono>
#include <cmath>

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

    glm::vec4 &&result = u_viewProjection * u_transform * glm::vec4(vertex.position, 1);


    LOG_TRACE("projected: {0:5.2f}, {1:5.2f}, {2:5.2f}, {3:5.2f}", result[0], result[1], result[2], result[3]);
    //printf("%s%llu", idx ? ", " : "", idx);
    return result;
  }
};

class FragmentShader : public IFragmentShader
{
public:
  FragmentShader(Program &parent) : IFragmentShader(parent) {}
};



int vao;
int program;

App::App()
{
  // Create and setup the terminal instance (will manage the window)
  {
    m_terminal = Terminal::Create();
    m_terminal->SetUserPointer(this);
  
    m_terminal->SetResizeCallback([](Terminal &term, size_t width, size_t height) {
      FrameBuffer &framebuffer = Context::Instance()->GetFrameBuffer();
  
      //framebuffer.Resize((unsigned)width, (unsigned)height);
      framebuffer.Clear(0, 0, 0, 0);

      gl::Viewport(0.0f, 0.0f, (float)framebuffer.Width(), (float)framebuffer.Height());
  
      //framebuffer.SetPixel(0,         0,          0xffffffff);
      //framebuffer.SetPixel(0,         height - 1, 0xffff0000);
      //framebuffer.SetPixel(width - 1, 0,          0xff00ff00);
      //framebuffer.SetPixel(width - 1, height - 1, 0xff0000ff);
    });

    m_terminal->SetKeyPressedCallback([](Terminal &term, uint32_t key, uint32_t scancode) {
      /// TODO: Implement a proper key/button mapping

      // check if the escape key is pressed
      if (key == 27)
      {
        static_cast<App *>(term.GetUserPointer())->Stop();
      }
    });
  }

  // set framebuffer and viewport size
  {
    FrameBuffer &framebuffer = Context::Instance()->GetFrameBuffer();
    //framebuffer.Resize(unsigned(m_terminal->Width() / 2), unsigned(m_terminal->Height()));
    gl::Viewport(0, 0, (float)framebuffer.Width(), (float)framebuffer.Height());
  }

  // Create the shader
  {
    program = gl::CreateProgram();
    gl::UseProgram(program);

    gl::CompileShader<VertexShader>();   // triggers a compile error if the VertexShader lacks the call operator
    gl::CompileShader<FragmentShader>(); // triggers a compile error if the FragmentShader doesn't inherit from IFragmentShader

    gl::AttachShader<VertexShader>(program);
    gl::AttachShader<FragmentShader>(program);

    gl::LinkProgram(program); // only returns false if the program lacks a vertex or fragment shader
  }

  // Create the vertex array object
  {
    gl::CreateBuffers(1, &vao);
    gl::BindBuffer(vao);

    constexpr float angle = float(M_PI * 2.0 / 3.0);

    gl::BufferData<Vertex>({
      {        0.0f,       1.0f, 0.0f }, //{  0.0f,  1.0f, 0.0f },
      {  sin(angle), cos(angle), 0.0f }, //{ -1.0f, -1.0f, 0.0f },
      { -sin(angle), cos(angle), 0.0f }, //{  1.0f, -1.0f, 0.0f },

      //{  0.0f,  0.0f, 0.0f },
      //{  0.0f,  0.0f, 0.0f },
      //{  0.0f,  0.0f, 0.0f },
      //
      //{  0.0f,  0.0f, 0.0f },
      //{  0.0f,  0.0f, 0.0f },
      //{  0.0f,  0.0f, 0.0f },
    });

    gl::BindBuffer(0);
  }

  // generate the modelview matrix
  {
    glm::mat4 proj = glm::perspective(glm::radians(60.0f), 1280.0f / 720.0f, 0.1f, 1000.0f);
    glm::mat4 view = glm::lookAt(glm::vec3{ 0, 0, 2.0f }, glm::vec3{ 0, 0, 0 }, glm::vec3{ 0, 1, 0 });
    gl::Uniform(program, "u_viewProjection", proj * view);

    // I don't want any transform for now. So I send a constant identity matrix
    gl::Uniform(program, "u_transform", glm::identity<glm::mat4>());
  }
}

App::App(int ac, char **av) : App()
{

}

void App::Run()
{
  using namespace std::chrono;

  m_running = true;

  const steady_clock::time_point start = steady_clock::now();

  float last = 0.0f;
  while (m_running)
  {
    gl::Clear();
    
    m_terminal->PollEvents();
    
    const float start_time = float(duration_cast<nanoseconds>(steady_clock::now() - start).count()) / 1'000'000'000.0f;
    const float timestep = start_time - last;
    last = start_time;

    gl::BindBuffer(vao);
    gl::UseProgram(program);
    
    glm::mat4 transform = glm::rotate(glm::identity<glm::mat4>(), glm::radians(start_time * 90.0f), glm::vec3{ 0, 0, 1 });
    gl::Uniform(program, "u_transform", transform);

    //gl::DrawElements(gl::POINTS, { 0, 1, 2 });// { 0, 1, 2, 3, 4, 5, 6, 7, 8 });

    //gl::DrawElements(gl::LINES,      { 1, 2 });// { 0, 1, 2, 3, 4, 5, 6, 7, 8 });
    gl::DrawElements(gl::LINE_LOOP,  { 0, 1, 2 });// { 0, 1, 2, 3, 4, 5, 6, 7, 8 });
    //gl::DrawElements(gl::LINE_STRIP, { 0, 1, 2 });// { 0, 1, 2, 3, 4, 5, 6, 7, 8 });

    //gl::DrawElements(gl::TRIANGLES,      { 0, 1, 2 });// { 0, 1, 2, 3, 4, 5, 6, 7, 8 });
    //gl::DrawElements(gl::TRIANGLE_STRIP, { 0, 1, 2 });// { 0, 1, 2, 3, 4, 5, 6, 7, 8 });
    //gl::DrawElements(gl::TRIANGLE_FAN,   { 0, 1, 2 });// { 0, 1, 2, 3, 4, 5, 6, 7, 8 });

    LOG_WARN("Timestep: {:.8f} s | {:3.0} Fps", timestep, 1.0f / timestep);

    m_terminal->Display();
  }

  while (m_running)
  {
    m_terminal->PollEvents();
  }
}

void App::Stop() { m_running = false; }
