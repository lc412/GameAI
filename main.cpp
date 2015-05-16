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

#include <getopt.h>
#include <iostream>
#include <vector>

#include "GameAIException.h"
#include "GameAIVersion.h"
#include "Player.h"
#include "Game.h"

/**
  * ShowUsage
  *
  * Print command line arguments
  *
  * \param sName Name of the executing program.
  */

static void ShowUsage(std::string sName)
{
    std::cerr << "Usage: " << sName << " OPTIONS\n"
              << "Run a Game AI program\n\n"
              << "Required Options:\n"
              << "    -1 TYPE,  --player1=TYPE  assign TYPE of player to Player 1\n"
              << "    -2 TYPE,  --player2=TYPE  assign TYPE of player to Player 2\n"

              << "    -g GAME,  --game=GAME     play GAME"
              << "\n\nNon Required Options:\n"
              << "              --port=PORT     port for network communications.  Required for TYPE of server and client.\n"
              << "    -h,       --host=HOSTNAME host address or name of the server.  Required for TYPE server.\n"
              << "    -H,       --help          display this help message and exit\n"
              << "    -p PLIES, --plies=PLIES   assign the number of PLIES that minimax players will use\n"
              << "              --plies1=PLIES  assign the number of PLIES to Player 1, if minimax\n"
              << "              --plies2=PLIES  assign the number of PLIES to Player 2, if minimax\n"
              << "    -v LEVEL, --verbose=LEVEL display game information\n"
              << "    -V,       --version       display version and exit\n"
              << "\n"
              << "PORT is a port specification for a server and client to communicate over.  The default is 60000.\n"
              << "HOST is a host name or address for a server.  The default is 127.0.0.1.\n"
              << "TYPE is either human, minimax, client, or server.  Start a server before staring a client.\n"
              << "PLIES are from 1 to 9.  The default is 4.\n"
              << "GAME is ttt, connectfour, or reversi.\n"
              << "LEVEL is an integer 0 to 3.  The default is 1.\n"
              << "    0 = display start and ending announcements\n"
              << "    1 = display game move-by-move\n"
              << "    2 = display AI scoring of moves and basic network communications\n"
              << "    3 = display AI evaluation of moves and detailed network communications\n"
              << "\n"
              << "Examples:\n"
              << "GameAI -1 human -2 minimax -g ttt\n\n"
              << "GameAI -1 human  -2 client --port=60001 -g connecfour\n"
              << "GameAI -1 server -2 human  --port=60001 --host=192.168.0.1 -g connecfour\n"
              << std::endl;
}

/**
  * ShowVersion
  *
  * Print version
  *
  */

static void ShowVersion()
{
    std::cerr << "Game AI version " << GameAIVersion::SemanticVersion() << " " << GameAIVersion::DateVersion() << std::endl;
}

/**
  * Create the player.
  *
  * Depending on the player type, generate a player and push them onto the
  * players vector.
  *
  * \param pcType   Char string of player type
  * \param vPlayers Vector of players
  *
  * \return True if player type known.  False otherwise.
  */

static bool SetPlayerType(char* pcType, std::vector<std::unique_ptr<Player>> &vPlayers)
{
    std::string sType(pcType);
    if (sType == "human")
        vPlayers.emplace_back(Player::MakePlayer(PlayerType::TYPE_HUMAN));
    else if (sType == "minimax")
        vPlayers.emplace_back(Player::MakePlayer(PlayerType::TYPE_MINIMAX));
    else if (sType == "server")
        vPlayers.emplace_back(Player::MakePlayer(PlayerType::TYPE_CLIENT));
    else if (sType == "client")
        vPlayers.emplace_back(Player::MakePlayer(PlayerType::TYPE_SERVER));
    else
        return false;

    return true;
}

/**
  * Create the game.
  *
  * Depending on the game type, generate a game and return a pointer to it.
  *
  * \param pcGame   Char string of game type
  *
  * \return unique_ptr to the game, if game type known.  nullptr otherwise.
  */

