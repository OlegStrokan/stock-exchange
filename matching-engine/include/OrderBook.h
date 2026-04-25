#pragma once

#include "Types.h"
#include "Order.h"
#include "Trade.h"

#include <map>
#include <list>
#include <unordered_map>
#include <vector>
#include <optional>

namespace engine {


    struct MatchResult {
        std::vector<Trade> trades;
        std::vector<Order*> filledOrders;
    };

    struct PriceLevel {
        Price price;
        std::list<Order*> orders;
        Quantity totalQuantity = 0;

        explicit PriceLevel(Price p) : price(p) {}

        void addOrder(Order* order) {
            orders.push_back(order);
            order->bookPosition = std::prev(orders.end());
            totalQuantity += order->remaining;
        }

        void removeOrder(Order* order) {
            totalQuantity -= order->remaining;
            orders.erase(order->bookPosition);
        }

        bool empty() const { return orders.empty(); }
    };




    class OrderBook {
    public:
        OrderBook() = default;

        void addOrder(Order* order);

        bool cancelOrder(OrderId id);

        MatchResult match(Order& incomingOrder);

        std::optional<Price> bestBid() const;
        std::optional<Price> bestAsk() const;
        std::optional<Price> spread() const;

        size_t bidLevelCount() const { return bids_.size(); }
        size_t askLevelCount() const { return asks_.size(); }

        void printBook(int depth = 5) const;

    private:
        std::Map<Price, PriceLevel, std::greater<Price>> bids_;

        std::map<Price, PriceLevel> asks_;

        std::unordered_map<OrderId, Order*> orderLookup_;

        MatchResult matchBuy(Order& order);
        MatchResult matchSell(Order& order);
        void addToAsks(Order* order);
        void askToBids(Order* order);
    };

}