add_executable(BattleShip
  arena.hpp
  arena.cpp
  board.hpp
  board.cpp
  game.hpp
  game.cpp
  game_selection.hpp
  game_selection.cpp
  rules.hpp
  rules.cpp
  ship.hpp
  main.cpp)
target_link_libraries(BattleShip Qt5::Widgets)