static std::unique_ptr<Game> SetGame(char* pcGame)
{
    std::string sGame(pcGame);
    if (sGame == "connectfour")
        return Game::Make(GameType::TYPE_CONNECT_FOUR);
    else if (sGame == "ttt")
        return Game::Make(GameType::TYPE_TTT);
    else if (sGame == "reversi")
        return Game::Make(GameType::TYPE_REVERSI);
    else if (sGame == "chess-gardner")
        return Game::Make(GameType::TYPE_CHESSGARDNER);
    else
        return nullptr;
}

/**
  * Set player parameters.
  *
  * Set the name, look-ahead (plies), verbosity of the players.
  *
  * \param sName      Name of the executable
  * \param nPlies1    Plies (moves to look ahead) for player number 1
  * \param nPlies2    Plies for plyer number 2
  * \param nVerbosity Level of logging
  * \param vPlayers   Vector of players
  *
  */

static void SetPlayers(std::string sName, int nPlies1, int nPlies2, int nVerbosity, std::string sGameTitle,
                       std::string sHost, int nPort, std::vector<std::unique_ptr<Player>> &vPlayers)
{
    // Set verbosity
    if (nVerbosity >= 0 && nVerbosity <= 3)
    {
        vPlayers[0]->SetVerbosity(nVerbosity);
        vPlayers[1]->SetVerbosity(nVerbosity);
    }
    else
    {
        ShowUsage(sName);
        exit(EXIT_FAILURE);
    }

    // Set number of plies for minimax players
    if (nPlies1 > 0 && nPlies1 <= 9)
    {
        vPlayers[0]->SetPlies(nPlies1);
    }
    else
    {
        ShowUsage(sName);
        exit(EXIT_FAILURE);
    }

    if (nPlies2 > 0 && nPlies2 <= 9)
    {
        vPlayers[1]->SetPlies(nPlies2);
    }
    else
    {
        ShowUsage(sName);
        exit(EXIT_FAILURE);
    }

    // Set game title
    vPlayers[0]->SetGameTitle(sGameTitle);
    vPlayers[1]->SetGameTitle(sGameTitle);

    // Initialize players
    bool bSwap0 = false;
    bool bSwap1 = false;
    vPlayers[0]->Initialize(sHost, nPort, bSwap0);
    vPlayers[1]->Initialize(sHost, nPort, bSwap1);

    // Swap players to match server players
    if (bSwap0 || bSwap1)
    {
        std::iter_swap(vPlayers.begin() + 0, vPlayers.begin() + 1);
        int nNumber = vPlayers[0]->PlayerNumber();
        vPlayers[0]->SetPlayerNumber(vPlayers[1]->PlayerNumber());
        vPlayers[1]->SetPlayerNumber(nNumber);
    }
}

/**
  * Main
  *
  * Gather options, generate players and the game, and
  * call each player to make their move.  Evaluate game for an end state
  * and declare a winner when game ended.
  *
  */

