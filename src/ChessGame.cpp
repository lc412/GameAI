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

#include "ChessGame.h"

/**
  * Generate a game description.
  *
  * This class will create a titlemade up of Chess game
  * options.
  *
  * \return A string containing the game title.
  */

std::string ChessGame::Description() const
{
    std::string sMessage {};

    sMessage += "Castling is ";

    if (m_abCastlingAllowed[0])
    {
        sMessage += "Allowed";
    }
    else
    {
        sMessage += "Not Allowed";
    }

    sMessage += "\nDouble Pawn Move is ";

    if (m_bDoublePawnMoveAllowed)
    {
        sMessage += "Allowed";
    }
    else
    {
        sMessage += "Not Allowed";
    }

    sMessage += "\nEn Passant Move is ";

    if (m_bEnPassantAllowed)
    {
        sMessage += "Allowed";
    }
    else
    {
        sMessage += "Not Allowed";
    }

    sMessage += "\nAutomatic Pawn Promotion to Queen: ";

    if (m_bAutomaticPromoteToQueen)
    {
        sMessage += "True";
    }
    else
    {
        sMessage += "False";
    }

    return sMessage;
}


/**
  * Set up the game board.
  *
  * Call parent class SetBoard and reverse the Y-coordinates
  */

void ChessGame::SetBoard()
{
    BoardGame::SetBoard();
    cBoard.ReverseY();
}


/**
  * Generate a GameMove from a string.
  *
  * From a string, generate a GameMove object.
  *
  * move      ::= <square><square>[promotion]
  * square    ::= <'a'|'b'| ...><'1'|'2'|...>
  * promotion ::= <'='><piece>      // TODO
  * piece     ::= <'R'|'N'|'B'|'Q'> // TODO
  *
  * \param sMove A string representing a game move.
  *
  * \return A GameMove object.
  */

GameMove ChessGame::GenerateMove(std::string sMove) const
{
    GameMove cGameMove = BoardGame::GenerateMove(sMove);

    if (cGameMove.Resignation())
        return cGameMove;

    cGameMove.SetFromX(sMove[0]);
    cGameMove.SetFromY(sMove[1]);
    cGameMove.SetToX(sMove[2]);
    cGameMove.SetToY(sMove[3]);
    cGameMove.SetUseFrom(true);
    cGameMove.SetUseY(true);

    return cGameMove;
}

/**
  * Add a move to the vector of game moves after testing the move to see if it
  * results in the player's King in check.
  *
  * If the GameMove is a TestMove (a look ahead move), add it to the vector.
  * If not, evaluate if the move allows a check to the King.  If not, add it
  * to the vector.
  *
  * \param nPlayer Number of the player
  * \param cGameMove The game move
  * \param vGameMoves The vector of game moves
  */

void ChessGame::TestForCheck(int nPlayer, GameMove cGameMove, std::vector<GameMove> &vGameMoves) const
{
    // If this is a test move, add it to the vector.
    // We are not looking ahead more than one move.
    if (cGameMove.TestMove())
    {
        vGameMoves.push_back(cGameMove);
    }
    else // Not a test move
    {
        // Set as test move
        cGameMove.SetTestMove(true);
        // Clone the game
        std::unique_ptr<Game> pcGameClone = Clone();
        // Apply the move to cloned game.  If a valid move, set
        // test move to false and add the move to the vector.
        if (pcGameClone->ApplyMove(nPlayer, cGameMove))
        {
            cGameMove.SetTestMove(false);
            vGameMoves.push_back(cGameMove);
        }
        else // Otherwise set the test move to false
        {
            cGameMove.SetTestMove(false);
        }

    }
}

/**
  * Return a vector of valid game moves.
  *
  * Review and collect all valid moves for a player into a vector.
  *
  * \param nPlayer The player whose turn it is.
  *
  * \return A vector of valid moves.
  */

std::vector<GameMove> ChessGame::GenerateMoves(int nPlayer)
{
    std::vector<GameMove> vGameMoves {};
    GameMove cGameMove;
    char cToken;

    // Run through all squares on the board
    for (int yyy = 0; yyy < m_knY; ++yyy)
    {
        for (int xxx = 0; xxx < m_knX; ++xxx)
        {
            // If the square is occupied by this player ...
            if (cBoard.PositionOccupiedByPlayer(xxx, yyy, nPlayer))
            {
                cGameMove.SetFromX(xxx);
                cGameMove.SetFromY(yyy);
                cGameMove.SetUseFrom(true);

                // Generate the appropriate moves depending on the piece token
                cToken = cBoard.Token(xxx, yyy);

                if (cToken == m_kcPawnToken)
                    GeneratePawnMoves(cGameMove, nPlayer, vGameMoves);

                if (cToken == m_kcRookToken)
                    GenerateRookMoves(cGameMove, nPlayer, vGameMoves);

                if (cToken == m_kcKnightToken)
                    GenerateKnightMoves(cGameMove, nPlayer, vGameMoves);

                if (cToken == m_kcBishopToken)
                    GenerateBishopMoves(cGameMove, nPlayer, vGameMoves);

                if (cToken == m_kcQueenToken)
                    GenerateQueenMoves(cGameMove, nPlayer, vGameMoves);

                if (cToken == m_kcKingToken)
                    GenerateKingMoves(cGameMove, nPlayer, vGameMoves);
            }
        }
    }

    return vGameMoves;
}

/**
  * Generate moves for a pawn.
  *
  * Review and collect all valid moves for a pawn.
  *
  * \param cGameMove The game move under consideration
  * \param nPlayer The player whose turn it is.
  * \param vGameMoves The vector to add valid moves to
  */

