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

#include "PlayingCards.h"

//// Initial static int to zero; used to generate unique identifier numbers
//int PlayingCards::m_nCount {0};

int PlayingCards::HasCards() const
{
    return m_vCards.size();
}

bool PlayingCards::HasRank(const std::string &sRank) const
{
    for (const Card &cCard : m_vCards)
    {
        if (sRank == cCard.Rank())
        {
            return true;
        }
    }

    return false;
}

bool PlayingCards::HasSuit(const std::string &sSuit) const
{
    for (const Card &cCard : m_vCards)
    {
        if (sSuit == cCard.Suit())
        {
            return true;
        }
    }

    return false;
}

int PlayingCards::HasCardsOfRank(const std::string &sRank) const
{
    int nCount {0};

    for (const Card &cCard : m_vCards)
    {
        if (sRank == cCard.Rank())
        {
            ++nCount;
        }
    }

    return nCount;
}

int PlayingCards::HasCardsOfSuit(const std::string &sSuit) const
{
    int nCount {0};

    for (const Card &cCard : m_vCards)
    {
        if (sSuit == cCard.Suit())
        {
            ++nCount;
        }
    }

    return nCount;
}

void PlayingCards::AddCard(Card &cCard)
{
    m_vCards.push_back(cCard);
}

void PlayingCards::AddCardToTop(Card &cCard)
{
    std::vector<Card>::iterator it = m_vCards.begin();
    m_vCards.insert(it, cCard);
}

void PlayingCards::AddCards(std::vector<Card> &vCards)
{
    for (Card &cCard : vCards)
    {
        m_vCards.push_back(cCard);
    }
}

Card PlayingCards::DrawTopCard()
{
    Card cCard = m_vCards.front();

    m_vCards.erase(m_vCards.begin());

    return cCard;
}

Card PlayingCards::PeekAtBottomCard() const
{
    Card cCard = m_vCards.back();

    return cCard;
}

Card PlayingCards::RemoveCard(std::string sRank, std::string sSuit)
{
    Card cCard;

    for (std::vector<Card>::iterator it = m_vCards.begin(); it != m_vCards.end(); ++it)
    {
        if ((it->Rank() == sRank) && (it->Suit() == sSuit))
        {
            cCard = *it;
            m_vCards.erase(it);
        }
    }

    return cCard;
}

std::vector<Card> PlayingCards::RemoveCardsOfRank(std::string sRank)
{
    std::vector<Card> vCards {};

    for (std::vector<Card>::iterator it = m_vCards.begin(); it != m_vCards.end(); ++it)
    {
        if (it->Rank() == sRank)
        {
            vCards.push_back(*it);
            m_vCards.erase(it);
        }
    }

    return vCards;
}

std::vector<Card> PlayingCards::RemoveCardsOfSuit(std::string sSuit)
{
    std::vector<Card> vCards {};

    for (std::vector<Card>::iterator it = m_vCards.begin(); it != m_vCards.end(); ++it)
    {
        if (it->Suit() == sSuit)
        {
            vCards.push_back(*it);
            m_vCards.erase(it);
        }
    }

    return vCards;
}



