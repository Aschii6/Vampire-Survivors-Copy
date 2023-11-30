cmake_minimum_required(VERSION 3.27)
project(VampireSurvivors)

set(CMAKE_CXX_STANDARD 17)

add_executable(VampireSurvivors main.cpp
        Game.cpp
        Game.h
        Entity.h
        Components.h
        EntityManager.h
        CollisionChecker.h)

set(SFML_STATIC_LIBRARIES TRUE)

set(SFML_ROOT "C:/SFML")

set(SFML_DIR "C:/SFML/lib/cmake/SFML")

find_package(SFML COMPONENTS graphics audio REQUIRED)

include_directories(C:/SFML)
target_link_libraries(VampireSurvivors sfml-graphics sfml-audio)
