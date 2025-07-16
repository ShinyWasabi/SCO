### `NATIVE PROC WRITE_MEMORY(STRING name, STRING pattern, INT offset, BOOL rip, INT& patch[], BOOL protect) = "0xEEE74A05DE4C2A07"`

Writes the bytes in the patch array to the address found by pattern in `GTA5_Enhanced.exe`.

- **Parameters:**
  - `name (STRING)`: A Unique name for your pattern (used for caching).
  - `pattern (STRING)`: The IDA-style pattern to scan for within `GTA5_Enhanced.exe`.
  - `offset (INT)`: The amount to add/subtract from the found address.
  - `rip (BOOL)`: Whether to rip the found address.
  - `patch (INT[])`: The bytes to be written to the found address. Each item in the array must be within the range of 0-255.
  - `protect (BOOL)`: Whether to change the memory protections. Use this when writing to the `.text` section.

**Example Usage:**
```
INT patch[1]
patch[0] = 235 // 0xEB
WRITE_MEMORY("ModelSpawnBypass", "48 8B 06 48 89 F1 FF 50 ? 84 C0 75 ? 31 F6 48 89 F0", 11, FALSE, patch, TRUE)
```

### `NATIVE FUNC INT READ_MEMORY(STRING name, STRING pattern, INT offset, BOOL rip) = "0x1E9F7F45D0E77AAC"`

Reads a 4-byte integer value from the address found by pattern in `GTA5_Enhanced.exe`.

- **Parameters:**
  - `name (STRING)`: Unique name of your pattern (used for caching).
  - `pattern (STRING)`: The IDA-style pattern to scan for within `GTA5_Enhanced.exe`.
  - `offset (INT)`: The amount to add/subtract from the found address.
  - `rip (BOOL)`: Whether to rip the found address.

- **Returns:**
  - `INT`: The 4-byte value read from the found address.

**Example Usage:**
```
INT gameTimer = READ_MEMORY("GameTimer", "3B 2D ? ? ? ? 76 ? 89 D9", 2, TRUE)
```