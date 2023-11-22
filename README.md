# Team 11 Pwnie Island Hack

A Windows DLL for hacking Pwnie Island 3 (https://www.pwnadventure.com/) 

## Contributors
- Efe Suner
- Axel Yalmaz
- Adam Iqbal
- Arvin Ziaei
- Roshaan Sothilingam
- Osama Bokanan

## Hack List

Below hacks can be initiated through the chat in the game by writing the associated commands

- **Mana hack (set mana <int>):** You can alter the mana value in the game
- **Health hack (set health <int>):** You can alter your health in the game
- **Walk Speed hack (set walkspeed <float>):** You can update your walking speed which can enable you to travel faster
- **Jump Speed hack (set jumpspeed <float>):** You can update your jump speed which will enable you to jump higher
- **Player X Coordinate hack (set x <float>):** You can set your X coordinate to a specific value
- **Player Y Coordinate hack (set y <float>):** You can set your Y coordinate to a specific value
- **Player Z Coordinate hack (set z <float>):** You can set your Z coordinate to a specific value
- **Inventory 1 hack (set inventory 1 <int>):** You can alter the quantity of the item on your 1st inventory slot
- **Inventory 2 hack (set inventory 2 <int>):** You can alter the quantity of the item on your 2nd inventory slot
- **Inventory 3 hack (set inventory 3 <int>):** You can alter the quantity of the item on your 3rd inventory slot
- **Get gun (get gun):** You can equipt yourself another pistol once you purchase 1 (we need the object to be created first)
- **Set peace mode:** By enabling the peace mode, the enemies wouldn't attack you
- **Trampoline hack (init trampoline):** By holding the space bar you can constantly remain in the air
**Minigame**
- **Space Invaders (init spaceInvaders):**

In addition to above hack which can be trigger by user's choice, we have also implemented a hack which is hooked to the game as soon as the DLL is injected:

**Rat Display Name hack:** Rats are the first enemies the player faces in the game and we have changed their display names to "Team 11".
