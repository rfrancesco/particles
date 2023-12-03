cmake_minimum_required(VERSION 3.0.0)
project(Particles VERSION 0.1.0 LANGUAGES C CXX)

include(CTest)
enable_testing()

find_package(SDL2 REQUIRED)
include_directories(include ${SDL2_INCLUDE_DIRS})

add_subdirectory(src)
set (EXTRA_LIBS ${EXTRA_LIBS} Game)

add_executable(Particles main.cpp)
target_link_libraries(Particles ${SDL2_LIBRARIES} ${EXTRA_LIBS})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
