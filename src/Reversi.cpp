#include "Reversi.h"

void Reversi::SetBoard()
{
    m_acGrid[(m_knY / 2) - 1][(m_knX / 2) - 1] = m_acTokens[2];
    m_acGrid[(m_knY / 2) - 1][(m_knX / 2)]     = m_acTokens[1];
    m_acGrid[(m_knY / 2)][(m_knX / 2) - 1]     = m_acTokens[1];
    m_acGrid[(m_knY / 2)][(m_knX / 2)]         = m_acTokens[2];
}

std::vector<GameMove> Reversi::GenerateMoves(int nPlayer, int nOpponent) const
{
    std::vector<GameMove> vGameMoves {};

    for (int xxx = 0; xxx < m_knX; ++xxx)
    {
        for (int yyy = 0; yyy < m_knY; ++yyy)
        {
            if (m_acGrid[yyy][xxx] == m_kcClear)
            {
                if (Adjacent(xxx, yyy, nPlayer, int nOpponent)) // Check valid move
                {
                    vGameMoves.emplace_back(0,0,xxx,yyy);
                }
            }
        }
    }

    return vGameMoves;
}

bool Reversi::Adjacent(int nX, int nY, int nPlayer, int nOpponent) const
{
    if (AdjacentUp(nX, nY, nPlayer, nOpponent))
        return true;

    return false;
}

bool Reversi::AdjacentUp(int nX, int nY, int nPlayer, int nOpponent) const
{
    static bool bOpponentPeiceAdjacent = false;

    ++nY;

    if (!ValidMove(nY, nX))
        return false;

    if (m_acGrid[nY][nX] == m_kcClear)
        return false;

    if (m_acGrid[nY][nX] == m_acTokens[nOpponent])
    {
        bOpponetPeiceAdjacent = true;
        AdjacentUp(nX, nY);
    }

    if (m_acGrid[nY][nX] == m_acTokens[nPlayerNumber])
    {
        if (bOpponentPeiceAdjacent)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    return false;  // Should not get here
}

GameMove Reversi::GetMove(int nPlayer, int nOpponent) const
{
    std::string sMove {};
    GameMove cGameMove;

    std::cin >> sMove;

    std::string sToken = sMove.substr(0,sMove.find(m_kDelimeter));
    int xxx = atoi(sToken.c_str());

    sToken = sMove.substr(sMove.find(m_kDelimeter) + 1,sMove.length() );
    int yyy = atoi(sToken.c_str());

    cGameMove.SetToX(xxx - 1);
    cGameMove.SetToY(yyy - 1);

    return cGameMove;
}


int Reversi::ApplyMove(int nPlayer, GameMove &cGameMove)
{
    if ((nPlayer != m_kPlayer1) && (nPlayer != m_kPlayer2))
        return -1;

    if ((cGameMove.ToX() > m_knX - 1) || (cGameMove.ToX() < 0))
        return -1;

    if ((cGameMove.ToY() > m_knY - 1) || (cGameMove.ToY() < 0))
        return -1;

    if (m_acGrid[cGameMove.ToY()][cGameMove.ToX()] != m_kcClear)
        return -1;

    m_acGrid[cGameMove.ToY()][cGameMove.ToX()] = m_acTokens[nPlayer];
    ++m_nNumberOfMoves;
    //UpdatePlayerTurn();

    return 1;
}

bool Reversi::GameEnded(int nPlayer, int nOpponent)
{
    m_nWinner = 0;
    m_sWinBy.assign("nothing");

    if (CheckOrthogonal(m_kPlayer1, m_kWin))
    {
        m_nWinner = m_kPlayer1;
        return true;
    }

    if (CheckOrthogonal(m_kPlayer2, m_kWin))
    {
        m_nWinner = m_kPlayer2;
        return true;
    }

    if (CheckDiagonal(m_kPlayer1, m_kWin))
    {
        m_nWinner = m_kPlayer1;
        return true;
    }

    if (CheckDiagonal(m_kPlayer2, m_kWin))
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

int Reversi::PreferredMove(const GameMove &cGameMove) const
{
    //return std::abs(cGameMove.ToX() - 3);
    return 0;
}
