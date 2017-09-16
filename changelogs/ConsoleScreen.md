# Console Screen Change Log
All notable changes to this project will be documented in this file.

## [2.4.0] - 2017-09-16
### Added
- Ability to get a cell location at a given co-ordinate
- Ability to get (transformed) co-ordinate of a cell location
- New "Number" type to allow numbers to be streamed and printed
- New palette - 8-colour RGB
- Ability to get cell attributes bitmask
- Ability to set all textures values in using a single function
- Ability to set/get clear value (direct value or mapped character)
- Ability to set/get colour multiplier used for dark attribute (see below)

### Changed
- Bright attribute is now dark attribute
- Default value for number of texture tiles per row is now 16
- Re-arranged cycle palette (range) parameters; amount can now be left at its default of one

### FIXED
- Values of char above 127 stored in a string are no longer ignored
- Default foreground/cursor colours are now ColorCommand::Contrast to avoid out-of-range values in smaller palettes
- Removed unnecessary full updates when changing stretch type

## [2.3.2] - 2016-11-14
### Changed
- Altered error message in exception thrown by failed clear/fill

### Fixed
- Clearing colour pair now doesn't skip foreground if background is a command
- Implicit/explicit constructors for Fg and Bg types

## [2.3.1] - 2016-11-09
### Changed
- Buffers are no longer removed when mode is changed

### Fixed
- Stack is now removed when mode is changed

## [2.3.0] - 2016-11-08
### Added
- Ability to position stack cells off-grid

### Changed
- Cursor adjusts to double-height to match stretch type
- Cursor movements now take into account double-printing

### Fixed
- Double-height printing scrolls double automatically
- Some cross-platform compilation bugs

## [2.2.0] - 2016-11-05
### Added
- New "Character" type to allow single characters to be streamed
- Ability to manipulate buffer cells directly
- Ability to add and resize buffers

### Fixed
- Double-height printing with stretch type of both now prints also prints the lower line
- Specified storage type of forward-declared enums

## [2.1.1] - 2016-09-08
### Fixed
- Selection rectangle size information is now correct

## [2.1.0] - 2016-07-21
### Added
- Ability to cycle colours
- Ability to scroll a rectangular region
- Ability to get "perfect" size

### Changed
- Newline, Backspace and Delete cursor commands now take into account current stretch type

### Fixed
- Printing in the bottom-right cell can now trigger an automatic scroll
- Setting a palette colour now causes a screen update