void ChessGame::GeneratePawnMoves(GameMove cGameMove, int nPlayer, std::vector<GameMove> &vGameMoves) const
{
    const int knX = cGameMove.FromX();
    const int knY = cGameMove.FromY();
    int nNewY;
    int nIntermediateY;

    //
    // Push
    //
    if (nPlayer == 1)
        nNewY = knY + 1;
    else
        nNewY = knY - 1;

    if (!cBoard.ValidLocation(knX, nNewY))
        return;

    if (!cBoard.PositionOccupied(knX, nNewY))
    {
        cGameMove.SetToX(knX);
        cGameMove.SetToY(nNewY);
        TestForCheck(nPlayer, cGameMove, vGameMoves);
    }

    //
    // Capture
    //
    if (cBoard.PositionOccupiedByPlayer(knX - 1, nNewY, 3 - nPlayer))
    {
        cGameMove.SetToX(knX - 1);
        cGameMove.SetToY(nNewY);
        TestForCheck(nPlayer, cGameMove, vGameMoves);
    }

    if (cBoard.PositionOccupiedByPlayer(knX + 1, nNewY, 3 - nPlayer))
    {
        cGameMove.SetToX(knX + 1);
        cGameMove.SetToY(nNewY);
        TestForCheck(nPlayer, cGameMove, vGameMoves);
    }

    //
    // Double move
    //
    if (m_bDoublePawnMoveAllowed)
    {
        GamePiece cPiece = cBoard.Piece(knX, knY);
        if (!cPiece.HasMoved())
        {
            if (nPlayer == 1)
            {
                nNewY = knY + 2;
                nIntermediateY = knY + 1;
            }
            else
            {
                nNewY = knY - 2;
                nIntermediateY = knY - 1;
            }

            if ((!cBoard.PositionOccupied(knX, nIntermediateY)) && (!cBoard.PositionOccupied(knX, nNewY)))
            {
                cGameMove.SetToX(knX);
                cGameMove.SetToY(nNewY);
                TestForCheck(nPlayer, cGameMove, vGameMoves);
            }
        }
    }

    //
    // En passant
    //
    if (m_bEnPassantAllowed)
    {
        // Get last applied game move
        GameMove cLastGameMove = LastMove();

        // Check to see if there was a move
        if (!cLastGameMove.NoMove())
        {
            // If last move was an opponent Pawn
            if (cBoard.Token(cLastGameMove.ToX(), cLastGameMove.ToY()) == m_kcPawnToken)
            {
                // If the opponent Pawn moved two squares
                if (abs((cLastGameMove.FromY() - cLastGameMove.ToY())) == 2)
                {
                    // If the opponent Pawn's rank now equals this Pawn's rank
                    if (cLastGameMove.ToY() == knY)
                    {
                        // If the Pawns' files are adjacent
                        if ((knX - cLastGameMove.FromX() == 1) || (cLastGameMove.FromX() - knX == 1))
                        {
                            // Set To X coordinate
                            cGameMove.SetToX(cLastGameMove.FromX());

                            // Find Y-direction and set Y coordinate
                            if (cLastGameMove.FromY() > cLastGameMove.ToY())
                            {
                                cGameMove.SetToY(cLastGameMove.FromY() - 1);
                            }
                            else
                            {
                                cGameMove.SetToY(cLastGameMove.FromY() + 1);
                            }

                            TestForCheck(nPlayer, cGameMove, vGameMoves);
                        } // If the Pawns' files are adjacent
                    }  // If the opponent Pawn's rank now equals this Pawn's rank
                } // If the Pawn moved two squares
            } // If last move was a Pawn
        } // Check to see if there was a move
    } //  if (m_bEnPassantAllowed)
}

/**
  * Generate moves for a Rook, Queen, or King.
  *
  * Review and collect all valid moves for a Rook, Queen, or King
  *
  * \param cGameMove The game move under consideration
  * \param nPlayer The player whose turn it is.
  * \param vGameMoves The vector to add valid moves to
  * \param bUnlimitedMoves Whether to to limit moves to one square (used for the King)
  */

void ChessGame::GenerateRookMoves(GameMove cGameMove, int nPlayer, std::vector<GameMove> &vGameMoves, bool bUnlimitedMoves) const
{
    const int knX = cGameMove.FromX();
    const int knY = cGameMove.FromY();
    int nNewX;
    int nNewY;

    // Evaluate north along file
    cGameMove.SetToX(knX);
    nNewY = knY + 1;
    while (cBoard.ValidLocation(knX, nNewY))
    {
        cGameMove.SetToY(nNewY);
        if (GenerateLinearMove(cGameMove, nPlayer, vGameMoves))
            ++nNewY;
        else
            break;

        if (!bUnlimitedMoves) // Limit the King move to one square
            break;
    }

    // Evaluate south along file
    cGameMove.SetToX(knX);
    nNewY = knY - 1;
    while (cBoard.ValidLocation(knX, nNewY))
    {
        cGameMove.SetToY(nNewY);
        if (GenerateLinearMove(cGameMove, nPlayer, vGameMoves))
            --nNewY;
        else
            break;

        if (!bUnlimitedMoves) // Limit the King move to one square
            break;
    }

    // Evaluate east along rank
    cGameMove.SetToY(knY);
    nNewX = knX - 1;
    while (cBoard.ValidLocation(nNewX, knY))
    {
        cGameMove.SetToX(nNewX);
        if (GenerateLinearMove(cGameMove, nPlayer, vGameMoves))
            --nNewX;
        else
            break;

        if (!bUnlimitedMoves) // Limit the King move to one square
            break;
    }

    // Evaluate west along rank
    cGameMove.SetToY(knY);
    nNewX = knX + 1;
    while (cBoard.ValidLocation(nNewX, knY))
    {
        cGameMove.SetToX(nNewX);
        if (GenerateLinearMove(cGameMove, nPlayer, vGameMoves))
            ++nNewX;
        else
            break;

        if (!bUnlimitedMoves) // Limit the King move to one square
            break;
    }
}

/**
  * Generate moves for a Bishop, Queen or King.
  *
  * Review and collect all valid moves for a Bishop, Queen, or a King
  *
  * \param cGameMove The game move under consideration
  * \param nPlayer The player whose turn it is.
  * \param vGameMoves The vector to add valid moves to
  * \param bUnlimitedMoves Whether to to limit moves to one square (use d for the King)
  */

void ChessGame::GenerateBishopMoves(GameMove cGameMove, int nPlayer, std::vector<GameMove> &vGameMoves, bool bUnlimitedMoves) const
{
    const int knX = cGameMove.FromX();
    const int knY = cGameMove.FromY();
    int nNewX;
    int nNewY;

    // Evaluate north-west
    nNewX = knX + 1;
    nNewY = knY + 1;
    while (cBoard.ValidLocation(nNewX, nNewY))
    {
        cGameMove.SetToX(nNewX);
        cGameMove.SetToY(nNewY);
        if (GenerateLinearMove(cGameMove, nPlayer, vGameMoves))
        {
            ++nNewX;
            ++nNewY;
        }
        else
            break;

        if (!bUnlimitedMoves) // Limit the King move to one square
            break;
    }

    // Evaluate south-west
    nNewX = knX + 1;
    nNewY = knY - 1;
    while (cBoard.ValidLocation(nNewX, nNewY))
    {
        cGameMove.SetToX(nNewX);
        cGameMove.SetToY(nNewY);
        if (GenerateLinearMove(cGameMove, nPlayer, vGameMoves))
        {
            ++nNewX;
            --nNewY;
        }
        else
            break;

        if (!bUnlimitedMoves) // Limit the King move to one square
            break;
    }

    // Evaluate south-east
    nNewX = knX - 1;
    nNewY = knY - 1;
    while (cBoard.ValidLocation(nNewX, nNewY))
    {
        cGameMove.SetToX(nNewX);
        cGameMove.SetToY(nNewY);
        if (GenerateLinearMove(cGameMove, nPlayer, vGameMoves))
        {
            --nNewX;
            --nNewY;
        }
        else
            break;

        if (!bUnlimitedMoves) // Limit the King move to one square
            break;
    }

    // Evaluate north-east
    nNewX = knX - 1;
    nNewY = knY + 1;
    while (cBoard.ValidLocation(nNewX, nNewY))
    {
        cGameMove.SetToX(nNewX);
        cGameMove.SetToY(nNewY);
        if (GenerateLinearMove(cGameMove, nPlayer, vGameMoves))
        {
            --nNewX;
            ++nNewY;
        }
        else
            break;

        if (!bUnlimitedMoves) // Limit the King move to one square
            break;
    }
}

