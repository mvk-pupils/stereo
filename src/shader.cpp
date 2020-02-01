#include "shader.hpp"
#include "util.hpp"
#include "log.hpp"

#include <string>
#include <stdexcept>
#include <iostream>

void compile_shader(GLuint shader) {
  glCompileShader(shader);

  GLint success = 0;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    GLint log_size = 0;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_size);

    GLint length = 0;
    auto info_log = std::string(log_size, '\0');
    glGetShaderInfoLog(shader, log_size, &length, &info_log[0]);

    glDeleteShader(shader);

    ERROR("%s", &info_log[0]);

    throw std::runtime_error(info_log);
  }
}

GLuint link_program(GLuint vertex, GLuint fragment) {
  auto program = glCreateProgram();

  glAttachShader(program, vertex);
  glAttachShader(program, fragment);

  glLinkProgram(program);

  GLint success = 0;
  glGetProgramiv(program, GL_LINK_STATUS, &success);
  if (!success) {
    GLint log_size = 0;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &log_size);

    GLint length = 0;
    auto info_log = std::string(log_size, '\0');
    glGetProgramInfoLog(program, log_size, &length, &info_log[0]);

    glDeleteProgram(program);

    ERROR("%s", &info_log[0]);

    throw std::runtime_error(info_log);
  }

  glDetachShader(program, vertex);
  glDetachShader(program, fragment);

  return program;
}

Program Program::compile(const char* vertex_source, const char* fragment_source) {
  auto vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  auto fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

  auto _ = util::defer([=] { 
      glDeleteShader(vertex_shader);
      glDeleteShader(fragment_shader); 
    });
  
  glShaderSource(vertex_shader, 1, &vertex_source, NULL);
  glShaderSource(fragment_shader, 1, &fragment_source, NULL);

  compile_shader(vertex_shader);
  compile_shader(fragment_shader);

  auto program = link_program(vertex_shader, fragment_shader);

  return Program(program);
}

Program::Program(GLuint program) {
  DEBUG("Creating shader progam (%d)", program);
  this->program = std::shared_ptr<GLuint>(
      new GLuint(program), 
      [](GLuint* p) { 
        DEBUG("Deleting shader program (%d)", *p); 
        glDeleteProgram(*p);
      }
    );
}

void Program::use() {
  glUseProgram(*this->program);
}
