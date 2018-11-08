# Nine Patch Change Log
All notable changes to this project will be documented in this file.

## [1.4.2] - 2018-11-07
### Fixed
- Member initialisation updated and ordered

## [1.4.1] - 2017-10-31
### Changed
- Setting texture rect can now reset size to match (set by default to match setting texture)
### Fixed
- Removed excess image sub-copy when processing ranges
- Texture can now be reset to null

## [1.4.0] - 2017-05-02
### Added
- Ability to use a rectangle to specify the part of the texture to be used
- Ability to get local content area (area before transformations)
- Ability to test if a point is inside the actual global content area (not-axis aligned)
CHANGED:
- Renamed getContentArea to getGlobalContentArea (area after transformations)
- Manually resetting size matches the texture rectangle size not the entire texture

## [1.3.0] - 2016-11-13
### Added
- Ability to change and retrieve overall colour

## [1.2.0] - 2016-09-08
### Added
- Ability to manually reset size to match the texture
### Fixed
- Content area size

## [1.1.0] - 2016-08-05
### Added
- Ability to retrieve a float rect containing local or global bounds
- Ability to retrieve a float rect containing the content area
- Ability to set texture without resetting the size
### Fixed
- Ability to transform - now inherits from transformable

## [1.0.0] - 2015-12-19
### Added
- Full version

[1.4.2]: https://github.com/Hapaxia/SelbaWard/commit/abb44f6c8208394680670504d3a111201145b37a
[1.4.1]: https://github.com/Hapaxia/SelbaWard/commit/46c76bb496703187c04d61450506758191d8bc11
[1.4.0]: https://github.com/Hapaxia/SelbaWard/commit/3c6a84f82c43a58a00495f20fc58552b175431f8
[1.3.0]: https://github.com/Hapaxia/SelbaWard/commit/788f3d2991894342fd39fde558add492494adde5
[1.2.0]: https://github.com/Hapaxia/SelbaWard/commit/d12f5274d91b63036c02ab8f50fbb76e319ef521
[1.1.0]: https://github.com/Hapaxia/SelbaWard/commit/ac3a1dfbbffaea5ab3f545ba754e307c85e3ba9d
[1.0.0]: https://github.com/Hapaxia/SelbaWard/commit/202b592ce40f09fba106abebff8196cf2357bdba