/**
  * Generate moves for a Queen.
  *
  * Review and collect all valid moves for a Queen
  *
  * \param cGameMove The game move under consideration
  * \param nPlayer The player whose turn it is.
  * \param vGameMoves The vector to add valid moves to
  */

void ChessGame::GenerateQueenMoves(GameMove cGameMove, int nPlayer, std::vector<GameMove> &vGameMoves) const
{
    GenerateRookMoves(cGameMove, nPlayer, vGameMoves, true);
    GenerateBishopMoves(cGameMove, nPlayer, vGameMoves, true);
}

/**
  * Generate moves for a King.
  *
  * Review and collect all valid moves for a King
  *
  * \param cGameMove The game move under consideration
  * \param nPlayer The player whose turn it is.
  * \param vGameMoves The vector to add valid moves to
  */

void ChessGame::GenerateKingMoves(GameMove cGameMove, int nPlayer, std::vector<GameMove> &vGameMoves) const
{
    GenerateRookMoves(cGameMove, nPlayer, vGameMoves, false);
    GenerateBishopMoves(cGameMove, nPlayer, vGameMoves, false);
    GenerateCastleMoves(cGameMove, nPlayer, vGameMoves);
}

/**
  * Generate castle moves.
  *
  * \param cGameMove The game move under consideration
  * \param nPlayer The player whose turn it is.
  * \param vGameMoves The vector to add valid moves to
  */

void ChessGame::GenerateCastleMoves(GameMove cGameMove, int nPlayer, std::vector<GameMove> &vGameMoves) const
{
    // King's From coordinates
    const int knKX = cGameMove.FromX();
    const int knKY = cGameMove.FromY();

    // Used to test for  check on the intermediate castle move
    std::vector<GameMove> vIntermediateCastleMove {};

    // If castling is allowed, continue
    if (m_abCastlingAllowed[nPlayer - 1])
    {
        // Cannot castle if the King is in check
        if (KingInCheck(nPlayer))
            return;

        // Used to locate the Rook(s)
        int nRX    {0};
        int nRY    {0};

        // Ending King location on a castle move
        int nNewKX {0};
        // Intermediate King location on a castle move
        int nIntermediateKX {0};

        bool bCastleValid {false};

        // Find all Rooks
        while (FindPiece(nRX, nRY, nPlayer, m_kcRookToken))
        {
            GamePiece cRook = cBoard.Piece(nRX, nRY);
            // If Rook has not moved, continue
            if (!cRook.HasMoved())
            {
                // Assume a castle is valid
                bCastleValid = true;

                // If the Rook is to the west of the King ...
                if (nRX < knKX)
                {
                    // New end location for the King
                    nNewKX = knKX - 2;
                    // Intermediate location for the King
                    nIntermediateKX = knKX - 1;
                    // Ensure it is clear between Rook and the King
                    for (int xxx = nRX + 1; xxx != knKX; ++xxx)
                    {
                        if (cBoard.PositionOccupied(xxx, nRY))
                        {
                            bCastleValid = false;
                            break;
                        }
                    }
                }
                else // The Rook is to the East of the King
                {
                    // New end location for the King
                    nNewKX = knKX + 2;
                    // Intermediate location for the King
                    nIntermediateKX = knKX + 1;
                    // Ensure it is clear between Rook and the King
                    for (int xxx = nRX - 1; xxx != knKX; --xxx)
                    {
                        if (cBoard.PositionOccupied(xxx, nRY))
                        {
                            bCastleValid = false;
                            break;
                        }
                    }
                }

                // If castle is still valid after previous tests, continue
                if (bCastleValid)
                {
                    // Check that the intermediate castling move does not move
                    // the King adjacent to the opposing King
                    cGameMove.SetToX(nIntermediateKX);
                    cGameMove.SetToY(knKY);

                    if (!TestForAdjacentKings(cGameMove, nPlayer))
                    {
                        // Ensure the King is not in check on the intermediate castling move
                        TestForCheck(nPlayer, cGameMove, vIntermediateCastleMove);
                        if (!vIntermediateCastleMove.empty())
                        {

                            // Check that the end castling move does not move
                            // the King adjacent to the opposing King
                            cGameMove.SetToX(nNewKX);

                            if (!TestForAdjacentKings(cGameMove, nPlayer))
                            {
                                 // Ensure the King is not in check on the end castling move.
                                 // This method will add  the castle move as valid.
                                TestForCheck(nPlayer, cGameMove, vGameMoves);
                            }
                        }
                    }
                } // bValidCastle
            } // !cRook.HasMoved()

            // Update coordinates to find another Rook
            if (nRX < m_knX - 1)
            {
                ++nRX;
            }
            else
            {
                nRX = 0;
                ++nRY;
            }
        } // end of while
    } // end of if castling is allowed
}

/**
  * Generic method to test and add linear moves to the game moves vector.
  *
  * Evaluates a move.  If legal before considering adjacent Kings or checks, it
  * returns true.  True means continue checking for valid linear moves. It calls
  * TestForCheck() to add the move to the vector of moves.
  *
  * \param cGameMove The game move under consideration
  * \param nPlayer The player whose turn it is.
  * \param vGameMoves The vector to add valid moves to\
  *
  * \return True if game move is valid before considering adjacent kings or a check.  False otherwise.
  */

bool ChessGame::GenerateLinearMove(GameMove cGameMove, int nPlayer, std::vector<GameMove> &vGameMoves) const
{
    const int knToX = cGameMove.ToX();
    const int knToY = cGameMove.ToY();

    //
    // Move
    //

    // If To position is not occupied
    if (!cBoard.PositionOccupied(knToX, knToY))
    {
        // If this move results in adjacent Kings, return true, but do not add it to the vector of moves
        if (TestForAdjacentKings(cGameMove, nPlayer))
            return true;

        // Procedure tests for a King check before adding the move to the vector
        TestForCheck(nPlayer, cGameMove, vGameMoves);

        // Return true because the move was valid before considering adjacent Kings or checks
        return true;
    }
    else if (cBoard.PositionOccupiedByPlayer(knToX, knToY, nPlayer))
    {
        // Return false because the move cannot be made
        return false;
    }

    //
    // Capture
    //
    else //if (cBoard.PositionOccupiedByPlayer(nToX, nToY, 3 - nPlayer))
    {
        // If this move results in adjacent Kings, return true, but do not add it to the vector of moves
        if (TestForAdjacentKings(cGameMove, nPlayer))
            return false;

        // Procedure tests for a King check before adding the move to the vector
        TestForCheck(nPlayer, cGameMove, vGameMoves);

        // Return false because no more linear moves may be made and evaluated
        return false;
    }
}

