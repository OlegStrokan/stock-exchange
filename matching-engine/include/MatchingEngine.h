#pragma once

#include "OrderBook.h"
#include "Order.h"
#include "Trade.h"
#include "ObjectPool.h"

#include <vector>

// @todo: add multiple pools for different order types
// @todo: add utilization check => trigger pagerDuty/slack alert for team
namespace  engine {

    class MatchingEngine {
    public:
        explicit MatchingEngine(size_t poolSize = 2'000'000)
            : orderPool_(poolSize) {}

        std::vector<Trade> submitLimit(OrderId id, Side side, Price price, Quantity qty);

        std::vector<Trade> submitMarket(OrderId id, Side side, Quantity quantity);

        bool cancel(OrderId id);

        const OrderBook& book() const { return book_; }
        OrderBook& book() { return book_; }

        size_t totalTrades() const { return tradeCount_; }
        size_t totalOrders() const { return orderCount_; }

    private:
        OrderBook book_;

        ObjectPool<Order> orderPool_;

        size_t tradeCount_ = 0;
        size_t orderCount_ = 0;
    };
}