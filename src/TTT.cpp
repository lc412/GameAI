#include "TTT.h"

void TTT::Display(const bool bDisplayLines) const
{
    for (int yyy = 0; yyy < m_kY; ++yyy)
    {
        for (int xxx = 0; xxx < m_kX; ++xxx)
        {
            std::cout << m_anGrid[yyy][xxx];
            if (bDisplayLines && xxx < m_kX - 1)
                std::cout << " | ";
            else
                std::cout << "   ";
        }

        std::cout << std::endl;'

        if (bDisplayLines && yyy < m_kY - 1)
            std::cout << "---------------" << std::endl;
        else
            std::cout << std::endl;

        //std::cout << std::endl << std::endl;
    }
}

void TTT::DisplayValidMoves() const
{
    std::vector<int> vMoves = GenerateMoves();
    for (int iii : vMoves)
    {
        std::cout << iii + 1 << " ";
    }
    std::cout << std::endl;
}

int TTT::ApplyMove(const int nPlayer, const int x, const int y)
{
    int nAdjustedX = x - 1;
    int nAdjustedY = y - 1;

    if ((nPlayer != m_kPlayer1) && (nPlayer != m_kPlayer2))
        return -1;

    if ((nAdjustedX > m_kX - 1) || (nAdjustedX < 0))
        return -1;

    if (m_anGrid[nAdjustedY][nAdjustedX] != 0)
        return -1;

    m_anGrid[y][nAdjustedX] = nPlayer;
    ++m_nNumberOfMoves;

    return y;
}

bool TTT::RetractMove(const int y, const int x)
{
    m_anGrid[y][x] = 0;
    --m_nNumberOfMoves;
    return true;
}

std::vector<int> TTT::GenerateMoves() const
{
    std::vector<int> vMoves {};

    for (int xxx = 0; xxx < m_kX; ++xxx)
    {
        if (FindBottom(xxx) >= 0)
            vMoves.push_back(xxx);
    }

    return vMoves;
}

void TTT::CountSequence(int nSequence, SequenceCounts &stSequenceCounts)
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

int TTT::EvaluateGameState(const int nPlayer)
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

        return ( ((m_stMyCounts.nCount2 - m_stOpponentCounts.nCount2) * 10) +
                 ((m_stMyCounts.nCount3 - m_stOpponentCounts.nCount3) * 100) );

/*
        if (CheckOrthogonal(1 - nPlayer + 2, 3))
            return -200;

        if (CheckDiagonal(1 - nPlayer + 2, 3))
            return -200;

        if (CheckOrthogonal(nPlayer, 3))
            return 100;

        if (CheckDiagonal(nPlayer, 3))
            return 100;

        if (CheckOrthogonal(1 - nPlayer + 2, 2))
            return -20;

        if (CheckDiagonal(1 - nPlayer + 2, 2))
            return -20;

        if (CheckOrthogonal(nPlayer, 2))
            return 10;

        if (CheckDiagonal(nPlayer, 2))
            return 10;
*/
    }

    return 0;
}

bool TTT::GameEnded()
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

    std::vector<int> vMoves = GenerateMoves();
    if (vMoves.empty())
    {
    //std::cout << "Game ended - no more moves" << std::endl; // DEBUG
        return true;
    }

    return false;
}

bool TTT::CheckOrthogonal(const int nPlayer, int nConnect) //const
{
    bool bConnect = false;

    for (int yyy = 0; yyy < m_kY; ++yyy)
    {
        for (int xxx = 0; xxx < m_kX; ++xxx)
        {
            if (CheckHorizontal(nPlayer, yyy, xxx) == nConnect)
            {
              //      std::cout << "Game ended - Horizontal Player " << nPlayer << std::endl; // DEBUG
                bConnect = true;
                if (nConnect == 4)
                    m_sWinBy.assign("Horizontal");
                //std::string sHorizontal("Horizontal");
                //m_sWinBy = sHorizontal;
                //m_nWinBy = 1;
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
            //    std::cout << "Game ended - Vertical Player " << nPlayer << std::endl; // DEBUG
                    bConnect = true;
                    if (nConnect == 4)
                        m_sWinBy.assign("Horizontal");
                    //m_nWinBy = 2;
                    break;
                }
            }

            if (bConnect)
                break;
        }
    }

    return bConnect;
}

int TTT::CheckHorizontal(const int nPlayer, const int y, const int x) //const
{
    if (!ValidMove(y, x)) return 0;

    if (m_anGrid[y][x] == nPlayer)
        return (1 + CheckHorizontal(nPlayer, y, x+1));
    else
        return 0;
}

int TTT::CheckVertical(const int nPlayer, const int y, const int x) //const
{
    if (!ValidMove(y, x)) return 0;

    if (m_anGrid[y][x] == nPlayer)
        return (1 + CheckVertical(nPlayer, y+1, x));
    else
        return 0;
}

bool TTT::CheckDiagonal(const int nPlayer, int nConnect) //const
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

int TTT::CheckDiagonalUpperLeftLowerRight(const int nPlayer, const int y, const int x) //const
{
    if (!ValidMove(y, x)) return 0;

    if (m_anGrid[y][x] == nPlayer)
        return (1 + CheckDiagonalUpperLeftLowerRight(nPlayer, y+1, x+1));
    else
        return 0;
}

int TTT::CheckDiagonalUpperRightLowerLeft(const int nPlayer, const int y, const int x) //const
{
    if (!ValidMove(y, x)) return 0;

    if (m_anGrid[y][x] == nPlayer)
        return (1 + CheckDiagonalUpperRightLowerLeft(nPlayer, y+1, x-1));
    else
        return 0;
}

bool TTT::ValidMove(const int y, const int x) const
{
    if ((y >= m_kY) || (y < 0))
        return false;

    if ((x >= m_kX) || (x < 0))
        return false;

    return true;
}

int TTT::PreferredMove(const int nMove) const
{
    return std::abs(nMove - 4);
}