/**
  * Generate moves for a Knight.
  *
  * Review and collect all valid moves for a Knight
  *
  * \param cGameMove The game move under consideration
  * \param nPlayer The player whose turn it is.
  * \param vGameMoves The vector to add valid moves to
  */

void ChessGame::GenerateKnightMoves(GameMove cGameMove, int nPlayer, std::vector<GameMove> &vGameMoves) const
{
    const int knX = cGameMove.FromX();
    const int knY = cGameMove.FromY();

    // Two up, one right
    cGameMove.SetToX(knX + 2);
    cGameMove.SetToY(knY + 1);
    GenerateKnightMove(cGameMove, nPlayer, vGameMoves);

    // One up, two right
    cGameMove.SetToX(knX + 1);
    cGameMove.SetToY(knY + 2);
    GenerateKnightMove(cGameMove, nPlayer, vGameMoves);

    // One down, two right
    cGameMove.SetToX(knX - 1);
    cGameMove.SetToY(knY + 2);
    GenerateKnightMove(cGameMove, nPlayer, vGameMoves);

    // Two down, one right
    cGameMove.SetToX(knX - 2);
    cGameMove.SetToY(knY + 1);
    GenerateKnightMove(cGameMove, nPlayer, vGameMoves);

    // Two down, one left
    cGameMove.SetToX(knX - 2);
    cGameMove.SetToY(knY - 1);
    GenerateKnightMove(cGameMove, nPlayer, vGameMoves);

    // One down, two left
    cGameMove.SetToX(knX - 1);
    cGameMove.SetToY(knY - 2);
    GenerateKnightMove(cGameMove, nPlayer, vGameMoves);

    // One up, two left
    cGameMove.SetToX(knX + 1);
    cGameMove.SetToY(knY - 2);
    GenerateKnightMove(cGameMove, nPlayer, vGameMoves);

    // Two up, one left
    cGameMove.SetToX(knX + 2);
    cGameMove.SetToY(knY - 1);
    GenerateKnightMove(cGameMove, nPlayer, vGameMoves);
}

/**
  * Generate a move for a Knight.
  *
  * RTest a specific move for the knight.
  *
  * \param cGameMove The game move under consideration
  * \param nPlayer The player whose turn it is.
  * \param vGameMoves The vector to add valid moves to
  */

void ChessGame::GenerateKnightMove(GameMove cGameMove, int nPlayer, std::vector<GameMove> &vGameMoves) const
{
    const int knToX = cGameMove.ToX();
    const int knToY = cGameMove.ToY();

    if (cBoard.ValidLocation(knToX, knToY))
    {
        if ((!cBoard.PositionOccupied(knToX, knToY)) || (cBoard.PositionOccupiedByPlayer(knToX, knToY, 3 - nPlayer)))
        {
            TestForCheck(nPlayer, cGameMove, vGameMoves);
        }

    }
}

/**
  * Apply a move to the game.
  *
  * After checking to for a move's validity, apply the move to the game.
  *
  * \param nPlayer   The player whose turn it is.
  * \param cGameMove The player's move
  *
  * \return True, if valid.  False otherwise.
  */

