# Spline Change Log
All notable changes to this project will be documented in this file.

## [1.3.1] - 2018-11-07
### Fixed
- Member initialisation updated and ordered

## [1.3.0] - 2017-05-02
### Added
- Thickness per vertex - multiplied by global thickness
- Colour per vertex - multiplied by global colour
- Ability to get tangents of interpolated positions
- Ability to get normals of interpolated positions
- Ability to get the interpolated spline thickness at interpolated positions
- Ability to get the thickness correction scale for interpolated positions

## [1.2.0] - 2017-04-29
### Added
- Ability to automatically close the spline
- Ability to specify vertices' position in the initializer list
- Ability to retrieve the actual interpolated positions
- Ability to retrieve the total number of interpolated positions
### Fixed
- Setting the colour now also changes the colour of the thick spline
- Now correctly calculates reserved sizes for the interpolated vertices (both thick and thin)

## [1.1.2] - 2017-04-26
### Fixed
- m_thickness is now explicitly initialised in the constructor

## [1.1.1] - 2017-03-13
### Fixed
- Now only uses deprecated SFML when using older SFML versions (pre v2.4)

## [1.1.0] - 2017-03-04
### Added
- Ability to specify a thickness for the lines
### Changed
- All switch/flag methods have had "enable"/"enabled" removed from their names
### Fixed
- Replaced deprecated SFML enum value

## [1.0.1] - 2016-11-03
### Fixed
- Improved code style consistency

## [1.0.0e] - 2015-12-12
### Added
- Ability to specify initial vertices in the constructor

## [1.0.0d] - 2015-11-17
### Added
- Methods to find the length of the spline

## [1.0.0c] - 2015-11-17
### Added
- Ability to access vertices directly using [] operator

## [1.0.0b] - 2015-11-16
### Added
- Method to automatically adjust handles to form a smooth curve
### Fixed
- Handles are now cleared when all vertices are removed

## [1.0.0a] - 2015-11-14
### Added
- Vertex handles and Bezier interpolation
- Primitive type can now be changed to any type

## [1.0.0] - 2015-11-13
### Added
- Full version

[1.3.1]: https://github.com/Hapaxia/SelbaWard/commit/abb44f6c8208394680670504d3a111201145b37a
[1.3.0]: https://github.com/Hapaxia/SelbaWard/commit/ecc30129e0faca817ffa0a20f5d121f6ea58ae6c
[1.2.0]: https://github.com/Hapaxia/SelbaWard/commit/81800b8348d4f1bb10ffa0613521c2db79a4dfeb
[1.1.2]: https://github.com/Hapaxia/SelbaWard/commit/cc0b4d1c9e2befc7d182db331c6fccc3219f19fe
[1.1.1]: https://github.com/Hapaxia/SelbaWard/commit/24b8fe4a8ea286ac9b4423ddedf884371ea5ee4b
[1.1.0]: https://github.com/Hapaxia/SelbaWard/commit/9d6c37205f511b313ec745e4c5c68f78f76c680b
[1.0.1]: https://github.com/Hapaxia/SelbaWard/commit/9032d0a164447ad192be1d3b759d3c8563607086
[1.0.0e]: https://github.com/Hapaxia/SelbaWard/commit/94037c5625680605fe7a402b8cfc6cabe3a11f5a
[1.0.0d]: https://github.com/Hapaxia/SelbaWard/commit/cdaa8a8a90dc03a6088cfd667231ec3028a941c6
[1.0.0c]: https://github.com/Hapaxia/SelbaWard/commit/4e17b4bb55ef0449d0b537210fef99e04ae3b162
[1.0.0b]: https://github.com/Hapaxia/SelbaWard/commit/f6e3f56c9f57bb1c53f55f7ddd129f5235ca962d
[1.0.0a]: https://github.com/Hapaxia/SelbaWard/commit/1f5bac3d235c462bc90340019040a70f81f455d5
[1.0.0]: https://github.com/Hapaxia/SelbaWard/commit/5589ee0e56f286bcb5f1271919b29ef37413a214
