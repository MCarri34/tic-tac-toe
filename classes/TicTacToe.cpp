#include "TicTacToe.h"

// -----------------------------------------------------------------------------
// TicTacToe.cpp
// -----------------------------------------------------------------------------
// This file is intentionally *full of comments* and gentle TODOs that guide you
// through wiring up a complete Tic‑Tac‑Toe implementation using the game engine’s
// Bit / BitHolder grid system.
//
// Rules recap:
//  - Two players place X / O on a 3x3 grid.
//  - Players take turns; you can only place into an empty square.
//  - First player to get three-in-a-row (row, column, or diagonal) wins.
//  - If all 9 squares are filled and nobody wins, it’s a draw.
//
// Notes about the provided engine types you'll use here:
//  - Bit              : a visual piece (sprite) that belongs to a Player
//  - BitHolder        : a square on the board that can hold at most one Bit
//  - Player           : the engine’s player object (you can ask who owns a Bit)
//  - Game options     : let the mouse know the grid is 3x3 (rowX, rowY)
//  - Helpers you’ll see used: setNumberOfPlayers, getPlayerAt, startGame, etc.
//
// I’ve already fully implemented PieceForPlayer() for you. Please leave that as‑is.
// The rest of the routines are written as “comment-first” TODOs for you to complete.
// -----------------------------------------------------------------------------

const int AI_PLAYER   = -1;      // index of the AI player (O)
const int HUMAN_PLAYER= 1;      // index of the human player (X)

TicTacToe::TicTacToe()
{
}

TicTacToe::~TicTacToe()
{
}

// -----------------------------------------------------------------------------
// make an X or an O
// -----------------------------------------------------------------------------
// DO NOT CHANGE: This returns a new Bit with the right texture and owner
Bit* TicTacToe::PieceForPlayer(const int playerNumber)
{
    // depending on playerNumber load the "x.png" or the "o.png" graphic
    Bit *bit = new Bit();
    bit->LoadTextureFromFile(playerNumber == 0 ? "x.png" : "o.png"); // Player 0 (Human) = X, Player 1 (AI) = O
    bit->setOwner(getPlayerAt(playerNumber));
    return bit;
}

//
// setup the game board, this is called once at the start of the game
//
void TicTacToe::setUpBoard()
{
    // here we should call setNumberOfPlayers to 2 and then set up the game options so the mouse knows to draw a 3x3 grid
    // _gameOptions has a rowX and rowY property we should set to 3
    // then we need to setup our 3x3 array in _grid with the correct position of the square, and load the "square.png" sprite for each square
    // we will use the initHolder function on each square to do this
    // finally we should call startGame to get everything going

    // Set up a two-player game.
    setNumberOfPlayers(2);
    setAIPlayer(1); // Let player 2 be the AI.

    // Tell the engine that the board is a 3x3 grid so mouse picking works.
    _gameOptions.rowX = 3;
    _gameOptions.rowY = 3;

    // Initialize each square holder with a position and a sprite.
    // The spacing value only needs to be consistent; the engine uses the column/row numbers for click detection.
    const float cellSize = 128.0f;
    for (int row = 0; row < 3; ++row)
    {
        for (int col = 0; col < 3; ++col)
        {
            ImVec2 pos(col * cellSize, row * cellSize);
            _grid[row][col].initHolder(pos, "square.png", col, row);
        }
    }

    // Start the game so turns and input scanning begin.
    startGame();

}

//
// about the only thing we need to actually fill out for tic-tac-toe
//
bool TicTacToe::actionForEmptyHolder(BitHolder *holder)
{
    // 1) Guard clause: if holder is nullptr, fail fast.
    //    (Beginner hint: always check pointers before using them.)
    //    if (!holder) return false;
    if (!holder)
        return false;

    // 2) Is it actually empty?
    //    Ask the holder for its current Bit using the bit() function.
    //    If there is already a Bit in this holder, return false.
    
    // Only empty squares can accept a new piece.
    if (holder->bit() != nullptr)
        return false;

    // 3) Place the current player's piece on this holder:
    //    - Figure out whose turn it is (getCurrentPlayer()->playerNumber()).
    //    - Create a Bit via PieceForPlayer(currentPlayerIndex).
    //    - Position it at the holder's position (holder->getPosition()).
    //    - Assign it to the holder: holder->setBit(newBit);

    // Only empty squares can accept a new piece.
    Bit *placeBit = PieceForPlayer(getCurrentPlayer()->playerNumber());
    placeBit->setPosition(holder->getPosition());
    holder->setBit(placeBit);
    
    // 4) Return whether we actually placed a piece. true = acted, false = ignored.
    
    return true; // replace with true if completed a successful placement    
}

