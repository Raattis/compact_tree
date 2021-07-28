# compact_tree
A linear tree structure with focus on low memory usage.

Instead of storing absolute or relative pointers, only depth values are stored in a separate array from the payload. This enables moving the sub trees around and only the moved depths need to be adjusted.

## structure
The structure is simple and inherently more cache friendly than typical pointer tree structures.
```cpp
struct compact_tree
{
  vector<uint8_t> depths;
  vector<item> items;
};
```

## Program output
```
>root
 +-1
 | +-2
 | | +-3
 | | | +-4
 | | | | +-5
 | | | | | +-6
 | | | | | | +-7
 | | | | | | | +-8
 | | | | | | | | +-9
 | | | | | | | +-10
 | | | | | | +-11
 | | | | | | | +-12
 | | | | | | +-13
 | | | | | +-14
 | | | | | | +-15
 | | | | | | | +-16
 | | | | | | +-17
 | | | | | +-18
 | | | | | | +-19
 | | | | | +-20
 | | | | +-21
 | | | | | +-22
 | | | | | | +-23
 | | | | | | | +-24
 | | | | | | +-25
...

With maximum tree depth set to 2^32
Total memory used by 200 items (104 bytes each) is 21664 bytes.
The overhead is 864 bytes or 4 %.

With maximum tree depth set to 2^16
Total memory used by 200 items (104 bytes each) is 21264 bytes.
The overhead is 464 bytes or 2 %.

With maximum tree depth set to 2^8
Total memory used by 200 items (104 bytes each) is 21064 bytes.
The overhead is 264 bytes or 1 %.

...

Erasing "67" and its 31 children
Tree size was 200, now it's 168.

Erasing "3" and its 63 children
Tree size was 168, now it's 104.
```
