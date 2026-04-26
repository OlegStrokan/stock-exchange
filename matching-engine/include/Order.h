#pragma once

#include "Types.h"
#include <string>
#include <list>


namespace engine {
    struct Order {
        OrderId id;
        Side side;
        OrderType type;
        Price price;
        Quantity quantity;
        Quantity remaining;
        Timestamp timestamp;
        std::list<Order*>::iterator bookPosition;

        Order(OrderId id, Side side, OrderType type, Price price, Quantity quantity)
            : id(id)
            , side(side)
            , type(type)
            , price(price)
            , quantity(quantity)
            , remaining(quantity)
            , timestamp(now())
        {}

        bool isFilled() const { return remaining == 0; }

        Quantity fill(Quantity qty) {
            Quantity filled = std::min(qty, remaining);
            remaining -= filled;
            return filled;
        }
    };
}