bool ChessGame::ApplyMove(int nPlayer, GameMove &cGameMove)
{
    bool bValidMove = false;
    char cToken;

    // Check player number
    if ((nPlayer != m_knPlayer1) && (nPlayer != m_knPlayer2))
        return false;

    // Check for resignation
    if (cGameMove.Resignation())
    {
        // Capture move for later playback or analysis
        m_vGameMoves.push_back(cGameMove);
        return true;
    }

    // Ensure we are on the board
    if (!cBoard.ValidLocation(cGameMove.FromX(), cGameMove.FromY()))
        return false;

    if (!cBoard.ValidLocation(cGameMove.ToX(), cGameMove.ToY()))
        return false;

    // Generate a vector of all possible valid moves for the piece on the from square
    std::vector<GameMove> vGameMoves = GenerateMovesForPiece(nPlayer, cGameMove);

    // Compare passed GameMove to generated game moves.  If one is found to be
    // the same, make the move on the board.
    for (GameMove &cValidGameMove : vGameMoves)
    {
        if (cValidGameMove.SameTo(cGameMove))
        {
            //
            // Update the ZobristKey to reflect the MOVE
            //

            // The piece on the FROM square
            cBoard.UpdateZobristKey(cGameMove.FromX(), cGameMove.FromY(), cGameMove.FromX(), cGameMove.FromY());
            // The piece on the TO square
            cBoard.UpdateZobristKey(cGameMove.FromX(), cGameMove.FromY(), cGameMove.ToX(), cGameMove.ToY());
            // If this was a capture, remove the captured piece from the Zobrist hash
            if (cBoard.PositionOccupied(cGameMove.ToX(), cGameMove.ToY()))
            {
                // The capture piece on the TO square
                cBoard.UpdateZobristKey(cGameMove.ToX(), cGameMove.ToY(), cGameMove.ToX(), cGameMove.ToY());
            }

            // Move the piece
            if (cBoard.MovePiece(cGameMove))
            {
                bValidMove = true;

                break;
            }
            else // If not a valid move, update the ZobristKey to reflect the UN-MOVE
            {

                cBoard.UpdateZobristKey(cGameMove.FromX(), cGameMove.FromY(), cGameMove.FromX(), cGameMove.FromY());
                cBoard.UpdateZobristKey(cGameMove.FromX(), cGameMove.FromY(), cGameMove.ToX(), cGameMove.ToY());
                if (cBoard.PositionOccupied(cGameMove.ToX(), cGameMove.ToY()))
                {
                    cBoard.UpdateZobristKey(cGameMove.ToX(), cGameMove.ToY(), cGameMove.ToX(), cGameMove.ToY());
                }
                return false;
            }
        }
    }

    // If King is in check after the move, the move is not valid // TODO: Should not ever trip this condition?
    if (KingInCheck(nPlayer))
    {
        bValidMove = false;
    }

    // If a valid move, capture it for later playback
    if (bValidMove)
    {
        // Increment move counter
        ++m_nNumberOfMoves;

        // Capture move for later playback or analysis
        m_vGameMoves.push_back(cGameMove);
    }
    else
    {
        return false;
    }

    //
    // Perform Pawn Promotion
    //
    cToken = cBoard.Token(cGameMove.ToX(), cGameMove.ToY());

    // If this is a pawn on the last or first rank, promote the pawn
    if ((cToken == m_kcPawnToken) && ((cGameMove.ToY() == m_knY - 1) || (cGameMove.ToY() == 0)))
    {
        bool bGoodToken = false;
        char cPromotion {};
        int  nIndexOffset = m_knPieceIndexOffset * (nPlayer - 1);

        GamePiece cGamePiece = cBoard.Piece(cGameMove.ToX(), cGameMove.ToY());

        while (!bGoodToken)
        {
            // If automatic Queen promotion has bee set, use the queen as the selected token
            if (m_bAutomaticPromoteToQueen)
            {
                cPromotion = m_kcQueenToken;
            }
            // Else ask for input from player.  //TODO: Make this work with AI.
            else
            {
                std::cout << "\nPAWN PROMOTION ("
                    << m_kcRookToken << ", "
                    << m_kcKnightToken << ", "
                    << m_kcBishopToken << ", "
                    << m_kcQueenToken << "): ";

                std::cin >> cPromotion;
                cPromotion = toupper(cPromotion);
            }

            // Based on selected token
            switch (cPromotion)
            {
                case m_kcRookToken:
                    cGamePiece.Set(cPromotion, nPlayer, m_knRookValue, m_knWhiteRookIndex + nIndexOffset);
                    bGoodToken = true;
                    break;
                case m_kcKnightToken:
                    cGamePiece.Set(cPromotion, nPlayer, m_knKnightValue, m_knWhiteKnightIndex + nIndexOffset);
                    bGoodToken = true;
                    break;
                case m_kcBishopToken:
                    cGamePiece.Set(cPromotion, nPlayer, m_knBishopValue, m_knWhiteBishopIndex + nIndexOffset);
                    bGoodToken = true;
                    break;
                case m_kcQueenToken:
                    cGamePiece.Set(cPromotion, nPlayer, m_knQueenValue, m_knWhiteQueenIndex + nIndexOffset);
                    bGoodToken = true;
                    break;
                default:
                    std::cout << "Invalid entry: " << cPromotion << std::endl;
                    break;
            }

            if (bGoodToken)
            {
                // Update the ZobristKey to reflect the removal of the pawn from the board
                cBoard.UpdateZobristKey(cGameMove.ToX(), cGameMove.ToY(), cGameMove.ToX(), cGameMove.ToY());

                // Promote the pawn on the board
                cBoard.SetPiece(cGameMove.ToX(), cGameMove.ToY(), cGamePiece);

                // Update the ZobristKey to reflect the promotion
                cBoard.UpdateZobristKey(cGameMove.ToX(), cGameMove.ToY(), cGameMove.ToX(), cGameMove.ToY());
            }
        }
    }

    //
    // Perform Second Half of Castling Move
    //

    // If castling allowed
    if (m_abCastlingAllowed[nPlayer -1])
    {
        cToken = cBoard.Token(cGameMove.ToX(), cGameMove.ToY());

        // If the token is a King
        if (cToken == m_kcKingToken)
        {
            int nX;
            int nY;
            bool bCastleWest = false;
            GameMove cRookMove;

            // if the King moved movre than 1 square
            if (abs(cGameMove.FromX() - cGameMove.ToX()) > 1)
            {
                bValidMove = false;

                //
                // Find Rook
                //

                // If King moved more than one square to the west
                if (cGameMove.FromX() - cGameMove.ToX() > 1)
                {
                    nX = 0;
                    bCastleWest = true;
                }
                // Else King moved more than one square to the west
                else
                {
                    nX = cGameMove.FromX() + 2;
                }

                // The King's from file
                nY = cGameMove.FromY();

                if (FindPiece(nX, nY, nPlayer, m_kcRookToken))
                {
                    //
                    // Move Rook
                    //
                    cRookMove.SetFromX(nX);
                    cRookMove.SetFromY(cGameMove.FromY());
                    cRookMove.SetToY(cGameMove.FromY());

                    if (bCastleWest)
                    {
                        cRookMove.SetToX(cGameMove.ToX() + 1);
                    }
                    else
                    {
                        cRookMove.SetToX(cGameMove.ToX() - 1);
                    }

                    // Update the ZobristKey to reflect the movement of the rook
                    cBoard.UpdateZobristKey(cGameMove.FromX(), cGameMove.FromY(), cGameMove.FromX(), cGameMove.FromY());

                    if (cBoard.MovePiece(cRookMove)) //TODO: Check return code
                    {
                        bValidMove = true;
                        m_abCastlingAllowed[nPlayer - 1] = false;
                        m_abKingCastled[nPlayer - 1] = true;

                        // Update the ZobristKey to reflect the movement of the rook
                        cBoard.UpdateZobristKey(cGameMove.ToX(), cGameMove.ToY(), cGameMove.ToX(), cGameMove.ToY());
                    }
                }
            }
        }
    }

    // If move is valid, insert current Zobrist hash into the container
    if (bValidMove) m_uomsZobrist.insert(cBoard.ZKey());

    return bValidMove;
}

/**
  * Generate a vector of possible moves for a piece located at GameMove
  * coordinates.
  *
  * \param nPlayer   The player whose turn it is.
  * \param cGameMove The player's move
  *
  * \return A vector of possible moves.
  */

std::vector<GameMove> ChessGame::GenerateMovesForPiece(int nPlayer, const GameMove &cGameMove) const
{
    std::vector<GameMove> vGameMoves {};

    if (cBoard.PositionOccupiedByPlayer(cGameMove.FromX(), cGameMove.FromY(), nPlayer))
    {
        char cToken = cBoard.Token(cGameMove.FromX(), cGameMove.FromY());

        if (cToken == m_kcPawnToken)
            GeneratePawnMoves(cGameMove, nPlayer, vGameMoves);

        if (cToken == m_kcRookToken)
            GenerateRookMoves(cGameMove, nPlayer, vGameMoves);

        if (cToken == m_kcKnightToken)
            GenerateKnightMoves(cGameMove, nPlayer, vGameMoves);

        if (cToken == m_kcBishopToken)
            GenerateBishopMoves(cGameMove, nPlayer, vGameMoves);

        if (cToken == m_kcQueenToken)
            GenerateQueenMoves(cGameMove, nPlayer, vGameMoves);

        if (cToken == m_kcKingToken)
            GenerateKingMoves(cGameMove, nPlayer, vGameMoves);
    }

    return vGameMoves;
}

