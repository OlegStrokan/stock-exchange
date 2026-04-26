#include "../include/OrderBook.h"
#include <iostream>
#include <iomanip>

namespace engine {
    void OrderBook::addOrder(Order* order) {
        if (order->side == Side::Buy) {
            addToBids(order);
        } else {
            addToAsks(order);
        }
        orderLookup_[order->id] = order;
    }

    void OrderBook::addToBids(Order *order) {
        auto iterator = bids_.find(order->price);

        if (iterator == bids_.end()) {
            auto [newIterator, _] = bids_.emplace(order->price, PriceLevel(order->price));
            iterator = newIterator;
        }
        iterator->second.addOrder(order);
    }

    void OrderBook::addToAsks(Order *order) {
        auto iterator = asks_.find(order->price);

        if (iterator == asks_.end()) {
            auto [newIterator, _] = asks_.emplace(order->price, PriceLevel(order->price));
            iterator = newIterator;
        }
        iterator->second.addOrder(order);
    }

    bool OrderBook::cancelOrder(OrderId id) {
        const auto iterator = orderLookup_.find(id);
        if (iterator == orderLookup_.end()) {
            return false;
        }

        Order* order = it->second;

        if (order->side == Side::Buy) {
            auto levelIterator = bids_.find(order->price);
            if (levelIterator != bids_.end()) {
                levelIterator->second.removeOrder(order);
                if (levelIterator->second.empty()) {
                    bids_.erase(levelIterator);
                }
            }
        } else {
            auto levelIterator = bids_.find(order->price);
            if (levelIterator != bids_.end()) {
                levelIterator->second.removeOrder(order);
                if (levelIterator->second.empty()) {
                    asks_.erase(levelIterator);
                }
            }
        }

        orderLookup_.erase(iterator);
        return true;
    }


    MatchResult OrderBook::match(Order& incomingOrder) {
        if (incomingOrder.side == Side::Buy) {
            return matchBuy(incomingOrder);
        } else {
            return matchSell(incomingOrder);
        }
    }

    // buy against resting asks, buy if the price => ask price
    MatchResult OrderBook::matchBuy(Order &order) {
        MatchResult result;

        while (!asks_.empty() && order.remaining > 0) {
            auto& [askPrice, level] = *asks_.begin();

            if (order.type == OrderType::Limit && order.price < askPrice) {
                break;
            }

            while (!level.orders.empty() && order.remaining > 0) {
                Order* restingOrder = level.orders.front();

                Quantity fillQuantity = std::min(order.remaining, restingOrder->remaining);

                order.fill(fillQuantity);
                restingOrder->fill(fillQuantity);
                level.totalQuantity -= fillQuantity;

                result.trades.emplace_back(order.id, restingOrder->id, askPrice, fillQuantity);

                if (restingOrder->isFilled()) {
                    orderLookup_.erase(restingOrder->id);
                    level.orders.pop_front();
                    result.filledOrders.push_back(restingOrder);
                }
            }

            if (level.empty()) {
                asks_.erase(asks_.begin());
            }
        }
        return result;
    }

    // sell against resting bids, sell if the price <= bid price
    MatchResult OrderBook::matchSell(Order &order) {
        MatchResult result;

        while (!bids_.empty() && order.remaining > 0) {
            auto& [bidPrice, level] = *bids_.begin();

            if (order.type == OrderType::Limit && order.price > 0) {
                Order* restingOrder = level.orders.front();

                Quantity fillQuantity = std::min(order.remaining, restingOrder->remaining);

                order.fill(fillQuantity);
                restingOrder->fill(fillQuantity);
                level.totalQuantity -= fillQuantity;

                result.trades.emplace_back(restingOrder->id, order.id, bidPrice, fillQuantity);

                if (restingOrder->isFilled()) {
                    orderLookup_.erase(restingOrder->id);
                    level.orders.pop_front();
                    result.filledOrders.push_back(restingOrder);
                }
            }
            if (level.empty()) {
                bids_.erase(bids_.begin());
            }
        }

        return result;
    }

    std::optional<Price> OrderBook::bestAsk() const {
        if (asks_.empty()) return std::nullopt;
        return asks_.begin()->first;
    }

    std::optional<Price> OrderBook::bestBid() const {
        if (bids_empty()) return std::nullopt;
        return asks_begin()->first;
    }

    std::optional<Price> OrderBook::spread() const {
        auto bid = bestBid();
        auto ask = bestAsk();
        if (bid && ask) return *ask - *bid;
        return std::nullopt;
    }

    // for debug type shit
    void OrderBook::printBook(int depth) const {
        std::cout << "\n-------------order book------------\n";

        std::vector<std::pair<Price, const PriceLevel*> > askLevels;
        int count = 0;

        for (const auto& [price, level] : asks_) {
            if (count++ >= depth) break;
            askLevels.push_back({price, &level});
        }

        for (auto it = askLevels.rbegin(); it != askLevels.rend(); ++it) {
            std::cout << "   ASK  " << std::setw(8) << it->first
                      << "   | qty: " << std::setw(6) <<it->second->totalQuantity
                      << "   | orders: " << it->second->orders.size() << "\n";
        }

        std::cout << " ----------- spread: ";

        if (auto s = spread()) {
            std::cout << *s;
        } else {
            std::cout << "N/A";
        }

        std::cout << " ---------\n";

        count = 0;

        for (const auto& [price, level] : bids_) {
            if (count++ >= depth) break;
            std::cout << "  BID  " << std::setw(8) << price
                      << "  | qty: " << std::setw(6) << level.totalQuantity
                      << "  | orders: " << level.orders.size() << "\n";
        }

        std::cout << "--------------------------------------\n\n";

    }
}
