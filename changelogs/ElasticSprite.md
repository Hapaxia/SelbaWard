# Elastic Sprite Change Log
All notable changes to this project will be documented in this file.

## [1.3.0] - 2021-11-14
### Added
- Ability to assign a flip to the texture

## [1.2.1] - 2019-12-03
### Fixed
- Bi-linear interpolation activated getter now correct
- Shaders now have GLSL version number (v1.10)
- Replaced switch command in shader with if commands to comply with v1.10
- Explicitly converted int to float before float calculation (in shader)

## [1.2.0] - 2019-03-24
### Added
- Ability to use normal vertex colours with perspective interpolation
### Fixed
- Perspective interpolation offset errors when using very large textures

## [1.1.1] - 2018-11-07
### Fixed
- Member initialisation updated and ordered

## [1.1.0] - 2017-08-07
### Added
- Ability to use perspective interpolation for quad distortion

## [1.0.2] - 2017-06-05
### Fixed
- Transform now combined with the render states' transform

## [1.0.1] - 2017-04-01
### Fixed
- Constructing from a texture now correctly sets the texture rectangle to match that texture

## [1.0.0] - 2017-03-29
### Added
- Full version

[1.3.0]: https://github.com/Hapaxia/SelbaWard/commit/792f0bccdaa2ea9da7c2fba5b633554754be8607
[1.2.1]: https://github.com/Hapaxia/SelbaWard/commit/f901693a6a0cdc4b747b7272a0c162ea4739c23c
[1.2.0]: https://github.com/Hapaxia/SelbaWard/commit/110c0a1c135dc04861e95a89650f279d5612003e
[1.1.1]: https://github.com/Hapaxia/SelbaWard/commit/abb44f6c8208394680670504d3a111201145b37a
[1.1.0]: https://github.com/Hapaxia/SelbaWard/commit/3387aa59e3653d2f754f80cb652e6d869c1d6072
[1.0.2]: https://github.com/Hapaxia/SelbaWard/commit/fb46f5cd0b0faa9f40b2da17ff810717dfababd4
[1.0.1]: https://github.com/Hapaxia/SelbaWard/commit/18314d9272d05a1f1d3ac4da275d39bc45a9fe01
[1.0.0]: https://github.com/Hapaxia/SelbaWard/commit/f53fd9fed57dbf2f663d13325da37e35b8fd3ca4
