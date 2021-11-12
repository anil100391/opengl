#ifndef _testgame_h_
#define _testgame_h_

#include "test.h"
#include "../shader.h"
#include "../texture.h"
#include "../utils/mesh.h"
#include "../utils/meshgl.h"
#include "../light.h"
#include "../camera.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <memory>
#include <vector>
#include <string>
#include <utility>
#include <iostream>

#include <irrKlang.h>

namespace test
{

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
struct Asset
{
    std::unique_ptr<mesh>           _mesh;
    std::unique_ptr<MeshGL>         _glMesh;
    std::unique_ptr<Texture>        _texture;
};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
class TestGame : public Test
{
public:

    TestGame(Application *app);
    virtual ~TestGame();

    virtual void OnUpdate(float deltaTime) override;
    virtual void OnRender() override;
    virtual void OnImGuiRender() override;
    virtual void OnEvent(Event &evt) override;

private:

    void         CreateEarth();
    void         CreatePlayer();
    void         CreateWake();
    void         SetupCamera();

    const material& GetMaterial() const;
    void            SetMaterial(const material& m);
    pointlight      GetLight() const;

    Asset                           _earth;
    Asset                           _player;
    Asset                           _wake;

    glm::mat4                       _playerXform;
    glm::vec3                       _playerOrientation;
    glm::vec3                       _dwing;
    float                           _dtime = 0.0005f;

    std::vector<float>              _windWake[2];

    std::unique_ptr<Shader>         _shader;
    std::unique_ptr<Shader>         _lineShader;

    glm::mat4                       _viewMat;
    glm::mat4                       _projMat;

    material                        _material;
    Camera                          _camera;

    double                          _time = 0.0;
    bool                            _paused = true;

    irrklang::ISoundEngine          *_soundEngine = nullptr;
};

}

#endif // _testobjloader_h_