bool TicTacToe::canBitMoveFrom(Bit *bit, BitHolder *src)
{
    // you can't move anything in tic tac toe
    return false;
}

bool TicTacToe::canBitMoveFromTo(Bit* bit, BitHolder*src, BitHolder*dst)
{
    // you can't move anything in tic tac toe
    return false;
}

//
// free all the memory used by the game on the heap
//
void TicTacToe::stopGame()
{
    // clear out the board
    // loop through the 3x3 array and call destroyBit on each square
    // Clear out the board by destroying any Bit currently owned by each square.
    for (int row = 0; row < 3; ++row)
    {
        for (int col = 0; col < 3; ++col)
        {
            _grid[row][col].destroyBit();
        }
    }
}

//
// helper function for the winner check
//
Player* TicTacToe::ownerAt(int index ) const
{
    // index is 0..8, convert to x,y using:
    // y = index / 3
    // x = index % 3 
    // if there is no bit at that location (in _grid) return nullptr
    // otherwise return the owner of the bit at that location using getOwner()
    const int y = index / 3;
    const int x = index % 3;

    Bit* b = _grid[y][x].bit();
    if (!b)
        return nullptr;

    return b->getOwner();
}

Player* TicTacToe::checkForWinner()
{
    // check all the winning triples
    // if any of them have the same owner return that player
    // winning triples are:
    // 0,1,2
    // 3,4,5
    // 6,7,8
    // 0,3,6
    // 1,4,7
    // 2,5,8
    // 0,4,8
    // 2,4,6
    // you can use the ownerAt helper function to get the owner of a square
    // for example, ownerAt(0) returns the owner of the top-left square
    // if there is no bit in that square, it returns nullptr
    // if you find a winning triple, return the player who owns that triple
    // otherwise return nullptr

    // Hint: Consider using an array to store the winning combinations
    // to avoid repetitive code
    // Store all eight winning lines as triples of indices into the 0..8 board.
    static const int wins[8][3] = {
        {0,1,2}, {3,4,5}, {6,7,8},
        {0,3,6}, {1,4,7}, {2,5,8},
        {0,4,8}, {2,4,6}
    };

    for (const auto& line : wins)
    {
        Player* a = ownerAt(line[0]);
        if (!a)
            continue;

        Player* b = ownerAt(line[1]);
        Player* c = ownerAt(line[2]);

        if (a == b && a == c)
            return a;
    }
    return nullptr;
}

bool TicTacToe::checkForDraw()
{
    // is the board full with no winner?
    // if any square is empty, return false
    // otherwise return true
    // A draw only happens when the board is full and there is no winner.
    if (checkForWinner() != nullptr)
        return false;

    for (int row = 0; row < 3; ++row)
    {
        for (int col = 0; col < 3; ++col)
        {
            if (_grid[row][col].bit() == nullptr)
                return false;
        }
    }

    return true;
}

//
// state strings
//
std::string TicTacToe::initialStateString()
{
    return "000000000";
}

//
// this still needs to be tied into imguis init and shutdown
// we will read the state string and store it in each turn object
//
std::string TicTacToe::stateString() const
{
    // return a string representing the current state of the board
    // the string should be 9 characters long, one for each square
    // each character should be '0' for empty, '1' for player 1 (X), and '2' for player 2 (O)
    // the order should be left-to-right, top-to-bottom
    // for example, the starting state is "000000000"
    // if player 1 has placed an X in the top-left and player 2 an O in the center, the state would be "100020000"
    // you can build the string using a loop and the to_string function
    // for example, to convert an integer to a string, you can use std::to_string(1) which returns "1"
    // you can get the bit at each square using _grid[y][x].bit()
    // if the bit is not null, you can get its owner using bit->getOwner()->playerNumber()
    // remember that player numbers are zero-based, so add 1 to get '1' or '2'
    // if the bit is null, add '0' to the string
    // finally, return the constructed string
    // Build a 9-character string (left-to-right, top-to-bottom).
    std::string state;
    state.reserve(9);

    for (int row = 0; row < 3; ++row)
    {
        for (int col = 0; col < 3; ++col)
        {
            Bit* b = _grid[row][col].bit();
            if (!b)
            {
                state.push_back('0');
                continue;
            }

            // Player numbers are 0-based; store them as '1' or '2'.
            int pn = b->getOwner()->playerNumber();
            state.push_back(static_cast<char>('1' + pn));
        }
    }

    return state;
}

