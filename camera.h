#ifndef _camera_h_
#define _camera_h_

#include <cassert>

#include <glm/glm.hpp>
#include <glm/geometric.hpp>
#include <glm/gtc/matrix_transform.hpp>

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
class Camera
{
public:

    Camera() = default;
    ~Camera() = default;

    enum class PROJECTION
    {
        ORTHOGRAPHIC,
        PERSPECTIVE
    };

    [[nodiscard]] PROJECTION GetType() const noexcept
    {
        return _projType;
    }

    void SetType(PROJECTION pt) noexcept
    {
        _projType = pt;
    }

    void SetPosition(const glm::vec3 &pos) noexcept
    {
        _position = pos;
        _viewDirty = true;
    }

    [[nodiscard]] const glm::vec3& GetPosition() const noexcept
    {
        return _position;
    }

    void SetLookAt(const glm::vec3 &lookAt) noexcept
    {
        _lookAt = lookAt;
        _viewDirty = true;
    }

    [[nodiscard]] const glm::vec3& GetLookAt() const noexcept
    {
        return _lookAt;
    }

    [[nodiscard]] const glm::mat4& GetViewMatrix() noexcept
    {
        if ( _viewDirty )
        {
            ComputeViewMatrix();
            _viewDirty = false;
        }

        return _viewMatrix;
    }

private:

    void ComputeViewMatrix()
    {
        if ( GetType() == PROJECTION::ORTHOGRAPHIC )
        {
            _viewMatrix = GetOrthographicViewMatrix();
        }
        else
        {
            assert(GetType() == PROJECTION::PERSPECTIVE);
            _viewMatrix = GetPerspectiveViewMatrix();
        }
    }

    [[nodiscard]] glm::mat4 GetOrthographicViewMatrix() const noexcept
    {
        return glm::mat4(1.0f);
    }

    [[nodiscard]] glm::mat4 GetPerspectiveViewMatrix() const noexcept
    {
        return glm::lookAt( _position, _lookAt, glm::vec3(0,1,0));
    }

    glm::vec3           _position;
    glm::vec3           _lookAt;

    glm::mat4           _viewMatrix;
    bool                _viewDirty = true;

    PROJECTION          _projType = PROJECTION::PERSPECTIVE;
};

#endif // _camera_h_