/**
  * Find a chess piece.
  *
  * Using the passed coordinates as a starting place, find a piece on the
  * board moving left to right and up each row.  If the peice is found, update
  * the passed coordinates with the location and return true.
  *
  * \param nX      The X-Coordinate location to begin the search
  * \param nY      The Y-Coordinate location to begin the search
  * \param nPlayer The player to search for
  * \param cToken  The piece token to search for
  *
  * \return True if piece found, false otherwise.
  */

bool ChessGame::FindPiece(int &nX, int &nY, int nPlayer, char cToken) const
{
    for (int yyy = nY; yyy < m_knY; ++yyy)
    {
        for (int xxx = nX; xxx < m_knX; ++xxx)
        {
            if ((cBoard.Token(xxx, yyy) == cToken) && (cBoard.Player(xxx, yyy) == nPlayer))
            {
                nX = xxx;
                nY = yyy;
                return true;
            }
        }
    }

    return false;
}

/**
  * Evaluate move to see if the Kings are adjacent.
  *
  * Check whether the to be moved piece is a King.  If so, rotate around the To
  * coordinates to ensure that the opposing King is not adjacent.
  *
  * \param cGameMove The game move
  * \param nPlayer The player to search for
  *
  * \return True if Kings are adjacent, false otherwise.
  */

bool ChessGame::TestForAdjacentKings(const GameMove &cGameMove, int nPlayer) const
{
    // Grab piece on From coordinates.  If not a King, return false;
    GamePiece cPiece = cBoard.Piece(cGameMove.FromX(), cGameMove.FromY());
    if (cPiece.Token() != m_kcKingToken)
        return false;

    // Find the opposing King
    int nKX  {0};
    int nKY  {0};

    if (!FindPiece(nKX, nKY, 3 - nPlayer, m_kcKingToken))
    {
        std::string sErrorMessage  = "Could not find King for Player " + std::to_string(nPlayer);
        std::cerr << sErrorMessage << std::endl;
        std::cout << "Exiting" << std::endl;
        throw GameAIException(sErrorMessage);
    }

    // Rotate around the game move To coordinates looking for the opposing King
    int nToX = cGameMove.ToX();
    int nToY = cGameMove.ToY();

    // North
    if ((nToX == nKX) && (nToY + 1 == nKY))
        return true;

    // North East
    if ((nToX + 1 == nKX) && (nToY + 1 == nKY))
        return true;

    // East
    if ((nToX + 1 == nKX) && (nToY == nKY))
        return true;

    // South East
    if ((nToX + 1 == nKX) && (nToY - 1 == nKY))
        return true;

    // South
    if ((nToX == nKX) && (nToY - 1 == nKY))
        return true;

    // South West
    if ((nToX - 1 == nKX) && (nToY - 1 == nKY))
        return true;

    // West
    if ((nToX - 1 == nKX) && (nToY == nKY))
        return true;

    // North West
    if ((nToX - 1 == nKX) && (nToY + 1 == nKY))
        return true;

    return false;
}

/**
  * Evaluate whether the King is in check.
  *
  * Evaluate whether any opposing piece has the King in check.
  *
  * \param nPlayer The player to search for
  *
  * \return True if King is in check, false otherwise.
  */

bool ChessGame::KingInCheck(int nPlayer) const
{
    int nKX {0};
    int nKY {0};

    if (!FindPiece(nKX, nKY, nPlayer, m_kcKingToken))
    {
        std::string sErrorMessage  = "Could not find King for Player " + std::to_string(nPlayer);
        std::cerr << sErrorMessage << std::endl;
        std::cout << "Exiting" << std::endl;
        throw GameAIException("Could not find King ");
    }

    for (int yyy = 0; yyy < m_knY; ++yyy)
    {
        for (int xxx = 0; xxx < m_knX; ++xxx)
        {
            if (cBoard.PositionOccupiedByPlayer(xxx, yyy, 3 - nPlayer))
            {
                if (AttackingTheKing(nKX, nKY, nPlayer, xxx, yyy))
                    return true;
            }
        }
    }

    return false;
}

/**
  * Evaluate whether an opposing piece is attacking the King.
  *
  * \param nKX the X-Coordinate of the King
  * \param nKY The Y-Coordinate of the King
  * \param nPlayer The player to search for
  * \param nX The X-Coordinate of the opposing piece
  * \param nX The Y-Coordinate of the opposing piece
  *
  * \return True if Kthe opposing piece is attacking the King, false otherwise.
  */

bool ChessGame::AttackingTheKing(int nKX, int nKY, int nPlayer, int nX, int nY) const
{
    GameMove cGameMove;
    cGameMove.SetFromX(nX);
    cGameMove.SetFromY(nY);
    cGameMove.SetTestMove(true);

    std::vector<GameMove> vGameMoves = GenerateMovesForPiece(3 - nPlayer, cGameMove);
    for (GameMove cGameMove: vGameMoves)
    {
        if ((cGameMove.ToX() == nKX) && (cGameMove.ToY() == nKY))
        {
            return true;
        }
    }

    return false;
}

/**
  * Announce game move.
  *
  * Construct and return a string containing the move.
  *
  * \param nPlayer   The player whose turn it is.
  * \param cGameMove The player's move
  *
  * \return A string containing the move.
  */

std::string ChessGame::AnnounceMove(int nPlayer, const GameMove &cGameMove) const
{
    std::string sMessage = "\rMove number: " + std::to_string(m_nNumberOfMoves + 1) +
        "  Player: " + std::to_string(nPlayer) +
        "  Moves From: " + cGameMove.AnnounceFromMove() +" To: " + cGameMove.AnnounceToMove();

    return sMessage;
}

/**
  * Evaluate the game state.
  *
  * From a player's perspective, return a value cooresponding to the player's
  * standing in the game.  If the player has won the game, return a large,
  * positive integer.  If winning, a smaller, positive integer. If lost or
  * losing, a negative integer.
  *
  * \param nPlayer   The player whose turn it is.
  *
  * \return An integer representing game state for the player.
  */

int ChessGame::EvaluateGameState(int nPlayer)
{
    // If won, return largest positive integer // TODO: make these constants
    if (m_nWinner == nPlayer)
        return INT_MAX;

    // If lost, return largest negative integer // TODO: make these constants
    if (m_nWinner == (1 - nPlayer + 2))
        return INT_MIN;

    // Evaluate the number of peices for each player. "Greedy evaluation."
    int nCountEval = CountEvaluation(nPlayer)  - CountEvaluation(3 - nPlayer);

    int nDoubledPawnsEval  {0};
    int nIsolatedPawnsEval {0};
    int nPassedPawnsEval   {0};
    CountPawns(nPlayer, nDoubledPawnsEval, nIsolatedPawnsEval, nPassedPawnsEval);

    // Evaluate the number of moves available.
    int nMobilityEval = MobilityEvaluation(nPlayer) - MobilityEvaluation(3 - nPlayer);

    int nKingSafety {0};
    if (KingCastled(nPlayer))
        nKingSafety = 10000;

    int nMinorPiecesDeveloped = MinorPiecesDevelopedEvaluation(nPlayer);

    int nRooksConnected {0};
    if (RooksConnected(nPlayer))
        nRooksConnected = 300;

    return (nCountEval * 30)
        - (nDoubledPawnsEval * 5)
        - (nIsolatedPawnsEval * 10)
        + (nPassedPawnsEval * 20)
        + (nMobilityEval * 20)
        + nKingSafety
        + (nMinorPiecesDeveloped * 200)
        + nRooksConnected;
}

