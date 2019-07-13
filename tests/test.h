#ifndef _tests_h_
#define _tests_h_

#include <vector>
#include <string>
#include <utility>
#include <iostream>
#include <functional>

namespace test
{

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
class Test
{
public:

    Test() {}
    virtual ~Test() {}

    virtual void OnUpdate(float deltaTime) {}
    virtual void OnRender() {}
    virtual void OnImGuiRender() {}
};

class TestMenu : public Test
{
public:

    TestMenu(Test*& currentTest);
    ~TestMenu();

    virtual void OnImGuiRender() override;

    template <typename T>
    void RegisterTest(const std::string& name)
    {
        std::cout << "Restering test\n";
        _tests.push_back(std::make_pair(name, [](){ return new T(); }));
    }

private:

    Test*&      _currentTest;
    std::vector<std::pair<std::string, std::function<Test*()>>> _tests;

};

}

#endif // _tests_h_
