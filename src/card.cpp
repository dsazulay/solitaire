#include "card.h"

#include <string_view>
#include "utils/log.h"

auto printCard(Card& card) -> void
{
    std::string_view suit_name;
    switch (card.suit)
    {
    case 0:
        suit_name = "hearts";
        break;
    case 1:
        suit_name = "spades";
        break;
    case 2:
        suit_name = "diamonds";
        break;
    case 3:
        suit_name = "clubs";
        break;
    }
    LOG_INFO("{} of {} - (x: {}, y: {}, z: {})", card.number + 1,
            suit_name, card.pos.x, card.pos.y, card.pos.z);
}
