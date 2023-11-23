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
- **Teleport hack (set tp {float},{float},{float}):** You can teleport to a specific position by passing the coordinates seperated with a comma
- **Inventory left hack (set inventoryLeft {int}):** You can alter the quantity of the item in your 1st inventory slot
- **Inventory parent hack (set inventoryParent {int}):** You can alter the quantity of the item in your 2nd inventory slot
- **Inventory right hack (set inventoryRight {int}):** You can alter the quantity of the item in your 3rd inventory slot
- **Get gun (get gun):** You can equipt yourself another pistol once you purchase 1 (we need the object to be created first)
- **Change bears' display name hack (change bearDisplayName {newName}):** You can change the display name of the bears in the game 
- **Change rats' display name hack (change ratDisplayName {newName}):** You can change the display name of the rats in the game 
- **Set peace mode ({enable peace mode}/"{disable peace mode}"):** By enabling the peace mode, the enemies wouldn't attack you
- **Trampoline hack (init trampoline):** By holding the space bar you can constantly remain in the air
- **Get fireball hack (get fireball):** Gives the player GreatBallsOfFire
- **Get pistol hack (get pistol):** Gives the player a Pistol
- **Get revolver hack (get revolver):** Gives the player a CowboyCoder
- **Get money hack (get money {int}):** Gives the player the integer quanity of pwncoins specified
- **Jump Time hack (set jumpTime {float}):** You can update your jump hold time which will enable you to jump for longer
- **Coordinate Control hack (coordControl):** can toggle on or off with "coordControl" command to enable holding keyboard keys to change coordinates directly: increase x (T), decrease x (F), increase y (Y), decrease y (G), increase z (U), decrease z (H). Makes traversal and flying around faster.



In addition to above hack which can be triggered by the user's choice using the chat functionality, we have also implemented a hack which is hooked to the game as soon as the DLL is injected:

**Loneliness mode:** By pressing left SHIFT key, you can push the bear the player's facing towards to the sky

## **Minigame**
- **Space Invaders (init spaceInvaders):** This is a mini-game where the bear's you have chosen are being thrown to the sky (use the LSHIFT key) and their gravity is hacked so they can't fall down. You can use your guns to shoot at them and kill them. Bears can run away and it's the challange of the game. Once the game is initiated, the display name of the bears are set to 'space invaders'
