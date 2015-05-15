/*
    Copyright 2015 Dom Maddalone

    This file is part of GameAI.

    GameAI is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    GameAI is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with GameAI.  If not, see <http://www.gnu.org/licenses/>.
*/

/** \file
 *
 * \brief The BoardGame class represents a virtual game played on a
 * two-dimensonal space, navigated with X and Y coordinates.
 *
 */

#ifndef BOARDGAME_H
#define BOARDGAME_H

#include "Game.h"
#include "GameBoard.h"

class BoardGame : public Game
{
    public:
        // Construct a BoardGame
        //BoardGame(GameType ecGameType, int nX, int nY, char cToken1, char cToken2, int nInARow, bool bUseY, bool bDisplayGrid, bool bDisplayXCoordinates, bool bDisplayYCoordinates) :
        BoardGame(GameType ecGameType, int nX, int nY, bool bDisplayGrid, bool bDisplayXCoordinates, bool bDisplayYCoordinates) :
            Game(ecGameType),
            m_knX(nX),
            m_knY(nY),
            //m_kcToken1(cToken1),
            //m_kcToken2(cToken2),
            //m_knTokensInARowWin(nInARow),
            //m_bUseY(bUseY),
            cBoard(nX, nY, bDisplayGrid, bDisplayXCoordinates, bDisplayYCoordinates)
            {  SetBoard(); }

        // Destructor
        virtual ~BoardGame() {}

        // Display the game board
        virtual void Display() const override;
        // Return a list of valid moves in string format
        virtual std::string ValidMoves(int nPlayer) const override;
        // Get the move from the designated player
        virtual GameMove GetMove(int nPlayer) const override;
        // Provide a preferred move
        virtual int  PreferredMove(const GameMove &cGameMove) const override;
        // Return the score of the game
        virtual std::string GameScore() const override;
        // Clone the current game
        virtual std::unique_ptr<Game> Clone() const = 0;

    protected:
        //// Set up the board
        void SetBoard() { cBoard.Clear(); return; };

        // Max X-coordinate for this game
        const int  m_knX;
        // Max Y-coordinate this game
        const int  m_knY;

        // Max X-coordinate for any board game
        static const int m_knMaxX {8};
        // Max Y-coordinate for any board game
        static const int m_knMaxY {8};

        // The game board, initialized in the constructor
        GameBoard cBoard;
};

#endif // BOARDGAME_H
