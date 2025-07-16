# SCO
Allows you to load RAGE compiled scripts (`.sco` files) for Grand Theft Auto V Enhanced. Strictly for educational purposes.

## Installation
- Download [SCO.asi](https://github.com/ShinyWasabi/SCO/releases/download/SCO/SCO.asi).
- Download [xinput1_4.dll](https://github.com/ThirteenAG/Ultimate-ASI-Loader/releases/download/x64-latest/xinput1_4-x64.zip).
- Place both files in the game directory where `GTA5_Enhanced.exe` is located.
- Launch the game. Make sure BattlEye is disabled.

## Usage
- By default, scripts are loaded from the game's current directory. You can change this in `SCO.ini`.
- The default key to reload all scripts is `F5`. This is also configurable in the `.ini` file.
- When a script is loaded for the first time, a new section is added to `SCO.ini`:
```ini
[example.sco]
Args = 0
ArgCount = 0
StackSize = 1424
```
You can edit these values as needed for your script. Use the `0,1,2,3` format to pass arguments.

## Natives
SCO registers additional natives that can be used in your scripts. See the documentation [here](https://github.com/ShinyWasabi/SCO/blob/main/NATIVES.md).

An example script to test can be found [here](https://github.com/ShinyWasabi/SCO/releases/download/SCO/hello_world.sco).