## [2.0.0] - 2016-06-27
### Added
- Mappable cursor commands. Characters in a string can invoke cursor commands
- Stack printing. Extra cells can be layers over and under the main screen
- Entire screen can be filled with a single, specified cell
- Individual cells can now be directly manipulated in place (by reference)
- "Cs" standard alias shortcut for "sw::ConsoleScreen"
- Switchable palette mode (standard/rgb). Standard is default
- If a full colour is used in standard palette mode, either the colour will added to the palette or the closest match in the palette is used
- Ability to get current print properties
- Many simple types to use in the "stream" to specify different information
- Ability to move the print position by an offset
- Buffers can be pasted as stack tiles (see "Stack printing" above)
- HTML 16-colour palette (https://www.w3.org/TR/REC-html40/types.html#h-6.5)
- Palette enums file added to Selba Ward that allows enums to be used to specify colours in the palettes used by Console Screen

### Changed
- Colours are now stored as values (palette index/colour command). Changing the palette changes all instances of that colour
- Most things are "streamed" directly into the Console Screen object using the insertion operator (<<)
- Reading is also "streamed" by using the extraction operator (>>)

### Fixed
- Incorrect "web-safe" palette colour values

## [1.5.0] - 2016-06-27
### Added
- Cursor switches. Cursor no longer must affect the its cell's colours and it can invert its cell's colours
- Ignored cursor value. A cursor value that is negative values will not affect its cell's value
- A mapped character can be used to set the cursor's value

### Changed
- Class name is now ConsoleScreenV1 and the filenames now have been appended with "Old" in preparation for the additional inclusion of version 2.

### Fixed
- Cursor now never stretches regardless of the cell's stretch value.

## [1.4.0] - 2016-03-29
### Added
- Character mapping. Printed characters can now be mapped to any cell value
- Reading. Retrieves a string from consecutive cells. Can use cursor or read directly
- Texture offset. A sub-texture can now be used

### Fixed
- Colour commands now processed for every cell individually in paintAt()

## [1.3.1] - 2016-03-28
### Fixed
- Colour commands now functions correctly when using printStretchedAt() ([#5])

## [1.3.0] - 2016-03-11
### Added
- Double-height printing. Each half can be printed separately or together
- Cell attributes:
    - Inverse swaps colour and background colour
    - Bright displays fully bright colour (darkens when switched off)
    - FlipX and FlipY flip the cell horizontally and vertically respectively

## [1.2.1] - 2016-02-09
### Fixed
- Buffer width now correctly calculated when selection rectangle extends beyond screen limits ([#3])

## [1.2.0] - 2016-02-07
### Added
- Buffers/clipboards (entire screen or rectangular regions)
- Painting: similar to printing but doesn't affect cell values (only alters colours), like highlighting
- Cell information retrieval to allow getting a cell's value, colour, background colour, or entire cell

### Removed
- "Greyscale" from palette enum. Now only "grayscale" can be used

## [1.1.0] - 2016-01-12
### Added
- Colour palettes
- Colour commands
- Customisable cursor
- Text-editing cursor controls

### Changed
- printAt() now prints both string and characters so printCharAt() has been removed
- Nullifying the texture no longer requires (or accepts) a null pointer to be passed as a parameter
- All switch/flag methods have had "enabled" removed from their name
Some renamed slightly for grammatical reasons
- nextline() has been renamed as cursorNextline() to increase naming consistency

## [1.0.0] - 2016-01-02
### Added
- Full version

[2.4.0]: https://github.com/Hapaxia/SelbaWard/commit/c9bfddc95e48aab6fd9d9a60bb872cfa5304851d
[2.3.2]: https://github.com/Hapaxia/SelbaWard/commit/832048d2e87544425c42bbfdc4e837b4c32dbe67
[2.3.1]: https://github.com/Hapaxia/SelbaWard/commit/04d9c8337dbe8522659c3c94e2ffbdfafb42e66f
[2.3.0]: https://github.com/Hapaxia/SelbaWard/commit/28c6a75a2d2604e40ccd1f0b3c05b78cc0af22dc
[2.2.0]: https://github.com/Hapaxia/SelbaWard/commit/289487af9a34aede3f6c2dd1f2697e208c43538f
[2.1.1]: https://github.com/Hapaxia/SelbaWard/commit/fb7a2babede652359015497a2dc494dddde1d824
[2.1.0]: https://github.com/Hapaxia/SelbaWard/commit/d4f771265a3ba6b90b88b59d7b41c8bc835c8db3
[2.0.0]: https://github.com/Hapaxia/SelbaWard/commit/79ab58b379bb0fe540edfab26ec22324a20ca5a6
[1.5.0]: https://github.com/Hapaxia/SelbaWard/commit/9cb2c06b2be8941f418d7eb03c1be893999af063
[1.4.0]: https://github.com/Hapaxia/SelbaWard/commit/3dc18730df9a096d5bbf80d19e7e839357fc985d
[1.3.1]: https://github.com/Hapaxia/SelbaWard/commit/f544eaf2e90d46558dcaeb80c54992def0a18ec8
[1.3.0]: https://github.com/Hapaxia/SelbaWard/commit/661223c925e9c1d57ce11f77462812cd024f9aa9
[1.2.1]: https://github.com/Hapaxia/SelbaWard/commit/2238bc8dfec3580d7da7188bf9a388b5e720ca2e
[1.2.0]: https://github.com/Hapaxia/SelbaWard/commit/37a22dbf625ce1468077c2455266a4b41651952d
[1.1.0]: https://github.com/Hapaxia/SelbaWard/commit/869078f4294e62814c43d63416b5a68af9c5363d
[1.0.0]: https://github.com/Hapaxia/SelbaWard/commit/424ca290165d74de99d00806166dc0b52eb6d5f0

[#3]: https://github.com/Hapaxia/SelbaWard/pull/3
[#5]: https://github.com/Hapaxia/SelbaWard/pull/5