int main(int argc, char* argv[])
{
    std::vector<std::unique_ptr<Player>> vPlayers;
    std::unique_ptr<Game> pcGame {};
    int  nPlies1      {4};
    int  nPlies2      {4};
    int nPort         {60000};
    std::string sHost {"127.0.0.1"};
    int  nVerbosity   {1};

    // Check for command line arguments
    if (argc < 2)
    {
        ShowUsage(argv[0]);
        exit (EXIT_FAILURE);
    }

    ShowVersion();

    // Set up getopt_long
    static struct option stLongOptions[] =
    {
        {"player1", required_argument, nullptr, '1'},
        {"player2", required_argument, nullptr, '2'},
        {"plies",   required_argument, nullptr, 'p'},
        {"plies1",  required_argument, nullptr, 'x'},
        {"plies2",  required_argument, nullptr, 'y'},
        {"game",    required_argument, nullptr, 'g'},
        {"port",    required_argument, nullptr, 't'},
        {"host",    required_argument, nullptr, 'h'},
        {"verbose", required_argument, nullptr, 'v'},
        {"help",    no_argument,       nullptr, 'H'},
        {"version", no_argument,       nullptr, 'V'},
        {NULL,      0,                 nullptr,  0}
    };

    // Execute getopt_long
    int nC = 0;
    int nOptionIndex = 0;
    while ((nC = getopt_long(argc, argv, "1:2:p:x:y:g:t:h:v:HV", stLongOptions, &nOptionIndex)) != -1)
    {
        switch (nC)
        {
            // Player 1
            case '1':
                if (!SetPlayerType(optarg, vPlayers))
                {
                    std::cerr << "unknown player type " << optarg << std::endl;
                    exit(EXIT_FAILURE);
                }
                break;
            // Player 2
            case '2':
                if (!SetPlayerType(optarg, vPlayers))
                {
                    std::cerr << "unknown player type " << optarg << std::endl;
                    exit(EXIT_FAILURE);
                }
                break;
            // Plies for player 1 and player 2
            case 'p':
                nPlies1 = nPlies2 = atoi(optarg);
                break;
            // Plies for player 1
            case 'x':
                nPlies1 = atoi(optarg);
                break;
            // Plies for player 3
            case 'y':
                nPlies2 = atoi(optarg);
                break;
            // Game
            case 'g':
                pcGame = SetGame(optarg);
                break;
            // Port
            case 't':
                nPort = atoi(optarg);
                break;
            // Host
            case 'h':
                sHost = optarg;
                break;
            // Verbosity
            case 'v':
                if (optarg)
                {
                    if (isdigit(optarg[0]))
                    {
                        nVerbosity = atoi(optarg);
                    }
                    else
                    {
                        ShowUsage(argv[0]);
                        exit (EXIT_FAILURE);
                    }
                }
                break;
            // Help
            case 'H':
                ShowUsage(argv[0]);
                exit(EXIT_SUCCESS);
                break;
            // Version
            case 'V':
                //ShowVersion(); Executed by default
                exit(EXIT_SUCCESS);
                break;
            // Help
            case '?':
                ShowUsage(argv[0]);
                exit(EXIT_SUCCESS);
                break;
            // Unkown option
            default:
                throw GameAIException("main getopt returned character code ", std::to_string(nC));
                break;
        }
    }

    // Ensure two players and a game are generated
    if ((vPlayers.size() != 2) || (pcGame == nullptr))
    {
        ShowUsage(argv[0]);
        exit(EXIT_FAILURE);
    }

    // Set player parameters
    SetPlayers(argv[0], nPlies1, nPlies2, nVerbosity, pcGame->Title(), sHost, nPort, vPlayers);

    // Announce game
    std::cout << "Playing " << pcGame->Title() << std::endl;
    for (int iii = 0; iii < 2; ++iii)
    {
        std::cout << "Player " << iii + 1 << ": " << vPlayers[iii]->TypeName();
        if (vPlayers[iii]->Type() == PlayerType::TYPE_MINIMAX)
        {
            std::cout << " Plies: " << vPlayers[iii]->Plies();
        }

        std::cout << std::endl;
    }

    // Have each player play in turn
    while(true)
    {
        // Player 1 move
        if (!vPlayers[0]->Move(*pcGame))
        {
            std::cerr << "Invalid move.  Exiting." << std::endl;
            exit(EXIT_FAILURE);
        }

        // Announce game score
        std::cout << pcGame->GameScore() << std::endl;

        // Evaluate game state from player 2 perspective.  If game ended, allow player 2 to finish.
        // Then break from loop.
        if (pcGame->GameEnded(pcGame->Player2()))
        {
            vPlayers[1]->Finish(*pcGame);
            break;
        }


        // Player 2 move
        if (!vPlayers[1]->Move(*pcGame))
        {
            std::cerr << "Invalid move.  Exiting." << std::endl;
            exit(EXIT_FAILURE);
        }

        // Announce game score
        std::cout << pcGame->GameScore() << std::endl;

        // Evaluate game state from player 1 perspective.  If game ended, allow player 1 to finish.
        // Then break from loop.
        if (pcGame->GameEnded(pcGame->Player1()))
        {
            vPlayers[0]->Finish(*pcGame);
            break;
        }
    }

    // Display game
    pcGame->Display();

    // Summarize game
    if (pcGame->Winner() == 0)
    {
        std::cout << "Game drawn" << std::endl;
    }
    else
    {
        std::cout << "Game won by Player " << pcGame->Winner() << std::endl;
        std::cout << "Game won with " << pcGame->WinBy() << std::endl;
    }

    std::cout << "Total number of moves: " << pcGame->NumberOfMoves() << std::endl;
}
