// standard libraries
#include <iostream>

// glm
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/compatibility.hpp>

// my own
#include <misc/printable.hpp>
#include <components/camera.hpp>
#include <utils/utils.hpp>

using namespace utils;

#define CAMERA_V glm::vec3(0,1,0)
#define CAMERA_H glm::vec3(1,0,0)
#define CAMERA_N glm::vec3(0,0,-1)
;

namespace components
{
  Camera::Camera(sf::Vector3f pos, float fov, float ratio, float near, float far)
    : Component(CAMERA),
      mPosition(sfmlToGlm(pos)), mFieldOfView(fov), mAspectRatio(ratio), mNear(near), mFar(far),
      mVerticalAngle(0.0f), mHorizontalAngle(0.0f), mNeedsUpdate(false)
  {
  }

  void Camera::move(glm::vec3 diff)
  {
    mPosition += diff;
    mNeedsUpdate = true;
  }

  void Camera::rotate(float vertical, float horizontal)
  {
    mVerticalAngle += glm::radians(vertical);
    mHorizontalAngle += glm::radians(horizontal);
    mNeedsUpdate = true;
  }

  void Camera::lookAt(sf::Vector3f pos)
  {
    glm::vec3 dir = glm::normalize(sfmlToGlm(pos) - mPosition);
    mVerticalAngle = glm::asin(-dir.y);
    mHorizontalAngle = -glm::atan(-dir.x, -dir.z);
    mNeedsUpdate = true;
  }

  const bool Camera::needsUpdate() const
  {
    return mNeedsUpdate;
  }

  void Camera::update()
  {
    mNeedsUpdate = false;
  }

  const glm::mat4 Camera::mwv() const
  {
    return mcv() * mwc();
  }

  const glm::mat4 Camera::mcv() const
  {
    return glm::perspective(glm::radians(mFieldOfView), mAspectRatio, mNear, mFar);
  }

  const glm::mat4 Camera::mwc() const
  {
    return glm::translate(orientation(), -mPosition);
  }

  const glm::mat4 Camera::orientation() const
  {
    glm::mat4 orientation;
    orientation = glm::rotate(orientation, mVerticalAngle, CAMERA_H);
    orientation = glm::rotate(orientation, mHorizontalAngle, CAMERA_V);
    return orientation;
  }

  const glm::vec3 Camera::forward() const
  {
    return glm::vec3(glm::inverse(orientation()) * glm::vec4(CAMERA_N, 1.0));
  }

  const glm::vec3 Camera::up() const
  {
    return glm::vec3(glm::inverse(orientation()) * glm::vec4(CAMERA_V, 1.0));
  }

  const glm::vec3 Camera::left() const
  {
    return glm::vec3(glm::inverse(orientation()) * glm::vec4(CAMERA_H, 1.0));
  }

  void Camera::print(std::ostream& where) const
  {
    printField(where, "position", toString(mPosition));
    printField(where, ", fov", mFieldOfView);
    printField(where, ", aspectRatio", mAspectRatio);
    printField(where, ", near", mNear);
    printField(where, ", far", mFar);
    printField(where, ", verticalAngle", glm::degrees(mVerticalAngle));
    printField(where, ", horizontalAngle", glm::degrees(mHorizontalAngle));
  }
}