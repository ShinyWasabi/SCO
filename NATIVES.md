### `NATIVE PROC WRITE_MEMORY(STRING name, STRING pattern, INT offset, BOOL rip, INT& patch[], BOOL protect) = "0xEEE74A05DE4C2A07"`

Writes the bytes in the patch array to the address found by pattern in `GTA5_Enhanced.exe`.

- **Parameters:**
  - `name (STRING)`: A unique name for your pattern (used for caching).
  - `pattern (STRING)`: The IDA-style pattern to scan for within `GTA5_Enhanced.exe`.
  - `offset (INT)`: The amount to add/subtract from the found address.
  - `rip (BOOL)`: Whether to rip the found address.
  - `patch (INT[])`: The bytes to be written to the found address. Each item in the array must be within the range of 0-255.
  - `protect (BOOL)`: Whether to change the memory protections. Use this when writing to the `.text` section.

**Example Usage:**
```
INT patch[1]
patch[0] = 235 // 0xEB
WRITE_MEMORY("ModelSpawnPatch", "48 8B 06 48 89 F1 FF 50 ? 84 C0 75 ? 31 F6 48 89 F0", 11, FALSE, patch, TRUE)
```

### `NATIVE FUNC INT READ_MEMORY(STRING name, STRING pattern, INT offset, BOOL rip) = "0x1E9F7F45D0E77AAC"`

Reads a 4-byte integer value from the address found by pattern in `GTA5_Enhanced.exe`.

- **Parameters:**
  - `name (STRING)`: A unique name for your pattern (used for caching).
  - `pattern (STRING)`: The IDA-style pattern to scan for within `GTA5_Enhanced.exe`.
  - `offset (INT)`: The amount to add/subtract from the found address.
  - `rip (BOOL)`: Whether to rip the found address.

- **Returns:**
  - `INT`: The 4-byte value read from the found address.

**Example Usage:**
```
INT gameTimer = READ_MEMORY("GameTimer", "3B 2D ? ? ? ? 76 ? 89 D9", 2, TRUE)
```

### `NATIVE PROC SET_CURRENT_SCRIPT_THREAD(INT scriptHash) = "0x7AFACDB81809E2C1"`

Spoofs the currently running script thread. Make sure to restore it when you're done with it.

- **Parameters:**
  - `scriptHash (INT)`: The hash of the script to spoof to.

**Example Usage:**
```
INT oldHash = GET_HASH_OF_THIS_SCRIPT_NAME()
SET_CURRENT_SCRIPT_THREAD(HASH("shop_controller"))
// Do your stuff.
SET_CURRENT_SCRIPT_THREAD(oldHash)
```

### `NATIVE PROC WRITE_SCRIPT_STATIC(INT scriptHash, INT index, INT value) = "0xBB3346E0994B7EA5"`

Writes the given value to a script static at given index.

- **Parameters:**
  - `scriptHash (INT)`: The hash of the script.
  - `index (INT)`: The index of the static.
  - `value (INT)`: The value to write.

**Example Usage:**
```
WRITE_SCRIPT_STATIC(HASH("freemode"), 19447 + 176, 0)
```

### `NATIVE FUNC INT READ_SCRIPT_STATIC(INT scriptHash, INT index) = "0x8725A6C8DE144DBC"`

Reads a script static at given index.

- **Parameters:**
  - `scriptHash (INT)`: The hash of the script.
  - `index (INT)`: The index of the static.

- **Returns:**
  - `INT`: The 4-byte value read from the script static.

**Example Usage:**
```
INT eventCooldown = READ_SCRIPT_STATIC(HASH("freemode"), 16019 + (1 + (0 * 12)) + 6)
```