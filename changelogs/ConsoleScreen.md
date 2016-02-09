# Console Screen Change Log
All notable changes to this project will be documented in this file.

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

[1.2.1]: https://github.com/Hapaxia/SelbaWard/commit/2238bc8dfec3580d7da7188bf9a388b5e720ca2e
[1.2.0]: https://github.com/Hapaxia/SelbaWard/commit/37a22dbf625ce1468077c2455266a4b41651952d
[1.1.0]: https://github.com/Hapaxia/SelbaWard/commit/869078f4294e62814c43d63416b5a68af9c5363d
[1.0.0]: https://github.com/Hapaxia/SelbaWard/commit/424ca290165d74de99d00806166dc0b52eb6d5f0

[#3]: https://github.com/Hapaxia/SelbaWard/pull/3