/**
  * Count the number of pieces, weighted by value, for a player.
  *
  * \param nPlayer The player whose turn it is.
  *
  * \return An integer representing the value of pieces for this player.
  */

int ChessGame::CountEvaluation(int nPlayer) const
{
    int nEval = 0;

    for (int yyy = 0; yyy < m_knY; ++yyy)
    {
        for (int xxx = 0; xxx < m_knX; ++xxx)
        {
            if (cBoard.PositionOccupiedByPlayer(xxx, yyy, nPlayer))
            {
                nEval += cBoard.PieceValue(xxx, yyy);
            }
        }
    }

    return nEval;
}

/**
  * Evaluate the mobility (number of moves) for this player.
  *
  * \param nPlayer The player whose turn it is.
  *
  * \return An integer representing the number of valid moves for the player.
  */

int ChessGame::MobilityEvaluation(int nPlayer)
{
    std::vector<GameMove> vGameMoves = GenerateMoves(nPlayer);

    return vGameMoves.size();
}

/**
  * Count the number of doubled, isolated, and passed pawns for this player.
  *
  * \param nPlayer The player whose turn it is.
  * \param nDoubled The count of doubled pawns
  * \param nIsolated The count of isolated pawns
  * \param nPassed the count of passed pawns
  */

void ChessGame::CountPawns(int nPlayer, int &nDoubled, int &nIsolated, int &nPassed) const
{
    int nX {0};
    int nY {0};
    bool bIsolated {false};
    bool bPassed   {false};

    for (int yyy = 0; yyy < m_knY; ++yyy)
    {
        for (int xxx = 0; xxx < m_knX; ++xxx)
        {
            // If piece is Pawn
            if (cBoard.Token(xxx, yyy) == m_kcPawnToken)
            {
                // If occupied by nPlayer?
                if (cBoard.PositionOccupiedByPlayer(xxx, yyy, nPlayer))
                {
                    //
                    // Count Doubled Pawns
                    //

                    // Calculate correct square to evaluate next
                    if (nPlayer == 1)
                        nY = yyy + 1;
                    else
                        nY = yyy - 1;

                    // If the piece is a Pawn. increment count
                    if (cBoard.Token(xxx, nY) == m_kcPawnToken)
                    {
                        // If the square ahead of the Pawn is occupied by the player
                        if (cBoard.PositionOccupiedByPlayer(xxx, nY, nPlayer))
                        {
                            ++nDoubled;
                        }
                    }

                    //
                    // Count Isolated Pawns
                    //

                    bIsolated = true;

                    // Find a friendly pawn to the west
                    if (cBoard.ValidLocation(xxx - 1, 0))
                    {
                        nX = xxx - 1;
                        nY = 0;
                        if (FindPiece(nX, nY, nPlayer, m_kcPawnToken))
                        {
                            if (nX == xxx - 1)
                            {
                                bIsolated = false;
                            }
                        }
                    }

                    // If we did not find one to the west, find a friendly pawn to the east
                    if ((bIsolated) && (cBoard.ValidLocation(xxx + 1, 0)))
                    {
                        nX = xxx + 1;
                        nY = 0;
                        if (FindPiece(nX, nY, nPlayer, m_kcPawnToken))
                        {
                            if (nX == xxx + 1)
                            {
                                bIsolated = false;
                            }
                        }
                    }

                    if (bIsolated)
                    {
                        ++nIsolated;
                    }

                    //
                    // Count Passed Pawns
                    //

                    bPassed = true;

                    // Find an unfriendly pawn to the west and ahead of this pawn
                    if (cBoard.ValidLocation(xxx - 1, 0))
                    {
                        nX = xxx - 1;
                        nY = 0;
                        if (FindPiece(nX, nY, 3 - nPlayer, m_kcPawnToken))
                        {
                            if (nX == xxx - 1)
                            {
                                if (nPlayer == 1)
                                {
                                    if (nY > yyy)
                                    {
                                        bPassed = false;
                                    }
                                }
                                else
                                {
                                    if (nY < yyy)
                                    {
                                        bPassed = false;
                                    }
                                }
                            }
                        }
                    }

                    // If we did not find one to the west, find a unfriendly pawn to the east
                    if ((bPassed) && (cBoard.ValidLocation(xxx + 1, 0)))
                    {
                        nX = xxx + 1;
                        nY = 0;
                        if (FindPiece(nX, nY, nPlayer, m_kcPawnToken))
                        {
                            if (nX == xxx - 1)
                            {
                                if (nPlayer == 1)
                                {
                                    if (nY > yyy)
                                    {
                                        bPassed = false;
                                    }
                                }
                                else
                                {
                                    if (nY < yyy)
                                    {
                                        bPassed = false;
                                    }
                                }
                            }
                        }
                    }

                    if (bPassed)
                    {
                        ++nPassed;
                    }

                }
            }
        }
    }

    return;
}

/**
  * Evaluate whether minor pieces have been developed for this player.
  *
  * \param nPlayer The player whose turn it is.
  *
  * \return An integer representing the number of minor pieces developed for
  * the player.
  */

int ChessGame::MinorPiecesDevelopedEvaluation(int nPlayer) const
{
    int nEval {0};

    for (int yyy = 0; yyy < m_knY; ++yyy)
    {
        for (int xxx = 0; xxx < m_knX; ++xxx)
        {
            if (cBoard.PositionOccupiedByPlayer(xxx, yyy, nPlayer))
            {
                if (cBoard.PieceValue(xxx, yyy) == 3)
                {
                    if (cBoard.Piece(xxx, yyy).HasMoved())
                    {
                        ++nEval;
                    }
                }
            }
        }
    }

    return nEval;
}

/**
  * Evaluate whether the Rooks are connected for this player.
  *
  * \param nPlayer The player whose turn it is.
  *
  * \return True if Rooks are connected, false otherwise.
  */

