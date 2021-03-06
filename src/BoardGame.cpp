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

#include "BoardGame.h"

/**
  * Display the game board.
  *
  * Run through every space on the board and display it's token.  Also display
  * coordinates and grid lines, depending on settings.
  *
  */

void BoardGame::Display() const
{
    cBoard.Display();
}

/**
  * Return information to be synchronized and sent to a networked opponent
  *
  * This function is a NOP and should be overridden in derived classes.
  *
  * \param sGameInformation String representing game information to be synchronized
  * between players.
  *
  * \return True if information is available to be sent, false otherwise.
  */

bool BoardGame::GetSyncInfo(std::string &sGameInformation)
{
    (void) sGameInformation;
    return false;
}

/**
  * Receive information to be synchronized from a networked opponent
  *
  * This function is a NOP and should be overridden in derived classes.
  *
  * \return True if information is available to be sent, false otherwise.
  */

bool BoardGame::ApplySyncInfo(const std::string &sGameInformation, std::string &sErrorMessage)
{
    (void) sGameInformation;
    (void) sErrorMessage;
    return true;
}


/**
  * Return a string of valid moves.
  *
  * Call GenerateMoves and collect information into a string.
  *
  * \param nPlayer The player whose turn it is.
  *
  * \return A string of valid moves.
  */

std::string BoardGame::ValidMoves(int nPlayer)
{
    std::string sValidMoves {};

    std::vector<GameMove> vGameMoves = GenerateMoves(nPlayer);

    if (vGameMoves.empty())
    {
        sValidMoves = "NO VALID MOVE";
    }
    else
    {
        for (GameMove cGameMove : vGameMoves)
        {
            sValidMoves += cGameMove.AnnounceFromMove() + cGameMove.AnnounceToMove() + " ";
        }
    }

    return sValidMoves;
}

/**
  * Get the player's move.
  *
  * From std::cin, generate a GameMove object.
  *
  * \param nPlayer The player whose turn it is.
  *
  * \return A GameMove object.
  */

GameMove BoardGame::GetMove(int nPlayer) const
{
    (void)nPlayer;

    std::string sMove {};

    std::cin >> sMove;

    // Genterate and return a GameMove
    return GenerateMove(sMove);
}

/**
  * Return a GameMove object from a string representing a game move.
  *
  * Create a GameMove object from the passed string.  Check for a resignation.
  *
  * \param sMove The string representing a game move.
  *
  * \return A GameMove object.
  */


GameMove BoardGame::GenerateMove(std::string sMove) const
{
    // Generic GamevMove
    GameMove cGameMove(-1, -1, -1, -1, false, false);

    // Upper case the move string
    std::locale loc;
    for (std::string::size_type iii = 0; iii < sMove.length(); ++iii)
        sMove[iii] = std::toupper(sMove[iii], loc);

    // Compare move against resignation
    if (sMove.compare(GameVocabulary::RESIGN) == 0)
    {
        cGameMove.SetResignation(true);
        return cGameMove;
    }

    return cGameMove;
}

/**
  * Return a measure of the preference of a move.
  *
  * This function is a NOP and should be overridden in derived classes.
  *
  * \param cGameMove  The GameMove to be evaluated.
  *
  * \return 0
  */

int BoardGame::PreferredMove(const GameMove &cGameMove) const
{
    (void)cGameMove;

    return 0;
}

/**
  * Return a  string providing a current score of the game.
  *
  * This function is a NOP and should be overridden in derived classes.
  *
  * \return ""
  */

std::string BoardGame::GameScore() const
{
    return "";
}

/**
  * Check to see if a player has won the game.
  *
  * If the last move made was a resignation, return true.
  *
  * \param nPlayer The player
  *
  * \return True, if any player has won the game.  False otherwise.
  */

bool BoardGame::GameEnded(int nPlayer)
{
    if (!m_vGameMoves.empty())
    {
        GameMove cGameMove = m_vGameMoves.back();
        if (cGameMove.Resignation())
        {
            m_nWinner = nPlayer;
            m_sWinBy.assign("resignation");
            m_bGameOver = true;
            return true;
        }
    }

    return false;
}
