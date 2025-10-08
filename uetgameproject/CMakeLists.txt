find_package (SDL3 REQUIRED)
find_package (SDL3_image REQUIRED)

add_executable (uetgameproject "uetgameproject.cpp"  "tile.cpp" "tile.h")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET uetgameproject PROPERTY CXX_STANDARD 20)
endif()

target_link_libraries (uetgameproject PRIVATE SDL3::SDL3 SDL3_image::SDL3_image)
