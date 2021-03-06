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
 * \brief The ConnectFour class represents a Connect Four game.
 *
 */

#ifndef CONNECTFOUR_H
#define CONNECTFOUR_H

#include "LinearGame.h"

class ConnectFour : public LinearGame
{
    public:
        // Constructor
        explicit ConnectFour(GameType ecGameType) : LinearGame(ecGameType, 7, 6, '1', '2', 4, true, true, true, false) {}

        // Destructor
        ~ConnectFour() {}

        // Provide a preferred move
        virtual int  PreferredMove(const GameMove &cGameMove) const override;

        // Apply the move to the game
        virtual bool ApplyMove(int nPlayer, GameMove &cGameMove) override;

        // Generate a vector of valis moves
        virtual std::vector<GameMove> GenerateMoves(int nPlayer) override;

        // Clone the current game
        virtual std::unique_ptr<Game> Clone() const override { return std::unique_ptr<Game>(new ConnectFour(*this)); }

        // Return the title of the game
        virtual std::string Title() const override { return "Connect Four"; }

        // Return the description of the game
        virtual std::string Description() const override { return ""; }

    private:
        // Return the bottom row in the Connect Four grid
        int  FindBottom(int x) const;
};

#endif // CONNECTFOUR_H
