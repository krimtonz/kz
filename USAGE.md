# User Manual

## 0 Table of Contents
- [User Manual](#user-manual)
  - [0 Table of Contents](#0-table-of-contents)
  - [1 Introduction](#1-introduction)
  - [2 Menus](#2-menus)
    - [2.1 Warps](#21-warps)
    - [2.2 Cheats](#22-cheats)
    - [2.3 Scene](#23-scene)
      - [2.3.1 Collision](#231-collision)
      - [2.3.2 Camera](#232-camera)
      - [Controlling the Free Camera](#controlling-the-free-camera)
  - [2.4 States](#24-states)
    - [2.5 Watches](#25-watches)
    - [2.6 Inventory](#26-inventory)
      - [2.6.1 Items](#261-items)
      - [2.6.2 Masks](#262-masks)
      - [2.6.3 Quest Status](#263-quest-status)
      - [2.6.4 Amounts](#264-amounts)
    - [2.7 Equips](#27-equips)
    - [2.8 File](#28-file)
    - [2.9 Debug](#29-debug)
      - [2.9.1 Memory](#291-memory)
      - [2.9.2 Objects](#292-objects)
      - [2.9.3 Actors](#293-actors)
      - [2.9.4 Flags](#294-flags)
      - [2.9.5 VC Debug](#295-vc-debug)
    - [2.10 Settings](#210-settings)
  - [3 Save States](#3-save-states)
  - [4 Lite Version](#4-lite-version)
  - [5 Commands](#5-commands)
  - [6 Keyboard](#6-keyboard)
  - [7 SD Features](#7-sd-features)
  - [8 Memfiles](#8-memfiles)
  - [9 Frame Advancing](#9-frame-advancing)
  - [10 Pause Screen Editor](#10-pause-screen-editor)

## 1 Introduction
The main interface for kz, is used by bringing up the menu which by default is done by pressing the `R + L` Buttons.

The menu is navigated with the dpad buttons, and items are activated with the `L` button.

**Note:** On WiiVC the `L` Button is mapped to the `c-stick down` by default.

kz also implements the following features beyond the menu:
* An input viewer
* A lag counter
* A real-time timer
* Various commands.
* An inventory editor within the pause screen.

All of these are configurable in the [settings menu](#210-settings)

If when rebinding the activate menu command, you find yourself in a state where the menu cannot be activated, entering the konami code (up up down down left right left right B A) will reset ALL settings to their default values.

## 2 Menus
### 2.1 Warps
The warps menu contains a listing of all the entrances in the game categorized into 10 categories.  These categories are based on the central clock town area, the 4 cardinal directions of normal gameplay, overworld, and other miscellaneous scenes.  Under each category is a listing of scenes, if a scene has multiple entrances selecting that scene will enter a menu with all available entrances. Selecting an entrance will immediately warp you to that entrance.  If a scene only has one entrance, selecting that scene will warp you to that single entrance.

**Note:** The Beta category is only available in the 1.0 japanese version of the game.

**Note:** By default entrances that are known to crash are disabled, if kz was compiled with these entrances enabled, using these entrances will most likely crash.

***Warning:*** Warping while not in a normal playing state of the game (i.e. spinning n64 logo, file select screen) will cause *undefined beahvior*
### 2.2 Cheats
The cheats menu will allow you to enable/disable various cheats.

Arrows, bombs, bombchus, powder kegs, sticks, nuts, health, magic, and rupees will all enable infinite amounts of these items.

`no blast mask cooldown` will allow you to use the blast mask immediately after using it.

`no item restriction` will allow you to use any item as any form.
**Note:** The fierce diety mask is a special case.  While the cheat will work for this mask, the c button will still be dimmed out though it is usable.  Putting the mask on or off will cause the HUD to be faded out until a scene change occurs.

`isg` will keep link's sword in a swinging state mimicking the infinite sword glitch.  Disabling the cheat will not instantly stop isg, you must cancel using the normal isg cancellation methods.  
**Note:** while the cheat is enabled, the normal cancellations for isg will not work i.e non-targeted shielding.

`freeze time` will not allow the 3 day timer to progress.
### 2.3 Scene
The scene menu contains information about the currently loaded scene.
**current room** shows you which room is currently loaded.  If no room is currently loaded 255 will be displayed.  
**num rooms** shows you how many rooms are in the current scene.  
**unload room** will unload all rooms in the current scene.  
**load room** will load the room specified by the index next to the button, if the index is the currently loaded room, then the room is unloaded.  If the index is larger than the number of rooms in the scene, no action is taken.

#### 2.3.1 Collision
The collision menu will allow control over kz's collision viewer feature.  Enabling the collision viewer will draw polygons over the scene's collision.  Under the enable checkbox the collision viewer has these options:
* **reduce** When this option is enabled only special types of collision are displayed.  This will use less memory and cause less graphics lag.
* **opaque** When this option is enabled the polygons that are displayed will not be see through and will appear as solid colors.
* **wireframe** When this option is enabled lines will be drawn around each polygon.  **Note:** This option typically causes heavy lag.
* **autoupdate** When this option is enabled dynamic collision will also be displayed, also when a new scene is loaded the collision will be regenerated for the new scene.

The collision viewer uses these colors to identify special types of collision
* **blue** the surface is hookshotable
* **purple** the surface can be interacted with, such as ladders of vines
* **red** special void planes
* **cyan** loading zones
* **green** surfaces that can cause damage or slow link down
* **yellow** slippery slopes
* **white** normal collision 

The hide room option will prevent the game from rendering the currently loaded room.

The hitbox viewer will display the colliders, the opaque option is the same as it is for the collision viewer.  The hitbox viewer will display hitboxes with the following colors:

* **Red:** causes damage when collided with a blue hitbox.
* **Blue:** hitbox which can receive damage from a red hitbox
* **White:** a hitbox which does not cause damage, but is meant for bumping

The hide actors option will prevent actors from being rendered visually.

***Warning:*** Lite versions of kz are more prone to crashing when the collision and hitbox viewers are enabled without the **reduce** option enabled for larger scenes such as dungeons.

#### 2.3.2 Camera
The camera menu contains a few toggles for enabling kz's free camera.

**enable free cam**  
enable kz's free camera.

**lock free cam**  
this will release the button restrictions that are placed when the free camera is enabled, but keep the camera at the current free cam position.  
  
**set cam pos to link pos** this will move the free camera's position to links current position

#### Controlling the Free Camera
Using the control stick will rotate the camera.  
If holding **Z** the camera will roll when moving the control stick left or right.

**c-up** will move the camera forwards (zoom in)  
**c-down** will move the the camera backwards (zoom out)  
**c-right** will move the camera to the right (strafe right)  
**c-left** will move the camera to the left (strafe left)

When using the c buttons to move the camera, holding the **Z** button will move the camera faster.

## 2.4 States
The states menu will allow you to change the active savestate slot, create a name for your save state, and export and import states to the SD card.  See the [states section](#3-save-states) for more information on save states.

**Note**: sd card features are only enabled for the WiiVC version of kz.

**cur state** This will allow you to switch the change the active state. kz allows the saving of up to 5 states numbered from 0 to 4 use the **<** and **>** buttons to switch between them.  
**state name** underneath the **cur state** section there will be a name for the current state.  **no state** will be displayed if currently no state is saved in the selected state.  **untitled** will be displayed as the default name when a state is created in the selected slot for the first time.  Selecting the state name will bring up a keyboard to rename the state.  
**export** will allow you to export the currently saved state to the sd card  
**import** will allow you to select an exported state to import to the current selected slot.

### 2.5 Watches
The Watches menu provides a list of custom memory watches

Hitting the `+` button will add a new watch.  You can add up to 10 watches. This will add a new row to the watch table which has the following elements:

* Delete Button - Removes this watch row
* Float Button (cloud)/Anchor Button
Causes the watch to be stuck on the screen, even when the watches menu is closed.  When the watch is changed to floating, the button icon changes to an anchor.  Pressing the anchor button will anchor the watch to the watches menu
* Move Button
This is only displayed when the watch is floating,  Pressing this button will allow you to move the watch around the screen.  Holding the `Z` Button will allow you to move the watch faster
* Address
This is the address of the watch.  The address can be in any range of `0x80000000` - `0x807FFFFF` for lite versions or `0x80000000` - `0x80BFFFFF` for full versions.  If the address is unaligned with the address type, then the address is changes to the closest aligned address.  Pressing this value will allow you to change the address
* Address Type
This is how the watch will be displayed.  Pressing this value will allow you to change how the watch will be displayed, using the `dpad-up` and `dpad-down` buttons will change the value
These are the options:
    * s8 - a signed 8 bit integer  (1 byte)
    * u8 - an unsigned 8 bit integer (1 byte)
    * x8 - an 8 bit integer displayed in hexadecimal (1 byte)
    * s16 - a signed 16 bit integer  (2 bytes)
    * u16 - an unsigned 16 bit integer (2 bytes)
    * x16 - a 16 bit integer displayed in hexadecimal (2 bytes)
    * s32 - a signed 32 bit integer  (4 bytes)
    * u32 - a unsigned 32 bit integer (4 bytes)
    * x32 - a 32 bit integer displayed in hexadecimal (4 bytes)
    * f32 - a 32 bit single precision floating point value (4 bytes)
* Watch
This is the watch's value
* Label
This is a custom label that can be applied to the watch for easy identification,  pressing this value will bring up the on screen keyboard. see [keyboard](#5-keyboard)
### 2.6 Inventory
This is the main inventory editor in kz which has the following submenus:
#### 2.6.1 Items
This is a listing of the items that are available on the **Select Item** Subscreen in the pause menu.  Selecting any of the items will make that item available in your inventory.

Pulling up an item upgrade list which are indicated by the white squares on the left of the menu will pull up an item wheel for available upgrade values.  The wheel can be navigates a single item at a time by using the `dpad-left` and `dpad-right` buttons, or three items at a time by using the `dpad-up` and `dpad-down` buttons.

Bottles are listed at the bottom row for each of the 6 bottle slots.  Like the item upgrades, selecting a bottle slot will bring up an item wheel with all available bottle options.

**Note:** In the Japanese versions of the game there are several empty bottle options, these are actual item values in the game and are retained for accuracy

Trade Quest Items are listed at the right of the screen.  Selecting these items will bring up an item wheel its available trade quest items.

**Note:** In the Japanese versions of the game, there are several Moon's Tear Items which all behave the same.  These are retained for accuracy.

#### 2.6.2 Masks
This is a listing of all the masks that are available on the **Masks** Subscreen.  Selecting any of the masks will make them available in your inventory

#### 2.6.3 Quest Status
This is a listing of all the various options that are available in the **Quest Status** Subscreen, and other upgrades that are gotten thoughout the game.

The **max health** will allow you to raise your current maximum health.  Health is kept track of in increments of 16ths, so raising this value by 1 will incremeant your max health by 1/16.  Raising the value by 0x10 (16) will increase your max health by 1 full heart.

The **double defense** option will enable double defense

The **magic** option will enable magic.

The **double magic** option will enable double magic.
**Note:** If double magic is enabled while magic is disabled, it will enable magic, and vice-versa if magic is disabled while double magic is enabled, it will disable double magic.

The **dungeon list** will affect the **small keys**, **stray fairies**, **boss key**, **compass**, and **map** options.  Changing which dungeon is selected will show the current status of these options for that dungeon.  Changing the small key amount, stay fairies amount, boss key switch, compass switch, or map switch will be reflected for that dungeon only.

The four **remains switches**, and **bomber's notebook switch** will give you those items in your inventory.

The first 10 **owl switches** will enable/disable those owl locations.  The final owl switch is for the hidden south clock town owl.

The first 10 **song switches** will enable/disable the songs as they appear in your invtentory.  The 11th song is the lullaby intro, and the final two are songs left over from Ocarina of Time, and are playable, though they are misrepresented in the quest status subscreen.

#### 2.6.4 Amounts
The **amounts** menu allows you to change the ammo amounts for items in your inventory as well as the current amount of magic and health that you have.

### 2.7 Equips
The equips menu allows you to change your currently equiped sword and shield as well as which items are equipped on the B Button, and the three C Buttons.  Changing which sword is equipped will also change the sword/shield you have in the **Quest Status** Subscreen.

Selecting the B Button equip, or one of the three C Button equips will bring up an item wheel to select which item to equip to that button. See [2.5.1 Items](#251-items) for navigation of the item wheel.

***Warning:*** While you may be able to equip any of these items to you buttons, using them may cause the game to crash.

### 2.8 File
The file menu lets you modify data that pertains to the current save file.

**have tatl** will set the flag where you can summon tatl.  
**intro watched** will set the flag where the intro up to entering south clock town for the first time has been complete  
**great spin** will set the flag for having obtained great spin  
**stored song** will allow you to select which song is currently pending action.   **Note:** Only songs which display a textbox will be affected.  
**current day** will allow you to change which day you are on  **time of day** will allow you to change what the current time of day is.  Because this is in hexadecimal, here are a few values to help understand it:

* 0x4000 - 6 AM
* 0x8000 - 12 PM
* 0xC000 - 6 PM
* 0x0000 - 12 AM

Hours are typically incremented by 0xAAA, but this is not exact in all cases.

**time speed** allows you to change how quickly the 3 day timer is moving.  The value is added to 5 to get the actual speed, setting this to -5 will freeze time.

### 2.9 Debug
The debug menu allows you to see and modify at a lower level to what is happening within the game.  There are 4 different menus.

#### 2.9.1 Memory
The memory menu allows you to see and edit the entirety of the games RAM.

Underneath the return button is the `starting address` This is the address of the first address in the first row.  Changing this value will change which values are being shown.

Next to the starting address is the `value type` changing this between `byte`, `halfword`, and `word` will change how the data is showing and how it can be modified.

Underneath the starting address is the starting address of each row. Underneath the value type is the offset of each column.

Underneath the column offsets are the memory values/number inputs for each address.  Selecting these will allow you to change the memory at that location.

#### 2.9.2 Objects
The objects menu allows you to see the game's object context.

Underneath the return button is the `id specifier` this is the object id that will be loaded when the push button is hit.

Next to the id specifier is the the `push` button, when this button is hit, the object specified by the id specifier is pushed to the object context, and is loaded immediately.

Next to the push button is the `pop` button, when this button is hit, the last item in the object content is removed from the content, and that memory is freed.

***Warning:*** Pushing an object which needs more memory than is available in the game will cause a crash, and popping an object that is required will also cause a crash.

#### 2.9.3 Actors
The actors menu allows you to modify the currently loaded actors in the game.

Underneath the return button is the `type specifier` changing this value will change the actor address to the first actor of that type.  There are 16 different actor types, which the final 4 are unknown.

Underneath the type specifier are the `previous` and `next` buttons, hitting these buttons will change the actor address to the prevous actor of that type, or the next actor of that type respectively.  Hitting previous on the first actor of the current type will change the actor to the final actor of that type, while hitting next of the final actor of that type will change the actor to the first actor.

Underneath the previous and next buttons is the `actor address` button.  This will show the address of the currently selected actor.  Hitting this button will take you to the [memory editor](#291-memory) at that actor's address.

Underneath the actor address button are the `goto`, `view`, and `delete` buttons.  Hitting the goto button will move link to that actor's position.  Hitting the view button will move the camera to that actor for a few frames, then return to link.  Holding the view button will keep the camera at that actor.  Hitting the delete button will immediately delete and unload that actor.

Underneath the goto, view, and delete buttons is the `actor id specifier`.  This is the actor id to be loaded for the spawn function.

Underneath the actor id specifier is the `variable specifier`.  This is the actor variable that is used for the spawn function.

Underneath the variable specifier are the `position and rotation specifiers` these are the position and rotation values in xyz format that are used for the spawn function.

Underneath the position and rotation specifiers is the `spawn` button.  Pressing the spawn button will spawn the actor with id specified by the actor id specifier, with the variable specified by the variable specifier located at the position indicated by the position specifiers, with the rotation specified by the rotation specifiers.
**Note:** An actor will only spawn in the required [objects](#292-objects) have been loaded.

Next to the spawn button is the `fetch from link` button.  Pressing this button will set the position and rotation specifiers to link's current position and rotation.

#### 2.9.4 Flags
The flags menu will show which flags have been set.  Flags that have been set will be indicated by a green flag, and flags that have not been set will be indicated by a red flag.  Pressing the flag will turn that flag on or off.

Underneath the return button is the `previous` and `next` flag set buttons.  Hitting these buttons will change which flag set is being shown.

Next to the previous and next buttons is the current flag set indicator.

Underneath the current flag set indicator are the bit offsets for the flags.  Next to the previous and next offset buttons are the current flag offsets.

The `previous` button is indicated by the ^ button.  This will move the current flag set's offset back by 2.

The `next` button is indicated by the v button.  This will move the current flag set's offset forward by 2.

**Note:** The switch, chest, clear, and collect flag sets are for the ***current scene***

#### 2.9.5 VC Debug
The VC Debug menu will display some statistics about the current state of the VC emulator.

**dump vc mem** will dump the Wii's memory to the SD card.

**MEM1 Free**: Displays the amount of memory that is free in the Wii's mem1 region's heap.  
**MEM2 Free**: Displays the amount of memory that is free in the Wii's mem2 region's heap.  
**Recomp 1**: Display the amount of memory that is free in the VC Emulator's small block code recompilation area.  
**Recomp 2**: Displays the amount of memory that is free inthe VC Emulator's large block code recomplication area.

If `MEM1 Free`, `Recomp 1`, and `Recomp 2` are all very low a VC Crash may be imminent.  

### 2.10 Settings
The settings menu allows you to modify kz specific settings.

The `profile` allows you to change which settings profile you are currently using.

The `save settings` button will save the currently loaded settings to flash memory to the indicated profile.

The `load settings` button will load the saved profile from flash memory to the currently selected profile.  If no settings are saved at that profile, default settings are loaded.

The `load default settings` button will load the default settings to the currently selected profile.

The `input display checkbox` will enable the input display.  Next to the input display text is a `input display move` button that will allow you to change the location of the input display.  Holding the `Z` button while moving the input display will move it faster.

The `lag counter checkbox` will enable the lag counter.  Next to the lag counter text is a `lag counter move` button that will allow you to change the location of the lag counter.  Holding the `Z` button while moving the lag counter will move it faster.

The `timer checkbox` will enable the timer  Next to the timer text is a `timer move` button that will allow you to change the location of the timer.  Holding the `Z` button while moving the timer will move it faster.

The `turbo hold checkbox` will change the behavior of the turbo command.  By default the turbo command is a toggle, setting this option will change it to a hold command.

The `memfile increase/decrease` button will allow you to change which memfile slot is being used.  You can use up to 3 memfiles.

The `export` button will allow you to export the memfile to an sd card.  The memfile from the currently selected slot is the memfile exported.  If no memfile is currently saved at this slot, no action is taken.  See [sd features](#7-sd-features) for file management.

The `import` button will allow you to import a memfile from the sd card into the currently selected memfile slot.  See [sd features](#7-sd-features) for file management.

The `memfile action` option list will allow you to select an action when a memfile is loaded.
* `void` will cause a scene void out
* `load position` will cause the current saved positoin slot to be loaded

The `saved position increase/decrease` buttons will allow you to change which position slot is being used.  You can use up to 3 positions.

The `export` button will allow you to export the position save to an sd card.  The saved position from the currently selected slot is the saved position exported.  If no saved position is currently saved at this slot, no action is taken.  See [sd features](#7-sd-features) for file management.

The `import` button will allow you to import a saved position from the sd card into the currently selected saved position slot.  See [sd features](#7-sd-features) for file management.

The `commands` menu will allow you to customize the bindings for command activation.  To change a binding, select the button combination or `none`.  This will change the text or button combination to green indication that input is pending.  You can use up to 4 buttons in an activator, and as soon as all buttons are released the binding is saved.  Press and hold the button combination to unbind a command.

## 3 Save States
Save states are implemented in a way which only saves information which is needed to reconstruct the state of the game when the state is created.  Because of this there are a few things that are not restored in the same state when the savestate was created.

* **Audio** Saving and restoring the entire audio state would greatly increase the size of states, therefore only parts of the audio system are saved to restore the audio to a functional state.  The correct sequences will be played, however they will not be restored to the same time in the sequence the state was saved at.  If a state is saved while audio is transitioning from one volume to another, the volume will be restored at the final volume level.
* **pause screen** The background image of the pause screen is not saved in the save state therefore if a state is saved from the pause screen and then loaded after another pause has been done, the most recent pause screen background will be displayed.
* **ace/srm** any locations in memory that are edited by these or other similiar glitches are not guaranteed to be saved in the save state.

## 4 Lite Version
The lite version of kz, allows most of the kz features to be used on systems that cannot take advantage of expanded memory space such as the N64, WiiUVC, and standard n64 emulators.

Because kz-lite must be placed in a much smaller memory area, features had to be elimiated in order to make this viable.  The [debug menu](#29-debug) is removed completely.

SD Features are not implemented in lite versions.

Item icons that are normally displayed in the [inventory menu](#26-inventory) are replaced with check boxes to enable/disable those items.  A tooltip is provided so which item is being switched is known.

***Warning:*** Because of the limited amount of memory available to the lite version, crashes may be more likely.  The `collision viewer` in particular should be used with care.

## 5 Commands
kz implements the following commands
* toggle menu - opens or closes the main menu
* return - returns to the previous menu
* levitate - classic "L to levitate" causes links to rise when held
* turbo - Increases link's linear velocity
* fall - allows link to fall through the ground
* reload scene - reloads the current scene at the last entrance
* void out - activates a void out
* break free - breaks out of a cutscene
* pause - pauses the game at the current frame. see [frame advancing](#9-frame-advancing)
* advance - advances a single frame. see [frame advancing](#9-frame-advancing)
* reset lag counter - resets the lag counter to 0
* start/stop timer - starts or stops the timer
* reset timer - sets the timer to 0
* title scrren - returns to the title screen
* save state - creates a save state
* load state - loads the current save state
* prev state - moves to the previous save state slot
* next state - moves to the next save state slot
* save memfile - saves a memfile to the current slot
* load memfile - loads a memfile from the current slot
* next memfile - moves to the next memfile slot
* prev memfile - moves to the previous memfile slot
* save position - saves link's current position to the current slot
* load position - set's link's position to the position saved in the current slot
* next position - moves to the next position slot
* prev position - moves to the previous position slot
* skip cutscene - skips a cutscene, playing the next one if exists

## 6 Keyboard
kz implements an on screen keyboard for easy text editing.  There are a few shortcuts to make editing a bit faster.

While holding `R`:
* `D-Left` : Backspace
* `D-Right` : space
* `D-Up` : Caps-Lock
* `D-Down` : Accept

## 7 SD Features
kz is able to communicate with the SD Card in the WiiVC Version.

When a feature is used that needs the sd card, a files menu will pop up.

Underneath the return button is the `reset disk` button.  If the SD Card is removed and reinserted or another sd card is used, this button will reset the state of the sd card and reload the current files on the disk.

Underneath the reset disk button displays the current directory.

When saving a file underneath the current directory is the filename input button.  When the button is clicked the [keyboard](#6-keyboard) is brought up to name the file.

When saving a file underneath the filename input button is the `accept` button.  Hitting this button will accept the new file name and return to the previous menu.  Next to the accept button is the `clear` button.  Hitting this button will clear out the filename.

When saving a file, there is a `create new folder` in the file list indicated by a folder icon with a `+`, pressing this button will bring up a keyboard to create a new folder in the current directory.

A normal file is indicated by a page icon next to the file name.  When saving a file and the file name is hit, the filename is set to that filename.  When loading a file and the filename is hit, that file is the file loaded.

A directory is indicated by a folder icon next to the file name.  Hitting the directory name will switch to that directory.

## 8 Memfiles

memfiles are a sort of psuedo-savestate.  They are much simpler than true save states, but still allow for powerful practice.

A memfile saves the entirety of the save context, as well as some other information needed for restoring that save context.

When a memfile is loaded in the same scene that the memfile was saved from, no action is taken.

When a memfile is loaded from a different scene that the memfile was saved from, link is warped to the entrance the memfile was saved from.

There are also a few other options for memfile loading that are confirgurable in the [settings menu](#210-settings) including the ability to load a position when loading a memfile, and voiding out when loading a memfile.

Memfiles are exportable and importable in versions that allow [sd features](#7-sd-features)

## 9 Frame Advancing
When the game is paused using the `pause` or `advance` commands, the game will stop frame execution at this point.  Use the `advance` command to advance to the next frame.  When the `advance` command is used when the game is not currently paused, the resulting action is the same as if you used the `pause` command.

## 10 Pause Screen Editor
Holding the `L` Button (`c-stick-down` on VC) while in the pause screen will enable inventory editing.  When this mode is enabled a red `i` will be shown in the lower right corner of the screen.  This mode will allow you to move the cursor into any item or mask slot in order to select an item to place in that slot.  When on the `Quest Status` subscreen, selecting each item in the menu with the `L` button will toggle that item as obtained or not.  
