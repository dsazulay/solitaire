#include "freecell_input.h"

#include "../utils/log.h"

auto FreecellInputHandler::init(FreecellBoardManager* manager) -> void
{
    boardManager = manager;
}

auto FreecellInputHandler::restart() -> void
{
    boardManager->emptyTable();
    boardManager->fillTable();
    boardManager->turnCardsUp();
    boardManager->updateCardList();
}

auto FreecellInputHandler::newGame() -> void
{
    boardManager->emptyTable();
    boardManager->shuffleDeck();
    boardManager->fillTable();
    boardManager->turnCardsUp();
    boardManager->updateCardList();
}

//auto FreecellInputHandler::pause() -> void
//{

//}

