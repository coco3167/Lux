#include <set>
#include <stdio.h>
#include <string.h>
#include <vector>


#include "lux/define.cpp"
#include "lux/kit.hpp"

#include "lux/game_objects.hpp"
#include "lux/map.hpp"

#include "CustomAgent/GameDatas.h"
#include "CustomAgent/MetaAI.hpp"
#include "CustomAgent/Debug.h"

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

    Debug::Init(player.team);

    while (true)
    {
        /** Do not edit! **/
        // wait for updates
        gameState.update();

        vector<string> actions = vector<string>();

        /** AI Code Goes Below! **/

        Debug::Log(Utils::FormatString("Turn : %i", turn - 1));
        Debug::Log("");


        Player& player = gameState.players[gameState.id];
        Player& opponent = gameState.players[(gameState.id + 1) % 2];
        
        gameDatas.Update(&actions, &player, turn);

        metaAI.Update(turn);

        turn++;

        // you can add debug annotations using the methods of the Annotate class.
        // actions.push_back(Annotate::circle(0, 0));

        /** AI Code Goes Above! **/

        /** Do not edit! **/

        Debug::Log("");
        Debug::Log("Actions : ");
        Debug::Log("---------");
        for (int i = 0; i < actions.size(); i++)
        {
            if (i != 0)
                cout << ",";
            cout << actions[i];
            Debug::Log(actions[i]);
        }
        cout << endl;
        // end turn
        gameState.end_turn();
        Debug::Log("============================================");
        Debug::Log("");
    }
    
    Debug::LogError("Out of the loop");
    return 0;
}
