# SCOL
Allows you to load RAGE compiled scripts (`.sco` files) for Grand Theft Auto V Enhanced. Strictly for educational purposes.

## How It Works
Although the SCO format is never used in GTA V, the game engine still has the support for loading them. However, scripts compiled for GTA IV are not directly compatible due to different SCO header and incompatible natives and globals.

SCOL works by redirecting the script thread creation function to the unused SCO loader. This function loads a `.sco` file from a specified path on disk and creates a script program and thread for it automatically.

An example script to test can be found [here](https://github.com/ShinyWasabi/SCOL/blob/main/hello_world.sco).

If you want to load YSC scripts, you need to convert them to SCO (without the RSC7 header) using [YSC2SCO.exe](https://github.com/ShinyWasabi/SCOL/blob/main/YSC2SCO.exe). I won't provide the source code for this program due to reasons.

## Installation
- Download [SCOL.asi](https://github.com/ShinyWasabi/SCOL/releases/download/SCOL/SCOL.asi).
- Download [xinput1_4.dll](https://github.com/ThirteenAG/Ultimate-ASI-Loader/releases/download/x64-latest/xinput1_4-x64.zip).
- Place both files in the game directory where `GTA5_Enhanced.exe` is located.
- Launch the game. Make sure BattlEye is disabled.

## Usage
- By default, scripts are loaded from the game's current directory. You can change this in `SCOL.ini`.
- The default key to reload all scripts is `F5`. This is also configurable in the `.ini` file.
- When a script is loaded for the first time, a new section is added to `SCOL.ini`:
```ini
[examplescript]
Args = 0
ArgCount = 0
StackSize = 1424
```
You can edit these values as needed for your script. Use the `0,1,2,3` format to pass arguments.

## Natives
SCOL registers additional native commands that can be used in your scripts. See the documentation [here](https://github.com/ShinyWasabi/SCOL/blob/main/NATIVES.md).