bool ChessGame::RooksConnected(int nPlayer) const
{
    bool bRooksConnected {false};
    int nFirstX  {0};
    int nFirstY  {0};
    int nSecondX {0};
    int nSecondY {0};

    // Find first Rook
    if (FindPiece(nFirstX, nFirstY, nPlayer, m_kcRookToken))
    {
        nSecondX = nFirstX;
        nSecondY = nFirstY;

        // Update coordinates to find another Rook
        if (nFirstX < m_knX - 1)
        {
            ++nSecondX;
        }
        else
        {
            nSecondX = 0;
            ++nSecondY;
        }

        // Find second Rook
        if (FindPiece(nSecondX, nSecondY, nPlayer, m_kcRookToken))
        {
            bRooksConnected = true;

            //
            // Ensure it is clear between Rooks
            //

            // If same file
            if (nFirstX == nSecondX)
            {
                for (int yyy = nFirstY + 1; yyy != nSecondY; ++yyy)
                {
                    if (cBoard.PositionOccupied(yyy, nFirstX))
                    {
                        bRooksConnected = false;
                        break;
                    }
                }
            }
            // else if same rank
            else if (nFirstY == nSecondY)
            {
                for (int xxx = nFirstX + 1; xxx != nSecondX; ++xxx)
                {
                    if (cBoard.PositionOccupied(nFirstY, xxx))
                    {
                        bRooksConnected = false;
                        break;
                    }
                }
            }
            else
            {
                bRooksConnected = false;
            }
        }
    }

    return bRooksConnected;
}

/**
  * Return a string providing a game score.
  *
  * This method returns the Zobrist hash if the logging level is set to 3.
  *
  * \return String representing the Zobrist hash.
  */

std::string ChessGame::GameScore() const
{
    std::string sMessage {};

    if (m_cLogger.Level() >= 3)
    {
        sMessage += "ZKey=" + std::to_string(cBoard.ZKey());
    }

    return sMessage;
}

/**
  * Check to see if a player has won the game.
  *
  * For this player, check various conditions of an ended game.
  *
  * \param nPlayer The player
  *
  * \return True, if any player has won the game.  False otherwise.
  */

bool ChessGame::GameEnded(int nPlayer)
{
    // Clear win variables
    m_nWinner = 0;
    m_sWinBy.assign("nothing");

    if (BoardGame::GameEnded(nPlayer))
        return true;

    //
    // Check for threefold repetition
    //

    // If the last Zobrist hash has been seen three times, this is a threefold repetition of a move and therefore a draw
    if (m_uomsZobrist.count(cBoard.ZKey()) >= m_knMaxRepetition)
    {
        m_sWinBy.assign("drawn by threefold repetition");
        m_bGameOver = true;
        return true;
    }

    // Evaluate whether the player has any valid moves to make
    std::vector<GameMove> vGameMoves = GenerateMoves(nPlayer);
    if (vGameMoves.empty())
    {
        // If King is in checkmate
        if (KingInCheck(nPlayer))
        {
            m_nWinner = 3 - nPlayer;
            m_sWinBy.assign("checkmate");
        }
        else // If King is not in checkmate
        {
            m_sWinBy.assign("stalemate");
        }

        m_bGameOver = true;
        return true;
    }

    return false;
}


/**
  * Read game moves from a file.
  *
  * Read the file until game moves are available and pass control to ReadAndApplyMoves().
  * Keep track of and return the player number whose turn is next.
  *
  * \param sFileName Name of the file to read from.
  *
  * \return The number of the player whose turn is next.
  */

int ChessGame::ReadMovesFromFile(const std::string &sFileName)
{
    std::fstream fsFile;
    int nPlayer;

    if (!OpenFileForRead(sFileName, fsFile))
    {
        std::cerr << "Could not open " << sFileName << std::endl;
        return 0;
    }

    // Read Psuedo PGN
    // Then read and apply moves
    std::string sLine;
    while (true)
    {
        char cPeek = fsFile.peek();
        // If first character is [, this is not a move
        if (cPeek == '[')
        {
            getline(fsFile, sLine);
        }
        else
        {
            break;
        }
    }

    // Read the moves from the file and apply them to the game
    nPlayer = ReadAndApplyMoves(sFileName, fsFile);

    CloseFile(fsFile);

    return nPlayer;
}

/**
  * Write game moves to a file.
  *
  * Write pseudo PGN tags and game moves to a file.
  *
  * \param sFileName Name of the file to read from.
  *
  * \return True if successful.  False otherwise.
  */

bool ChessGame::WriteMovesToFile(const std::string &sFileName)
{
    std::fstream fsFile;

    if (!OpenFileForWrite(sFileName, fsFile))
    {
        std::cerr << "Could not open " << sFileName << std::endl;
        return false;
    }

    // Psuedo PGN
    // Write the following REQUIRED:

    // Event: the name of the tournament or match event.
    fsFile << "[Event \"" << m_sProgramName << " " << Title() << "\"]" << std::endl;
        // TODO:
        //GameAIVersion::SemanticVersion() << " " <<
        //GameAIVersion::DateVersion() <<
        //"\"]" << std::endl;

    // Site: the location of the event. This is in "City, Region COUNTRY" format, where COUNTRY is the three-letter International Olympic Committee code for the country. An example is "New York City, NY USA".
    fsFile << "[Site \"??, ??, ??\"]" << std::endl;

    // Date: the starting date of the game, in YYYY.MM.DD form. "??" are used for unknown values.
    fsFile << "[Date \"" << m_sDateStart << "\"]" << std::endl;

    // Round: the playing round ordinal of the game within the event.
    fsFile << "[Round \"1\"]" << std::endl;

    // White: the player of the white pieces, in "last name, first name" format.
    fsFile << "[White \"" << m_sPlayer1Name << "\"]" << std::endl;

    // Black: the player of the black pieces, same format as White.
    fsFile << "[Black \"" << m_sPlayer2Name << "\"]" << std::endl;

    // Result: the result of the game. This can only have four possible values: "1-0" (White won), "0-1" (Black won), "1/2-1/2" (Draw), or "*" (other, e.g., the game is ongoing).
    fsFile << "[Result \"";
    if (m_nWinner == 1)
        fsFile << "1-0";
    else if (m_nWinner == 2)
        fsFile << "0-1";
    else
        fsFile << "1/2-1/2";
    fsFile << "\"]" << std::endl;

    // Write the following OPTIONAL:
    // Time: Time the game started, in "HH:MM:SS" format, in local clock time.
    fsFile << "[Time \"" << m_sTimeStart << "\"]" << std::endl;

    // WhiteType, BlackType: These tags use string values; these describe the player types.  The value "human" should be used for a person while the value "program" should be used for algorithmic (computer) players.
    fsFile << "[WhiteType \"" << m_sPlayer1Type << "\"]" << std::endl;
    fsFile << "[BlackType \"" << m_sPlayer2Type << "\"]" << std::endl;

    // Blank line
    fsFile << std::endl;

    if (!WriteMoves(sFileName, fsFile))
        return false;

    CloseFile(fsFile);

    return true;
}
