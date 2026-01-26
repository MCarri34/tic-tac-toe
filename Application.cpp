#include "Application.h"
#include "imgui/imgui.h"
#include "classes/TicTacToe.h"
#include "Logger.h"

#include <fstream>
#include <string>

// Implementation notes:
// - Logger is initialized in GameStartUp() and rendered in RenderGame().
// - Save/Load uses a simple 9-character board state string stored in a file.
// - Winner/draw display and reset controls are shown in the Settings window.

namespace ClassGame {
        //
        // our global variables
        //
        TicTacToe *game = nullptr;
        bool gameOver = false;
        int gameWinner = -1;

        // Save/load stays in Application.cpp so TicTacToe.cpp only contains game rules.
        static const char* kSaveFilePath = "tictactoe_save.txt";

        //
        // game starting point
        // this is called by the main render loop in main.cpp
        //
        void GameStartUp() 
        {
            Logger::GetInstance().Initialize("GameLog.txt");

            game = new TicTacToe();
            game->setUpBoard();

            Logger::GetInstance().Log(LogLevel::Info, "TicTacToe started");
        }

        //
        // game render loop
        // this is called by the main render loop in main.cpp
        //
        void RenderGame() 
        {
                ImGui::DockSpaceOverViewport();
                //ImGui::ShowDemoWindow();

                ImGui::Begin("GameWindow");
                game->drawFrame();
                ImGui::End();

                // Game Control window (ALWAYS renders)
                ImGui::Begin("Game Control");

                if (ImGui::Button("Log Game Event"))
                    Logger::GetInstance().Log(LogLevel::Info, "Player made a move.");

                ImGui::SameLine();
                if (ImGui::Button("Log Warning"))
                    Logger::GetInstance().Log(LogLevel::Warning, "Invalid move attempted.");

                ImGui::SameLine();
                if (ImGui::Button("Log Error"))
                    Logger::GetInstance().Log(LogLevel::Error, "Game state corrupted.");

                ImGui::End();


                // Game Log window
                ImGui::Begin("Game Log");

                if (ImGui::Button("Clear"))
                    Logger::GetInstance().Clear();

                ImGui::SameLine();

                static int levelIndex = 0;
                const char* levels[] = { "Info", "Warning", "Error" };

                // Sync dropdown with logger state
                LogLevel currentLevel = Logger::GetInstance().GetConsoleLevel();
                levelIndex = (currentLevel == LogLevel::Info) ? 0 :
                            (currentLevel == LogLevel::Warning) ? 1 : 2;

                if (ImGui::Combo("Log Level", &levelIndex, levels, IM_ARRAYSIZE(levels)))
                {
                    Logger::GetInstance().SetConsoleLevel(
                        (levelIndex == 0) ? LogLevel::Info :
                        (levelIndex == 1) ? LogLevel::Warning :
                                            LogLevel::Error
                    );
                }

                ImGui::Separator();
                ImGui::BeginChild("LogScroll", ImVec2(0, 0), true);

                LogLevel minLevel = Logger::GetInstance().GetConsoleLevel();
                for (const auto& entry : Logger::GetInstance().GetEntries())
                {
                    if (entry.level < minLevel)
                        continue;

                    ImGui::TextUnformatted(entry.text.c_str());
                }

                ImGui::EndChild();
                ImGui::End();

                if (!game) return;
                if (!game->getCurrentPlayer()) return;
                
                ImGui::Begin("Settings");
                ImGui::Text("Current Player Number: %d", game->getCurrentPlayer()->playerNumber());
                ImGui::Text("Current Board State: %s", game->stateString().c_str());

                // Save / Load
                // The save file stores a 9-character state string such as "102020001".
                if (ImGui::Button("Save Game"))
                {
                    std::ofstream out(kSaveFilePath, std::ios::out | std::ios::trunc);
                    if (out.is_open())
                    {
                        out << game->stateString();
                        out.close();
                        Logger::GetInstance().Log(LogLevel::Info, "Saved game state");
                    }
                    else
                    {
                        Logger::GetInstance().Log(LogLevel::Error, "Failed to open save file for writing");
                    }
                }
                ImGui::SameLine();

                if (ImGui::Button("Load Game"))
                {
                    std::ifstream in(kSaveFilePath);
                    std::string s;

                    if (in.is_open())
                    {
                        in >> s;
                        in.close();

                        if (s.size() == 9)
                        {
                            game->setStateString(s);
                            gameOver = false;
                            gameWinner = -1;
                            Logger::GetInstance().Log(LogLevel::Info, "Loaded game state");
                        }
                        else
                        {
                            Logger::GetInstance().Log(LogLevel::Warning, "Save file did not contain a valid 9-character state");
                        }
                    }
                    else
                    {
                Logger::GetInstance().Log(LogLevel::Error, "Failed to open save file for reading");
                    }
                }

                if (gameOver) {
                    ImGui::Text("Game Over!");
                    if (gameWinner >= 0)
                        ImGui::Text("Winner: %d", gameWinner);
                    else
                        ImGui::Text("Winner: Draw");

                    if (ImGui::Button("Reset Game")) {
                        game->stopGame();
                        game->setUpBoard();
                        gameOver = false;
                        gameWinner = -1;

                        Logger::GetInstance().Log(LogLevel::Info, "Game reset");
                    }
                }
                ImGui::End();

                ImGui::Begin("GameWindow");
                game->drawFrame();
                ImGui::End();

        }

        //
        // end turn is called by the game code at the end of each turn
        // this is where we check for a winner
        //
        void EndOfTurn() 
        {
            Player *winner = game->checkForWinner();
            if (winner)
            {
                gameOver = true;
                gameWinner = winner->playerNumber();
                Logger::GetInstance().Log(LogLevel::Info, "Winner Detected.");
            }
            if (game->checkForDraw()) {
                gameOver = true;
                gameWinner = -1;
                Logger::GetInstance().Log(LogLevel::Warning, "Draw Detected.");
            }
        }
}
