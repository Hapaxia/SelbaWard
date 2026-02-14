cmake_minimum_required(VERSION 3.28)
project(SelbaWard LANGUAGES CXX)

find_package(SFML 3 REQUIRED COMPONENTS Graphics)

add_library(SelbaWard
    src/SelbaWard/BitmapFont.cpp
    src/SelbaWard/BitmapText.cpp
    src/SelbaWard/ConsoleScreen.cpp
    src/SelbaWard/Crosshair.cpp
    src/SelbaWard/ElasticSprite.cpp
    src/SelbaWard/FrameTransition.cpp
    src/SelbaWard/GallerySprite.cpp
    src/SelbaWard/Line.cpp
    src/SelbaWard/NinePatch.cpp
    src/SelbaWard/PieChart.cpp
    src/SelbaWard/PixelDisplay.cpp
    src/SelbaWard/Polygon.cpp
    src/SelbaWard/ProgressBar.cpp
    src/SelbaWard/Ring.cpp
    src/SelbaWard/SpinningCard.cpp
    src/SelbaWard/Spline.cpp
    src/SelbaWard/Sprite3d.cpp
    src/SelbaWard/SpriteBatch.cpp
    src/SelbaWard/Starfield.cpp
    src/SelbaWard/Starfield3d.cpp
)
add_library(SelbaWard::SelbaWard ALIAS SelbaWard)
target_include_directories(SelbaWard PUBLIC src)
target_link_libraries(SelbaWard PUBLIC SFML::Graphics)
target_compile_features(SelbaWard PUBLIC cxx_std_17)

# Stop configuration if being consumed by a higher level project
if(NOT PROJECT_IS_TOP_LEVEL)
    return()
endif()

option(BUILD_EXAMPLES "Build examples" OFF)
if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()
