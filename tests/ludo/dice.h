#ifndef _dice_h_
#define _dice_h_

#include <random>

namespace ludo
{
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
class dice
{
public:

    dice() = default;
    ~dice() = default;

    unsigned int roll()
    {
        static std::random_device device;
        static std::mt19937       generator(device());
        _currentRolled = _distribution( generator );
        return _currentRolled;
    }

    [[nodiscard]] unsigned int currentRolled() const { return _currentRolled; }

private:

    std::uniform_int_distribution<unsigned int> _distribution{ 1u, 6u };
    unsigned int _currentRolled = -1;
};
}

#endif // _dice_h_