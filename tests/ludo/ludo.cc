#include <iostream>
#include "ludo.h"

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
template<unsigned int PLAYERS>
inline void lboard<PLAYERS>::draw() const
{
    for ( unsigned int ii = 0; ii < 15; ++ii )
    {
        for ( unsigned int jj = 0; jj < 15; ++jj )
        {
            if ( ii < 6 || ii >= 9 )
            {
                if ( jj >= 6 && jj < 9 )
                    std::cout << getPlayersAtLocation( ii, jj );
                else
                    std::cout << "  ";
            }
            else
            {
                if ( jj < 6 || jj >= 9 )
                    std::cout << getPlayersAtLocation( ii, jj );
                else
                    std::cout << "  ";
            }
        }

        std::cout << "\n";
    }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
template<unsigned int PLAYERS>
int lboard<PLAYERS>::playerToMove() const noexcept
{
    return _playerToMove;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
template<unsigned int PLAYERS>
void lboard<PLAYERS>::makeMove()
{
    auto rolled = _dice.roll();
    std::cout << rolled << "\n";

    auto player = playerToMove();
    auto moves = generateMoves();
    if ( !moves.empty() )
    {
        lmove &move = moves.front();
        int fromSq = move.fromSq();
        int toSq = move.toSq();

        // update piece locations
        for ( int &loc : _pieceLocation[player] )
        {
            if ( loc == fromSq )
            {
                loc = toSq;
                break;
            }
        }

        // update board
        if ( fromSq != GRAVE )
        {
            int fromBoardSq = toBoardSq( fromSq, player );
            _sq[fromBoardSq].removePlayer( player );
        }

        if ( toSq != GRAVE || toSq != DESTINY )
        {
            int toBrdSq = toBoardSq( toSq, player );
            _sq[toBrdSq].addPlayer( player );
        }
    }

    if ( rolled != 6 )
    {
        _playerToMove++;
        if ( _playerToMove >= numPlayers() )
            _playerToMove = 0;
    }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
template<unsigned int PLAYERS>
int lboard<PLAYERS>::toBoardSq( int pieceSq, unsigned int player )
{
    // TODO: assumed 4 player configuration
    constexpr unsigned int startBoardSq[4] = {  1 * 15 + 6,
                                                8 * 15 + 1,
                                               14 * 15 + 8,
                                                6 * 15 + 14 };

    auto zone = []( int player )
    {
        return player % 4;
    };

    int z = zone( player );
    if ( pieceSq <= 12 )
    {
    }
    else if ( pieceSq <= 24 )
    {
        pieceSq -= 12;
        z = zone( player + 1 );
    }
    else if ( pieceSq <= 36 )
    {
        pieceSq -= 24;
        z = zone( player + 2 );
    }
    else
    {
        pieceSq -= 36;
        z = zone( player + 3 );
    }

    if ( pieceSq <= 4 )
    {
        int row = -1;
        int col = -1;
        if ( z == 0 )
        {
            row = pieceSq + 1;
            col = 6;
        }
        else if ( z == 1 )
        {
            row = 8;
            col = pieceSq + 1;
        }
        else if ( z == 2 )
        {
            row = 15 - pieceSq - 1;
            col = 8;
        }
        else if ( z == 3 )
        {
            row = 6;
            col = 15 - pieceSq - 1;
        }

        return row * 15 + col;
    }
    else if ( pieceSq <= 10 )
    {
        int row = -1;
        int col = -1;
        if ( z == 0 )
        {
            row = 6;
            col = 6 - (pieceSq - 4);
        }
        else if ( z == 1 )
        {
            row = 8 + (pieceSq - 4);
            col = 6;
        }
        else if ( z == 2 )
        {
            row = 8;
            col = 8 + (pieceSq - 4);
        }
        else if ( z == 3 )
        {
            row = 6 - (pieceSq - 4);
            col = 8;
        }

        return row * 15 + col;
    }
    else if ( pieceSq <= 12 )
    {
        int row = -1;
        int col = -1;
        if ( z == 0 )
        {
            row = 6 + (pieceSq - 10);
            col = 0;
        }
        else if ( z == 1 )
        {
            row = 14;
            col = 6 + (pieceSq - 10);
        }
        else if ( z == 2 )
        {
            row = 8 - (pieceSq - 10);
            col = 14;
        }
        else if ( z == 3 )
        {
            row = 0;
            col = 8 - (pieceSq - 10);
        }

        return row * 15 + col;
    }

    return 0;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
template<unsigned int PLAYERS>
int lboard<PLAYERS>::toPlayerSq( int row, int col, unsigned int player )
{
    return 0;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
template<unsigned int PLAYERS>
std::string lboard<PLAYERS>::getPlayersAtLocation( int row, int col ) const noexcept
{
    if ( (row < 6 || row > 8) && (col < 6 || col > 8) )
        return "- ";

    std::string out;
    for ( unsigned int player = 0; player < numPlayers(); ++player )
    {
        if ( _sq[row * 15 + col]._playerCount[player] > 0 )
        {
            out += toString( player );
        }
    }

    if ( !out.empty() )
    {
        out += " ";
        return out;
    }

    return ". ";
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
template<unsigned int PLAYERS>
const std::array<unsigned int, 8>& lboard<PLAYERS>::getStopSquares() const noexcept
{
    static std::array<unsigned int, 8> squares
    {  1 * 15 + 6,
       6 * 15 + 2,
       8 * 15 + 1,
       12 * 15 + 6,
       13 * 15 + 8,
       8 * 15 + 12,
       6 * 15 + 13,
       2 * 15 + 8
    };

    return squares;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
template<unsigned int PLAYERS>
std::vector<lmove> lboard<PLAYERS>::generateMoves() const
{
    std::vector<lmove> moves;

    auto player = playerToMove();
    const std::array<int, 4>& locations = _pieceLocation[player];
    for ( int fromSq : locations )
    {
        if ( fromSq == DESTINY )
            continue;

        if ( fromSq == GRAVE )
        {
            if ( _dice.currentRolled() == 6 )
                moves.emplace_back( fromSq, 0 );
        }
        else
        {
            int toSq = fromSq + _dice.currentRolled();
            if ( toSq <= DESTINY )
                moves.emplace_back( fromSq, toSq );
        }
    }

    return moves;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// template<> class lboard<2>;
// template<> class lboard<4>;
