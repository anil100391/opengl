#ifndef _ludo_h_
#define _ludo_h_

#include <array>
#include <vector>
#include <string>
#include <cassert>
#include "dice.h"


// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
class lpiece
{
public:
    lpiece( int player ) : _player( player ) {}
    ~lpiece() = default;

    [[nodiscard]] int player() const noexcept { return _player; }

private:

    int _player;
};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
template<unsigned int PLAYERS=2>
class lsquareData
{
public:

    lsquareData() = default;
    ~lsquareData() = default;

    void addPlayer( unsigned int player ) { assert( _playerCount[player] <= 3 ); _playerCount[player]++;  }
    void removePlayer( unsigned int player ) { assert( _playerCount[player] != 0 ); _playerCount[player]--; }

    std::array<unsigned int, PLAYERS> _playerCount{ 0 };
};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
class lmove
{
public:

    lmove() = delete;
    lmove( int fromSq, int toSq ) : _fromSq( fromSq ), _toSq( toSq ) {}
    ~lmove() = default;

    [[nodiscard]] int fromSq() const noexcept { return _fromSq; }
    [[nodiscard]] int toSq() const noexcept { return _toSq; }

private:

    int _fromSq = -1;
    int _toSq   = -1;
};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
template<unsigned int PLAYERS=2>
class lboard
{
public:

    const int GRAVE   = -1; // location for dead or yet to start pieces
    const int DESTINY = 72; // location for passed pieces

    lboard()
    {
        for ( unsigned int ii = 0; ii < numPlayers(); ++ii )
        {
            _pieceLocation[ii] = { GRAVE, GRAVE, GRAVE, GRAVE };
        }
    }

    ~lboard() = default;

    [[nodiscard]] unsigned int numPlayers() const noexcept { return PLAYERS; }

    [[nodiscard]] int playerToMove() const noexcept;
    [[nodiscard]] int currentRolled() const noexcept { return _dice.currentRolled(); }
    [[nodiscard]] std::string getPlayersAtLocation( int row, int col ) const noexcept;
    [[nodiscard]] const std::array<unsigned int, 8>& getStopSquares() const noexcept;

    void makeMove();

    void draw() const;

private:

    [[nodiscard]] static int toBoardSq( int pieceSq, unsigned int player );
    [[nodiscard]] static int toPlayerSq( int row, int col, unsigned int player );

    [[nodiscard]] const char* toString( unsigned int player ) const noexcept { return (player + 1) % 2 == 0 ? "O" : "X"; }

    std::vector<lmove> generateMoves() const;

    ludo::dice _dice;

    std::vector<lsquareData<PLAYERS>> _sq{ 225 };

    // player piece locations
    std::array<std::array<int, 4>, PLAYERS> _pieceLocation;

    unsigned int _playerToMove = 0;
};

#include "ludo.cc"

#endif // _ludo_h_
