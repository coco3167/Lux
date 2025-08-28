#include <set>
#include <stdio.h>
#include <string.h>
#include <vector>

#include <iostream>
#include <fstream>


#include "lux/define.cpp"
#include "lux/kit.hpp"

#include "lux/game_objects.hpp"
#include "lux/map.hpp"

#include "CustomAgent/GameDatas.h"
#include "CustomAgent/MetaAI.hpp"

using namespace std;
using namespace lux;

int main()
{
    kit::Agent gameState = kit::Agent();
    // initialize
    gameState.initialize();

    Player& player = gameState.players[gameState.id];
    GameMap& map = gameState.map;
    GameDatas gameDatas{ map, &player };

    int turn = 0;

    MetaAI metaAI{ &gameDatas };

    ofstream outputDebugFile(Utils::FormatString("debug_%i.txt", player.team).c_str());

    while (true)
    {
        /** Do not edit! **/
        // wait for updates
        gameState.update();

        vector<string> actions = vector<string>();

        /** AI Code Goes Below! **/

        Player& player = gameState.players[gameState.id];
        Player& opponent = gameState.players[(gameState.id + 1) % 2];
        
        actions.push_back(std::move(Annotate::sidetext("Start Update")));
        gameDatas.Update(&actions, &player, turn);

        metaAI.Update(turn);
        actions.push_back(std::move(Annotate::sidetext("End Update")));

        turn++;

        // you can add debug annotations using the methods of the Annotate class.
        // actions.push_back(Annotate::circle(0, 0));

        /** AI Code Goes Above! **/

        /** Do not edit! **/

        outputDebugFile << "Turn : "  << turn << "\n";
        for (int i = 0; i < actions.size(); i++)
        {
            if (i != 0)
                cout << ",";
            cout << actions[i];
            outputDebugFile << actions[i] << "\n";
        }
        cout << endl;
        outputDebugFile << endl;
        // end turn
        gameState.end_turn();
    }

    return 0;
}
