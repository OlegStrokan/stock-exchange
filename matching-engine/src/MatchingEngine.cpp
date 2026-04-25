#include "../include/MatchingEngine.h"


namespace engine {
    std::vector<Trade> MatchingEngine::submitLimit(OrderId id, Side side, Price price, Quantity quantity) {
        // acquire from pool - no heap alloc, just grab pre-allocated slot
        Order* order = orderPool_.acquire(id, side, OrderType::Limit, price, quantity);

        orderCount_++;

        auto result = book().match(*order);
        tradeCount_ += result.trades.size();

        for (Order* filled : result.filledOrders) {
            orderPool_.release(filled);
        }

        // if order has remaining quantity, add to the book
        if (!order->isFilled()) {
            book_.addOrder(order);
        } else {
            // fully filled
            orderPool_.release(order);
        }

        return result.trades;
    }

    std::vector<Trade> MatchingEngine::submitMarket(OrderId id, Side side, Quantity quantity) {
        Order* order = orderPool_.acquire(id, side, OrderType::Market, 0, quantity);

        orderCount_++;

        // market order should match
        auto result = book().match(*order);
        tradeCount_ += result.trades.size();

        for (Order* filled : result.filledOrders) {
            orderPool_.release(filled);
        }

        orderPool_.release(order);

        return result.trades;
    }

    bool MatchingEngine::cancel(OrderId id) {
        return book_.cancelOrder(id);
    }
}
