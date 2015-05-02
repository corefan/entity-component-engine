// opengl
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// my own
#include <systems/render.hpp>
#include <utils/utils.hpp>
#include <assets/modelasset.hpp>
#include <assets/textureasset.hpp>

#define X_AXIS glm::vec3(1, 0, 0)
#define Y_AXIS glm::vec3(0, 1, 0)
#define Z_AXIS glm::vec3(0, 0, 1)

using namespace assets;
using namespace utils;
using namespace components;

namespace systems
{
  RenderSystem::RenderSystem(Program& program)
    : System("render", TRANSFORM | MODEL), mProgram(program), mCamera(nullptr)
  {
  }

  void RenderSystem::updateCamera()
  {
    if(mCamera)
    {
      mProgram.use();

      mCamera->update();

      // set the camera matrix
      GLint cameraIndex = mProgram.uniform("camera");
      glUniformMatrix4fv(cameraIndex, 1, GL_FALSE, glm::value_ptr(mCamera->mwv()));

      // set the light to be the position of the camera
      GLint positionIndex = mProgram.uniform("light.position");
      glUniform3fv(positionIndex, 1, glm::value_ptr(mCamera->position()));

      // set the light colour
      GLint intensityIndex = mProgram.uniform("light.intensities");
      glUniform3fv(intensityIndex, 1, glm::value_ptr(glm::vec3(1, 1, 1)));

      mProgram.unuse();
    }
  }

  void RenderSystem::activateTexture(GLenum activeTexture, GLint texture, const std::string uniform)
  {
    glActiveTexture(activeTexture);
    glUniform1i(mProgram.uniform((GLchar *)uniform.c_str()), texture);
  }

  void RenderSystem::entityAdded(Engine& engine, Entity& entity)
  {
    System::entityAdded(engine, entity);

    // flyweight of models
    auto model = entity.getComponent<Model>(MODEL);
    auto modelAsset = engine.getAsset<ModelAsset>(model->getName());

    if(modelAsset)
    {
      modelAsset->prepareVAO(mProgram, "vert", "vertTexCoord", "vertNormal");

      model->setAsset(modelAsset);
    }
    else
    {
      std::cerr << "Model " << model->getName() << " could not be loaded!" << std::endl;
    }

    // flyweight of textures
    auto texture = entity.getComponent<Texture>(TEXTURE);
    if(texture)
    {
      auto textureAsset = engine.getAsset<TextureAsset>(texture->getName());

      if(textureAsset)
      {
        texture->setAsset(textureAsset);
      }
      else
      {
        std::cerr << "Texture " << texture->getName() << " could not be loaded!" << std::endl;
      }
    }
  }

  void RenderSystem::entityChanged(Engine& engine, Entity& entity, ComponentType newComponent)
  {
    if(!mCamera && newComponent == CAMERA)
    {
      mCamera = entity.getComponent<Camera>(CAMERA);
      updateCamera();
    }
  }

  void RenderSystem::run()
  {
    if(!mCamera)
    {
      return;
    }

    if(mCamera->needsUpdate())
    {
      updateCamera();
    }

    mProgram.use();

    for( auto &e : mEntities )
    {
      auto entity = e.get();

      auto transform = entity.getComponent<Transform>(TRANSFORM);
      auto model = entity.getComponent<Model>(MODEL);
      auto texture = entity.getComponent<Texture>(TEXTURE);

      GLint modelIndex = mProgram.uniform("model");
      glm::mat4 modelMatrix = glm::mat4();

      // rotate
      const sf::Vector3f& rotation = transform->getRotation();
      modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.x), X_AXIS);
      modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.y), Y_AXIS);
      modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.z), Z_AXIS);

      // scale
      modelMatrix = glm::scale(modelMatrix, sfmlToGlm(transform->getScale()));

      // translate
      modelMatrix = glm::translate(modelMatrix, sfmlToGlm(transform->getPosition()));
      glUniformMatrix4fv(modelIndex, 1, GL_FALSE, glm::value_ptr(modelMatrix));

      // set lighting variables
      GLint normalIndex = mProgram.uniform("normalMatrix");
      glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(modelMatrix)));
      glUniformMatrix3fv(normalIndex, 1, GL_FALSE, glm::value_ptr(normalMatrix));

      // set the texture
      if(texture)
      {
        texture->bind(0);
        activateTexture(GL_TEXTURE0, 0, "uTexture");
      }

      model->draw();

      if(texture)
      {
        texture->unbind();
      }
    }

    mProgram.unuse();
  }
}
