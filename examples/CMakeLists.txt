add_executable(bitmap-text bitmapTextExample.cpp)
target_link_libraries(bitmap-text PRIVATE SelbaWard::SelbaWard)

add_executable(line lineExample.cpp)
target_link_libraries(line PRIVATE SelbaWard::SelbaWard)

add_executable(progress-bar progressBarExample.cpp)
target_link_libraries(progress-bar PRIVATE SelbaWard::SelbaWard)

add_executable(spinning-card spinningCardExample.cpp)
target_link_libraries(spinning-card PRIVATE SelbaWard::SelbaWard)

add_executable(spline splineExample.cpp)
target_link_libraries(spline PRIVATE SelbaWard::SelbaWard)

add_executable(spline2 splineExample2.cpp)
target_link_libraries(spline2 PRIVATE SelbaWard::SelbaWard)

add_executable(sprite3d sprite3dExample.cpp)
target_link_libraries(sprite3d PRIVATE SelbaWard::SelbaWard)

add_executable(sprite3d-card-fan sprite3dExampleCardFan.cpp)
target_link_libraries(sprite3d-card-fan PRIVATE SelbaWard::SelbaWard)

add_executable(sprite3d-spinning-card sprite3dExampleSpinningCard.cpp)
target_link_libraries(sprite3d-spinning-card PRIVATE SelbaWard::SelbaWard)
