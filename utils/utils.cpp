// standard libraries
#include <iostream>

// opengl
#include <GL/GLEW.h>

// my own
#include <utils/utils.hpp>

// http://www.lighthouse3d.com/cg-topics/error-tracking-in-opengl/
int printOglError(const char *file, int line)
{
  GLenum glErr;
  int retCode = 0;

  glErr = glGetError();
  if (glErr != GL_NO_ERROR)
  {
    // ignore deprecated warnings about gluErrorString
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wdeprecated-declarations"

    printf("glError in file %s @ line %d: %s\n", file, line, gluErrorString(glErr));

    #pragma GCC diagnostic pop
    retCode = 1;
  }
  return retCode;
}

namespace utils
{
  glm::vec3 sfmlToGlm(sf::Vector3f v)
  {
    return glm::vec3(v.x, v.y, v.z);
  }

  sf::Vector3f glmToSfml(glm::vec3 v)
  {
    return sf::Vector3f(v.x, v.y, v.z);
  }
}
