#pragma once

#include "freecell_boardmanager.h"

class FreecellInputHandler
{
public:
    auto init(FreecellBoardManager* manager) -> void;

    auto restart() -> void;
    auto newGame() -> void;
private:
    FreecellBoardManager* boardManager;
};