//
// this still needs to be tied into imguis init and shutdown
// when the program starts it will load the current game from the imgui ini file and set the game state to the last saved state
//
void TicTacToe::setStateString(const std::string &s)
{
    // set the state of the board from the given string
    // the string will be 9 characters long, one for each square
    // each character will be '0' for empty, '1' for player 1 (X), and '2' for player 2 (O)
    // the order will be left-to-right, top-to-bottom
    // for example, the starting state is "000000000"
    // if player 1 has placed an X in the top-left and player 2 an O in the center, the state would be "100020000"
    // you can loop through the string and set each square in _grid accordingly
    // for example, if s[0] is '1', you would set _grid[0][0] to have player 1's piece
    // if s[4] is '2', you would set _grid[1][1] to have player 2's piece
    // if s[8] is '0', you would set _grid[2][2] to be empty
    // you can use the PieceForPlayer function to create a new piece for a player
    // remember to convert the character to an integer by subtracting '0'
    // for example, int playerNumber = s[index] - '0';
    // if playerNumber is 0, set the square to empty (nullptr)
    // if playerNumber is 1 or 2, create a piece for that player and set it in the square
    // finally, make sure to position the piece at the holder's position
    // you can get the position of a holder using holder->getPosition()
    // loop through the 3x3 array and set each square accordingly
    // the string should always be valid, so you don't need to check its length or contents
    // but you can assume it will always be 9 characters long and only contain '0', '1', or '2'

    // Clear existing pieces before applying the saved state.
    stopGame();

    int placedCount = 0;
    for (int index = 0; index < 9; ++index)
    {
        const int row = index / 3;
        const int col = index % 3;
        const char c = s[index];

        if (c == '0')
            continue;

        // Saved format: '1' means player 0, '2' means player 1.
        const int savedPlayerIndex = (c - '1');
        if (savedPlayerIndex < 0 || savedPlayerIndex > 1)
            continue;

        Bit* b = PieceForPlayer(savedPlayerIndex);
        b->setPosition(_grid[row][col].getPosition());
        _grid[row][col].setBit(b);
        ++placedCount;
    }

    // Set the current turn so the next player is consistent with the loaded board.
    _gameOptions.currentTurnNo = static_cast<unsigned int>(placedCount);
}


//
// this is the function that will be called by the AI
//
void TicTacToe::updateAI()
{
    std::string s = stateString();

    int bestScore = -100000;
    int bestMove = -1;

    // AI is '2' (second player)
    for (int i = 0; i < 9; ++i)
    {
        if (s[i] != '0')
            continue;

        s[i] = '2';                             // try AI move
        int score = negamax(s, '1', 1);         // next turn is HUMAN (player = +1)
        s[i] = '0';

        if (score > bestScore)
        {
            bestScore = score;
            bestMove = i;
        }
    }

    if (bestMove != -1)
    {
        int row = bestMove / 3;
        int col = bestMove % 3;

        actionForEmptyHolder(&_grid[row][col]);
        endTurn(); // Game.cpp does NOT endTurn() for AI, so this stays here
    }
}


int TicTacToe::negamax(const std::string& state, char turnChar, int depth)
{
    static const int wins[8][3] = {
        {0,1,2},{3,4,5},{6,7,8},
        {0,3,6},{1,4,7},{2,5,8},
        {0,4,8},{2,4,6}
    };

    // Terminal winner check
    for (const auto& w : wins)
    {
        char a = state[w[0]];
        if (a != '0' && a == state[w[1]] && a == state[w[2]])
        {
            // Score from AI perspective, prefer faster outcomes
            if (a == '2') return 10 - depth;      // AI win
            else          return -(10 - depth);   // Human win
        }
    }

    // Draw check
    bool full = true;
    for (char c : state)
    {
        if (c == '0') { full = false; break; }
    }
    if (full) return 0;

    // Recurrence: maximize for AI, minimize for human
    bool maximizing = (turnChar == '2');
    int best = maximizing ? -100000 : 100000;

    for (int i = 0; i < 9; ++i)
    {
        if (state[i] != '0')
            continue;

        std::string next = state;
        next[i] = turnChar;

        char nextTurn = (turnChar == '1') ? '2' : '1';
        int score = negamax(next, nextTurn, depth + 1);

        if (maximizing)
            best = (score > best) ? score : best;
        else
            best = (score < best) ? score : best;
    }

    return best;
}
