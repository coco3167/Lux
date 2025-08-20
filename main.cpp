#include "lux/kit.hpp"
#include "lux/define.cpp"
#include <string.h>
#include <vector>
#include <set>
#include <stdio.h>

#include "CustomAgent/PathFinder.hpp"
#include "CustomAgent/Annotator.hpp"
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
  GameDatas gameDatas{ gameState.map,  player};

  int turn = 0;

  MetaAI metaAI{ gameDatas };

  while (true)
  {
    /** Do not edit! **/
    // wait for updates
    gameState.update();

    vector<string> actions = vector<string>();
    
    /** AI Code Goes Below! **/

    Player &player = gameState.players[gameState.id];
    Player &opponent = gameState.players[(gameState.id + 1) % 2];

    gameDatas.Update(&actions);
    GameMap &gameMap = gameState.map;

    metaAI.Update(turn++);

    // you can add debug annotations using the methods of the Annotate class.
    // actions.push_back(Annotate::circle(0, 0));

    /** AI Code Goes Above! **/

    /** Do not edit! **/
    for (int i = 0; i < actions.size(); i++)
    {
      if (i != 0)
        cout << ",";
      cout << actions[i];
    }
    cout << endl;
    // end turn
    gameState.end_turn();
  }

  return 0;
}
