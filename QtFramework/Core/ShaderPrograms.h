#pragma once

#include <GL/glew.h>
#include <iostream>
#include <string>
#include <vector>

namespace cagd {
class ShaderProgram
{
protected:
    // handles of objects
    GLuint _vertex_shader;
    GLuint _fragment_shader;
    GLuint _program;

    // file names of sources
    std::string _vertex_shader_file_name;
    std::string _fragment_shader_file_name;

    // sources
    std::string _vertex_shader_source;
    std::string _fragment_shader_source;

    // status values
    GLint _vertex_shader_compiled;
    GLint _fragment_shader_compiled;
    GLint _linked;

    // log
    GLboolean
    _ListOpenGLErrors(const char *file_name, GLint line,
                      std::ostream &output = std::cout) const; // returns
                                                               // GL_TRUE if an
                                                               // OpenGL error
                                                               // occurred,
                                                               // GL_FALSE
                                                               // otherwise
    GLvoid _ListVertexShaderInfoLog(std::ostream &output = std::cout) const;
    GLvoid _ListFragmentShaderInfoLog(std::ostream &output = std::cout) const;
    GLvoid _ListProgramInfoLog(std::ostream &output = std::cout) const;
    GLvoid _ListValidateInfoLog(std::ostream &output = std::cout) const;

public:
    // default constructor
    ShaderProgram();

    GLboolean InstallShaders(const std::string &vertex_shader_file_name,
                             const std::string &fragment_shader_file_name,
                             GLboolean          logging_is_enabled = GL_FALSE,
                             std::ostream &     output             = std::cout);

    GLboolean SetUniformVariable1i(const GLchar *name, GLint parameter) const;
    GLboolean SetUniformVariable2i(const GLchar *name, GLint parameter_1,
                                   GLint parameter_2) const;
    GLboolean SetUniformVariable3i(const GLchar *name, GLint parameter_1,
                                   GLint parameter_2, GLint parameter_3) const;
    GLboolean SetUniformVariable4i(const GLchar *name, GLint parameter_1,
                                   GLint parameter_2, GLint parameter_3,
                                   GLint parameter_4) const;

    GLboolean SetUniformVariable1f(const GLchar *name, GLfloat parameter) const;
    GLboolean SetUniformVariable2f(const GLchar *name, GLfloat parameter_1,
                                   GLfloat parameter_2) const;
    GLboolean SetUniformVariable3f(const GLchar *name, GLfloat parameter_1,
                                   GLfloat parameter_2,
                                   GLfloat parameter_3) const;
    GLboolean SetUniformVariable4f(const GLchar *name, GLfloat parameter_1,
                                   GLfloat parameter_2, GLfloat parameter_3,
                                   GLfloat parameter_4) const;

    GLboolean SetUniformVector1f(const GLchar *name, GLsizei count,
                                 const GLfloat *value) const;
    GLboolean SetUniformVector2f(const GLchar *name, GLsizei count,
                                 const GLfloat *value) const;
    GLboolean SetUniformVector3f(const GLchar *name, GLsizei count,
                                 const GLfloat *value) const;
    GLboolean SetUniformVector4f(const GLchar *name, GLsizei count,
                                 const GLfloat *value) const;

    GLboolean SetUniformVector1i(const GLchar *name, GLsizei count,
                                 const GLint *value) const;
    GLboolean SetUniformVector2i(const GLchar *name, GLsizei count,
                                 const GLint *value) const;
    GLboolean SetUniformVector3i(const GLchar *name, GLsizei count,
                                 const GLint *value) const;
    GLboolean SetUniformVector4i(const GLchar *name, GLsizei count,
                                 const GLint *value) const;

    GLboolean SetUniformMatrix2fv(const GLchar *name, GLsizei count,
                                  GLboolean      transpose,
                                  const GLfloat *value) const;
    GLboolean SetUniformMatrix3fv(const GLchar *name, GLsizei count,
                                  GLboolean      transpose,
                                  const GLfloat *value) const;
    GLboolean SetUniformMatrix4fv(const GLchar *name, GLsizei count,
                                  GLboolean      transpose,
                                  const GLfloat *value) const;

    GLboolean SetUniformMatrix2x3fv(const GLchar *name, GLsizei count,
                                    GLboolean      transpose,
                                    const GLfloat *value) const;
    GLboolean SetUniformMatrix3x2fv(const GLchar *name, GLsizei count,
                                    GLboolean      transpose,
                                    const GLfloat *value) const;
    GLboolean SetUniformMatrix2x4fv(const GLchar *name, GLsizei count,
                                    GLboolean      transpose,
                                    const GLfloat *value) const;
    GLboolean SetUniformMatrix4x2fv(const GLchar *name, GLsizei count,
                                    GLboolean      transpose,
                                    const GLfloat *value) const;
    GLboolean SetUniformMatrix4x3fv(const GLchar *name, GLsizei count,
                                    GLboolean      transpose,
                                    const GLfloat *value) const;
    GLboolean SetUniformMatrix3x4fv(const GLchar *name, GLsizei count,
                                    GLboolean      transpose,
                                    const GLfloat *value) const;
    // homework: declare and define the remained uniform variable handling
    // methods

    GLint GetUniformVariableLocation(const GLchar *name,
                                     GLboolean logging_is_enabled = GL_FALSE,
                                     std::ostream &output = std::cout) const;

    GLvoid Disable() const;
    GLvoid Enable(GLboolean     logging_is_enabled = GL_FALSE,
                  std::ostream &output             = std::cout) const;

    virtual ~ShaderProgram();
};
} // namespace cagd
