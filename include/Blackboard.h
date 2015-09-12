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
 * \brief The Blackboard class represents a knowledge base of game information
 * for a AIPlayer.
 *
 */

#ifndef BLACKBOARD_H
#define BLACKBOARD_H

#include "Deck.h"
#include "Hand.h"

class Blackboard
{
    public:
        Blackboard() {};
        virtual ~Blackboard() {};

        // Manage Initialization
        bool Initialized()     { return m_bInitialized; }
        void SetInitialized()  { m_bInitialized = true; }

        //
        // Public members
        //
        ProbableDeck m_cProbableDeck;
        ProbableHand m_cProbableOpponentHand;
        std::map<std::string, int> m_mAsks {};

    private:
        bool m_bInitialized {false};
};

#endif // BLACKBOARD_H
