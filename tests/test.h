#ifndef _tests_h_
#define _tests_h_

#include <vector>
#include <string>
#include <utility>
#include <iostream>
#include <functional>

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
class Event;
class Application;

namespace test
{

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
class Test
{
public:

    Test(Application *app) : _app(app) {}
    virtual ~Test() {}

    virtual void OnUpdate(float deltaTime) {}
    virtual void OnRender() {}
    virtual void OnImGuiRender() {}
    virtual void OnEvent(Event &evt) {}
protected:

    Application *_app = nullptr;
};

class TestMenu : public Test
{
public:

    TestMenu(Application *app, Test*& currentTest);
    ~TestMenu();

    virtual void OnImGuiRender() override;

    template <typename T>
    void RegisterTest(const std::string& name)
    {
        std::cout << "Registering test\n";
        _tests.push_back(std::make_pair(name, [this](){ return new T(_app); }));
    }

private:

    Test*&      _currentTest;
    std::vector<std::pair<std::string, std::function<Test*()>>> _tests;

};

}

#endif // _tests_h_
