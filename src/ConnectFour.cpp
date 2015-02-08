/*
    Copyright 2014 Dom Maddalone

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

#include "ConnectFour.h"

void ConnectFour::Display(const bool bDisplayCoordinates) const
{
    if (bDisplayCoordinates)
    {
        std::cout << "   ";
        for (int xxx = 0; xxx < m_kX; ++xxx)
        {
            std::cout << xxx + 1 << "   ";
        }
        std::cout << std::endl << std::endl;;
    }

    for (int yyy = 0; yyy < m_kY; ++yyy)
    {
        if (bDisplayCoordinates)
        {
            std::cout << yyy + 1 << "  ";
        }

        for (int xxx = 0; xxx < m_kX; ++xxx)
        {
            std::cout << m_anGrid[yyy][xxx] << "   ";
        }

        std::cout << std::endl << std::endl;
    }
}

void ConnectFour::DisplayValidMoves() const
{
    std::vector<GameMove> vGameMoves = GenerateMoves();
    for (GameMove cGameMove : vGameMoves)
    {
        std::cout << cGameMove.ToX() + 1 << " ";
    }
    std::cout << std::endl;
}

std::vector<GameMove> ConnectFour::GenerateMoves() const
{
    std::vector<GameMove> vGameMoves {};

    for (int xxx = 0; xxx < m_kX; ++xxx)
    {
        if (FindBottom(xxx) >= 0)
        {
            vGameMoves.emplace_back(0,0,xxx,0);
        }
    }

    return vGameMoves;
}


GameMove ConnectFour::GetMove() const
{
    int nMove {};
    GameMove cGameMove;

    std::cin >> nMove;
    cGameMove.SetToX(nMove - 1);

    return cGameMove;
}

void ConnectFour::AnnounceMove(const int nPlayer, const GameMove &cGameMove)
{
    std::cout << "Move number " << m_nNumberOfMoves + 1 << " -- Player " << nPlayer << " moves: " << cGameMove.ToX() + 1 << std::endl;;
}

int ConnectFour::ApplyMove(const int nPlayer, GameMove &cGameMove)
{
    if ((nPlayer != m_kPlayer1) && (nPlayer != m_kPlayer2))
        return -1;

    if ((cGameMove.ToX() > m_kX - 1) || (cGameMove.ToX() < 0))
        return -1;

    if (m_anGrid[0][cGameMove.ToX()] != 0)
        return -1;

    int y = FindBottom(cGameMove.ToX());

    if (y == -1)
        return -1;

    cGameMove.SetToY(y);

    m_anGrid[cGameMove.ToY()][cGameMove.ToX()] = nPlayer;
    ++m_nNumberOfMoves;

    return y;
}

bool ConnectFour::RetractMove(const int nPlayer, const GameMove &cGameMove)
{
    m_anGrid[cGameMove.ToY()][cGameMove.ToX()] = 0;
    --m_nNumberOfMoves;

    return true;
}

void ConnectFour::CountSequence(int nSequence, SequenceCounts &stSequenceCounts)
{
    switch (nSequence)
    {
        case 0:
            break;
        case 1:
            ++stSequenceCounts.nCount1;
            break;
        case 2:
            ++stSequenceCounts.nCount2;
            break;
        case 3:
            ++stSequenceCounts.nCount3;
            break;
        default:
            break; // TODO - should not get here
    }
    return;
}

int ConnectFour::EvaluateGameState(const int nPlayer)
{
    if (m_nWinner == nPlayer)
        return 1000000;

    if (m_nWinner == (1 - nPlayer + 2))
        return -1000000;

    m_stMyCounts = {};
    m_stOpponentCounts = {};

    if (m_nWinner == 0)
    {
        for (int yyy = 0; yyy < m_kY; ++yyy)
        {
            for (int xxx = 0; xxx < m_kX; ++xxx)
            {
                CountSequence(CheckHorizontal(nPlayer, yyy, xxx), m_stMyCounts);
                CountSequence(CheckHorizontal(1 - nPlayer + 2, yyy, xxx), m_stOpponentCounts);
                CountSequence(CheckVertical(nPlayer, yyy, xxx), m_stMyCounts);
                CountSequence(CheckVertical(1 - nPlayer + 2, yyy, xxx), m_stOpponentCounts);
                CountSequence(CheckDiagonalUpperLeftLowerRight(nPlayer, yyy, xxx), m_stMyCounts);
                CountSequence(CheckDiagonalUpperLeftLowerRight(1 - nPlayer + 2, yyy, xxx), m_stOpponentCounts);
                CountSequence(CheckDiagonalUpperRightLowerLeft(nPlayer, yyy, xxx), m_stMyCounts);
                CountSequence(CheckDiagonalUpperRightLowerLeft(1 - nPlayer + 2, yyy, xxx), m_stOpponentCounts);
            }
        }
/*
        return ( ((m_stMyCounts.nCount2 - m_stOpponentCounts.nCount2) * 10) +
                 ((m_stMyCounts.nCount3 - m_stOpponentCounts.nCount3) * 100) );
 */

        return ( (m_stMyCounts.nCount2 * 10) - (m_stOpponentCounts.nCount2 * 100) +
                 (m_stMyCounts.nCount3 * 1000) - (m_stOpponentCounts.nCount3 * 10000) );
    }

    return 0;
}

