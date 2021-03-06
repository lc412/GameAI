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
              << "    -h,       --host=HOSTNAME host address or name of the server.  Required for TYPE server.\n"
              << "              --port=PORT     port for network communications.  REQUIRED for TYPE of server and client.\n"
              << "    -i FILE,  --input=FILE    read game moves from FILE\n"
              << "    -o FILE,  --output=FILE   write game moves to FILE\n"
              << "              --name1=NAME    assign a name to player 1\n"
              << "              --name2=NAME    assign a name to player 2\n"
              << "    -p PLIES, --plies=PLIES   assign the number of PLIES that ai players will use\n"
              << "              --plies1=PLIES  assign the number of PLIES to Player 1, if ai\n"
              << "              --plies2=PLIES  assign the number of PLIES to Player 2, if ai\n"
              << "    -v LEVEL, --verbose=LEVEL display game information\n"
              << "    -V,       --version       display version and exit\n"
              << "    -H,       --help          display this help message and exit\n"
              << "\n"
              << "PORT is a port specification for a server and client to communicate over.  The default is 60000.\n"
              << "HOST is a host name or address for a server.  The default is 127.0.0.1.\n"
              << "TYPE is either human, ai, client, or server.  Start a server before staring a client.\n"
              << "PLIES are from 1 to 9.  The default is 4.\n"
              << "GAME is one of the following:\n"
              << "    connectfour chess         chess-jm         war\n"
              << "    reversi     chess-attack  chess-los-alamos go-fish\n"
              << "    ttt         chess-baby    chess-petty      rummy\n"
              << "                chess-elena   chess-quick      \n"
              << "                chess-gardner chess-speed      \n"
              << "LEVEL is an integer 0 to 3.  The default is 1.\n"
              << "    0 = display start and ending announcements\n"
              << "    1 = display game move-by-move\n"
              << "    2 = display AI scoring of moves and basic operations\n"
              << "    3 = display AI evaluation of moves and detailed operations\n"
              << "\n"
              << "Examples:\n"
              << "Start a game on a single computer:\n"
              << "GameAI -1 human -2 ai -g ttt\n\n"
              << "Start a game across two computers:\n"
              << "GameAI -1 human  -2 client -g connetcfour --port=60001\n"
              << "GameAI -1 server -2 human  -g connectfour --port=60001 --host=192.168.0.1\n"
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

