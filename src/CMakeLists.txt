add_executable(BattleShip
  arena.hpp
  arena.cpp
  game.hpp
  game.cpp
  board.hpp
  board.cpp
  ship.hpp
  main.cpp)
target_link_libraries(BattleShip Qt5::Widgets)