int ConnectFour::FindBottom(const int x) const
{
    for (int yyy = m_kY - 1; yyy >= 0; --yyy)
    {
        if (m_anGrid[yyy][x] == 0)
            return yyy;
    }

    return -1;
}

bool ConnectFour::GameEnded()
{
    m_nWinner = 0;
    m_sWinBy.assign("nothing");

    if (CheckOrthogonal(m_kPlayer1, 4))
    {
        m_nWinner = m_kPlayer1;
        return true;
    }

    if (CheckOrthogonal(m_kPlayer2, 4))
    {
        m_nWinner = m_kPlayer2;
        return true;
    }

    if (CheckDiagonal(m_kPlayer1, 4))
    {
        m_nWinner = m_kPlayer1;
        return true;
    }

    if (CheckDiagonal(m_kPlayer2, 4))
    {
        m_nWinner = m_kPlayer2;
        return true;
    }

    std::vector<GameMove> vGameMoves = GenerateMoves();
    if (vGameMoves.empty())
    {
        return true;
    }

    return false;
}

bool ConnectFour::CheckOrthogonal(const int nPlayer, int nConnect) //const
{
    bool bConnect = false;

    for (int yyy = 0; yyy < m_kY; ++yyy)
    {
        for (int xxx = 0; xxx < m_kX; ++xxx)
        {
            if (CheckHorizontal(nPlayer, yyy, xxx) == nConnect)
            {
                bConnect = true;
                if (nConnect == 4)
                    m_sWinBy.assign("Horizontal");
                break;
            }
        }

        if (bConnect)
            break;
    }

    if (!bConnect)
    {
        for (int yyy = 0; yyy < m_kY; ++yyy)
        {
            for (int xxx = 0; xxx < m_kX; ++xxx)
            {
                if (CheckVertical(nPlayer, yyy, xxx) == nConnect)
                {
                    bConnect = true;
                    if (nConnect == 4)
                        m_sWinBy.assign("Horizontal");
                    break;
                }
            }

            if (bConnect)
                break;
        }
    }

    return bConnect;
}

int ConnectFour::CheckHorizontal(const int nPlayer, const int y, const int x) //const
{
    if (!ValidMove(y, x)) return 0;

    if (m_anGrid[y][x] == nPlayer)
        return (1 + CheckHorizontal(nPlayer, y, x+1));
    else
        return 0;
}

int ConnectFour::CheckVertical(const int nPlayer, const int y, const int x) //const
{
    if (!ValidMove(y, x)) return 0;

    if (m_anGrid[y][x] == nPlayer)
        return (1 + CheckVertical(nPlayer, y+1, x));
    else
        return 0;
}

bool ConnectFour::CheckDiagonal(const int nPlayer, int nConnect) //const
{
    bool bConnect = false;

    for (int yyy = 0; yyy < m_kY; ++yyy)
    {
        for (int xxx = 0; xxx < m_kX; ++xxx)
        {
            if (CheckDiagonalUpperLeftLowerRight(nPlayer, yyy, xxx) == nConnect)
            {
                bConnect = true;
                if (nConnect == 4)
                    m_sWinBy.assign("DiagonalULLR");
                break;
            }
        }

        if (bConnect)
            break;
    }

    if (!bConnect)
    {
        for (int yyy = 0; yyy < m_kY; ++yyy)
        {
            for (int xxx = m_kX - 1; xxx >= 0; --xxx)
            {
                if (CheckDiagonalUpperRightLowerLeft(nPlayer, yyy, xxx) == nConnect)
                {
                    bConnect = true;
                    if (nConnect == 4)
                        m_sWinBy.assign("DiagonalURLL");
                    break;
                }
            }

            if (bConnect)
                break;
        }
    }

    return bConnect;
}

int ConnectFour::CheckDiagonalUpperLeftLowerRight(const int nPlayer, const int y, const int x) //const
{
    if (!ValidMove(y, x)) return 0;

    if (m_anGrid[y][x] == nPlayer)
        return (1 + CheckDiagonalUpperLeftLowerRight(nPlayer, y+1, x+1));
    else
        return 0;
}

int ConnectFour::CheckDiagonalUpperRightLowerLeft(const int nPlayer, const int y, const int x) //const
{
    if (!ValidMove(y, x)) return 0;

    if (m_anGrid[y][x] == nPlayer)
        return (1 + CheckDiagonalUpperRightLowerLeft(nPlayer, y+1, x-1));
    else
        return 0;
}

bool ConnectFour::ValidMove(const int y, const int x) const
{
    if ((y >= m_kY) || (y < 0))
        return false;

    if ((x >= m_kX) || (x < 0))
        return false;

    return true;
}

int ConnectFour::PreferredMove(const GameMove &cGameMove) const
{
    return std::abs(cGameMove.ToX() - 3);
}
