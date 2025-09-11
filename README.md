# Lux
## AI functionalities

### MetaAI
We've got a MetaIA controlling different parts of the global state of the player, it assigns objectives to CityTiles, Cities and Units (Workers and Carts).</br>
Those smaller parts each have specific IAs to make them accomplish their objectives.</br>
Some use State Machines when needed and other stays with simple actions.

### World data
To have a nice programming environment we also created a pathfinding component (with flags to customize the needed route) to help units navigate the world and an GameData object compiling all needed informations for the AI to process.

### Debug
We also used a lot of debug to help us understand where the AI was failing (which it did quite a lot, and maybe still does), we used Lux available annotations and created a separated Debug class to easily watch game values in a debug file.

## Credits
Luneau Paul - Programmer</br>
Alain Arthur - Programmer</br>
Boblet Corentin - Programmer</br>
Sorcière - Emotional Support (cat) <3

## Useful links
Lux AI Challenge specs : https://www.lux-ai.org/specs-2021</br>
Lux AI Challenge Viewer : https://2021vis.lux-ai.org/