static bool GeneratePlayer(std::string sPlayer, std::vector<std::unique_ptr<Player>> &vPlayers)
{
    //std::string sType(pcType);
    if (sPlayer == "human")
        vPlayers.emplace_back(Player::MakePlayer(PlayerType::TYPE_HUMAN));
    else if (sPlayer == "ai")
        vPlayers.emplace_back(Player::MakePlayer(PlayerType::TYPE_AI));
    else if (sPlayer == "server")
        vPlayers.emplace_back(Player::MakePlayer(PlayerType::TYPE_CLIENT));
    else if (sPlayer == "client")
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

static std::unique_ptr<Game> GenerateGame(std::string sGame)
{
    if (sGame == "connectfour")
        return Game::Make(GameType::TYPE_CONNECT_FOUR);
    else if (sGame == "ttt")
        return Game::Make(GameType::TYPE_TTT);
    else if (sGame == "reversi")
        return Game::Make(GameType::TYPE_REVERSI);
    else if (sGame == "chess-gardner")
        return Game::Make(GameType::TYPE_CHESS_GARDNER);
    else if (sGame == "chess-baby")
        return Game::Make(GameType::TYPE_CHESS_BABY);
    else if (sGame == "chess-jm")
        return Game::Make(GameType::TYPE_CHESS_JACOBS_MEIROVITZ);
    else if (sGame == "chess-petty")
        return Game::Make(GameType::TYPE_CHESS_PETTY);
    else if (sGame == "chess-speed")
        return Game::Make(GameType::TYPE_CHESS_SPEED);
    else if (sGame == "chess-quick")
        return Game::Make(GameType::TYPE_CHESS_QUICK);
    else if (sGame == "chess-elena")
        return Game::Make(GameType::TYPE_CHESS_ELENA);
    else if (sGame == "chess-attack")
        return Game::Make(GameType::TYPE_CHESS_ATTACK);
    else if (sGame == "chess-los-alamos")
        return Game::Make(GameType::TYPE_CHESS_LOS_ALAMOS);
    else if (sGame == "chess")
        return Game::Make(GameType::TYPE_CHESS);
    else if (sGame == "war")
        return Game::Make(GameType::TYPE_CARDGAME_WAR);
    else if (sGame == "go-fish")
        return Game::Make(GameType::TYPE_CARDGAME_GO_FISH);
    else if (sGame == "rummy")
        return Game::Make(GameType::TYPE_CARDGAME_BASIC_RUMMY);
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

static void SetPlayers(std::string sName, int nPlies1, int nPlies2,
                       int nProbability1, int Probability2,
                       std::string sPlayer1Name, std::string sPlayer2Name,
                       int nVerbosity, std::string sGameTitle,
                       std::string sHost, int nPort,
                       std::vector<std::unique_ptr<Player>> &vPlayers)
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

    // Set number of plies for AI players
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

    // Set level of probability for AI players
    if (nProbability1 > 0 && nProbability1 <= 9)
    {
        vPlayers[0]->SetProbability(nProbability1);
    }
    else
    {
        ShowUsage(sName);
        exit(EXIT_FAILURE);
    }

    if (Probability2 > 0 && Probability2 <= 9)
    {
        vPlayers[1]->SetProbability(Probability2);
    }
    else
    {
        ShowUsage(sName);
        exit(EXIT_FAILURE);
    }

    // Set game title
    vPlayers[0]->SetGameTitle(sGameTitle);
    vPlayers[1]->SetGameTitle(sGameTitle);

    // Set player names
    vPlayers[0]->SetPlayerName(sPlayer1Name);
    vPlayers[1]->SetPlayerName(sPlayer2Name);

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
    // Handles for generated game and players
    std::unique_ptr<Game> pcGame {};
    std::vector<std::unique_ptr<Player>> vPlayers;

    // Strings to hold game and players from getopt
    std::string sGame            {"unknown"};
    std::string sPlayer1         {"unknown"};
    std::string sPlayer2         {"unknown"};

    std::string sPlayer1Name     {};
    std::string sPlayer2Name     {};
    std::string sInputFile       {};
    std::string sOutputFile      {};

    // Depth level for minimax
    int  nPlies1                 {4};
    int  nPlies2                 {4};

    // Probability thresholds for Bayesian
    int nProbability1            {5};
    int nProbability2            {5};

    // Default network options
    int  nPort                   {60000};
    std::string sHost            {"127.0.0.1"};

    // Default verbosity
    int  nVerbosity              {1};

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
        {"player1",      required_argument, nullptr, '1'},
        {"player2",      required_argument, nullptr, '2'},
        {"plies",        required_argument, nullptr, 'p'},
        {"plies1",       required_argument, nullptr, 'x'},
        {"plies2",       required_argument, nullptr, 'y'},
        {"probability1", required_argument, nullptr, 'b'},
        {"probability2", required_argument, nullptr, 'c'},
        {"name1",        required_argument, nullptr, 'n'},
        {"name2",        required_argument, nullptr, 'm'},
        {"game",         required_argument, nullptr, 'g'},
        {"port",         required_argument, nullptr, 't'},
        {"host",         required_argument, nullptr, 'h'},
        {"input",        required_argument, nullptr, 'i'},
        {"output",       required_argument, nullptr, 'o'},
        {"verbose",      required_argument, nullptr, 'v'},
        {"help",         no_argument,       nullptr, 'H'},
        {"version",      no_argument,       nullptr, 'V'},
        {NULL,           0,                 nullptr,  0}
    };

    // Execute getopt_long
    int nC = 0;
    int nOptionIndex = 0;
    while ((nC = getopt_long(argc, argv, "1:2:p:x:y:b:c:n:m:g:t:h:i:o:v:HV", stLongOptions, &nOptionIndex)) != -1)
    {
        switch (nC)
        {
            // Player 1
            case '1':
                sPlayer1.assign(optarg);
                break;
            // Player 2
            case '2':
                sPlayer2.assign(optarg);
                break;
            // Plies for player 1 and player 2
            case 'p':
                nPlies1 = nPlies2 = atoi(optarg);
                break;
            // Plies for player 1
            case 'x':
                nPlies1 = atoi(optarg);
                break;
            // Plies for player 2
            case 'y':
                nPlies2 = atoi(optarg);
                break;
            // Probability for player 1
            case 'b':
                nProbability1 = atoi(optarg);
                break;
            // Probability for player 2
            case 'c':
                nProbability2 = atoi(optarg);
                break;
            // Name for player 1
            case 'n':
                sPlayer1Name = optarg;
                break;
            // Name for player 2
            case 'm':
                sPlayer2Name = optarg;
                break;
            // Game
            case 'g':
                sGame.assign(optarg);
                break;
            // Port
            case 't':
                nPort = atoi(optarg);
                break;
            // Host
            case 'h':
                sHost = optarg;
                break;
            // Input
            case 'i':
                sInputFile = optarg;
                break;
            // Output
            case 'o':
                sOutputFile = optarg;
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
                throw GameAIException("getopt returned character code ", std::to_string(nC));
                break;
        }
    }

    // Generate game
    pcGame = GenerateGame(sGame);

    // Generate players
    if (!GeneratePlayer(sPlayer1, vPlayers))
    {
        std::cerr << "unknown player type " << optarg << std::endl;
        exit(EXIT_FAILURE);
    }

    if (!GeneratePlayer(sPlayer2, vPlayers))
    {
        std::cerr << "unknown player type " << optarg << std::endl;
        exit(EXIT_FAILURE);
    }

    // Ensure two players and a game are generated
    if ((vPlayers.size() != 2) || (pcGame == nullptr))
    {
        ShowUsage(argv[0]);
        exit(EXIT_FAILURE);
    }

    // Set player parameters
    SetPlayers(argv[0], nPlies1, nPlies2, nProbability1, nProbability2,
        sPlayer1Name, sPlayer2Name, nVerbosity, pcGame->Title(),
        sHost, nPort, vPlayers);

    // Set verbosity of game
    pcGame->SetVerbosity(nVerbosity);

    // Set game player data
    pcGame->SetPlayer1Type(vPlayers[0]->TypeName());
    pcGame->SetPlayer2Type(vPlayers[1]->TypeName());
    pcGame->SetPlayer1Name(vPlayers[0]->PlayerName());
    pcGame->SetPlayer2Name(vPlayers[1]->PlayerName());

    // If game sync flag was set, but no players are NetworkPlayers,
    // turn the sync flag off (if no networked players, syncing the
    // game is not required.)
    if (pcGame->Sync())
    {
        if (vPlayers[0]->Type() != PlayerType::TYPE_CLIENT)
            if (vPlayers[0]->Type() != PlayerType::TYPE_SERVER)
                if (vPlayers[1]->Type() != PlayerType::TYPE_CLIENT)
                    if (vPlayers[1]->Type() != PlayerType::TYPE_SERVER)
                        pcGame->SetSync(false);
    }

    // Announce game
    std::cout << "Playing " << pcGame->Title() << std::endl;
    std::string sDesc = pcGame->Description();
    if (!sDesc.empty())
        std::cout << sDesc << std::endl;

    // Announce players
    for (int iii = 0; iii < 2; ++iii)
    {
        std::cout << "Player " << iii + 1;
        if (vPlayers[iii]->PlayerName().size() > 0)
            std::cout << " Name " << vPlayers[iii]->PlayerName();

        std::cout << " Type " << vPlayers[iii]->TypeName();
        if (vPlayers[iii]->Type() == PlayerType::TYPE_AI)
        {
            if (pcGame->EnvironmentDeterministic())
            {
                std::cout << " Plies: " << vPlayers[iii]->Plies();
            }
            else
            {
                std::cout << " Probability Threshold: " << vPlayers[iii]->Probability() * 10 << "%";
            }

        }

        std::cout << std::endl;
    }

    std::cout << "Start game\n" << std::endl;

    //
    // If input file specified, read and apply game moves
    //

    // nPlayer used to capture next player's turn after reading and applying moves
    // Also used in the player turn loop below.  Initially set to a 1.
    int nPlayer = 1;
    bool bGameNotEnded = true;

    // If input file specified, read and apply moves.
    // Returns number of Player-To-Play-Next, 1 or 2; or 0 if error.
    if (!sInputFile.empty())
    {
        nPlayer = pcGame->ReadMovesFromFile(sInputFile);
    }

    // Evaluate game ended before moving into player turns loop
    // If nPlayer == 0, an error occured.
    if (nPlayer == 0)
    {
        std::cerr << "Invalid move.  Exiting." << std::endl;
        exit(EXIT_FAILURE);
    }
    // Evaluate game state from Player-To-Play-Next's perspective.
    // If game ended, allow opponent to finish.
    else
    {
        if (pcGame->GameEnded(nPlayer))
        {
            vPlayers[nPlayer - 1]->Finish(*pcGame);
            bGameNotEnded = false;
        }
    }

    // Used for reporting the score
    std::string sGameScore {};

    // Have each player play in turn
    while (bGameNotEnded)
    {
        if (!vPlayers[nPlayer - 1]->Move(*pcGame))
        {
            std::cerr << "Invalid move. Exiting." << std::endl;
            exit(EXIT_FAILURE);
        }

        // Announce game score
        sGameScore = pcGame->GameScore();
        if (!sGameScore.empty())
        {
            std::cout << sGameScore << std::endl;
        }

        // Evaluate game state from opponent's perspective.  If game ended, allow opponent to finish.
        // Then break from loop.
        if (pcGame->GameEnded(3 - nPlayer))
        {
            vPlayers[2 - nPlayer]->Finish(*pcGame);
            break;
        }

        // Move to next player
        nPlayer = 3 - nPlayer;
    }

    // Display game
    pcGame->Display();

    // Summarize game
    if (pcGame->Winner() == 0)
    {
        std::cout << "Game drawn " << pcGame->WinBy() << std::endl;
    }
    else
    {
        std::cout << "Game won by Player " << pcGame->Winner() << std::endl;
        std::cout << "Game won with " << pcGame->WinBy() << std::endl;
    }

    std::cout << "Total number of moves: " << pcGame->NumberOfMoves() << std::endl;

    std::string sGameStats = pcGame->GameStatistics();
    if (!sGameStats.empty())
        std::cout << "Game Statistics:\n" << sGameStats << std::endl;

    // If output file specified, write game moves
    if (!sOutputFile.empty())
        pcGame->WriteMovesToFile(sOutputFile);
}
