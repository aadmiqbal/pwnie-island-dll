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

Below hacks can be initiated through the chat in the game by writing the associated commands:

- **Mana hack (set mana {int}):** You can alter the mana value in the game
- **Health hack (set health {int}):** You can alter your health in the game
- **Walk Speed hack (set walkspeed {float}):** You can update your walking speed which can enable you to travel faster
- **Jump Speed hack (set jumpspeed {float}):** You can update your jump speed which will enable you to jump higher
- **Player X Coordinate hack (set x {float}):** You can set your X coordinate to a specific value
- **Player Y Coordinate hack (set y {float}):** You can set your Y coordinate to a specific value
- **Player Z Coordinate hack (set z {float}):** You can set your Z coordinate to a specific value
- **Inventory left hack (set inventoryLeft {int}):** You can alter the quantity of the item in your 1st inventory slot
- **Inventory parent hack (set inventoryParent {int}):** You can alter the quantity of the item in your 2nd inventory slot
- **Inventory right hack (set inventoryRight {int}):** You can alter the quantity of the item in your 3rd inventory slot
- **Get gun (get gun):** You can equipt yourself another pistol once you purchase 1 (we need the object to be created first)
- **:** DISPLAY NAME HACK FOR BEARS
- **:** DISPLAY NAME HACK FOR RATS
- **Set peace mode:** By enabling the peace mode, the enemies wouldn't attack you
- **Trampoline hack (init trampoline):** By holding the space bar you can constantly remain in the air

In addition to above hack which can be triggered by the user's choice using the chat functionality, we have also implemented a hack which is hooked to the game as soon as the DLL is injected:

**Loneliness mode:** By pressing left SHIFT key, you can push the bear the player's facing towards to the sky

## **Minigame**
- **Space Invaders (init spaceInvaders):**
