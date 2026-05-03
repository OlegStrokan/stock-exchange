#include "MatchingEngine.h"
#include "Order.h"
#include <iostream>
#include <chrono>
#include <random>

using namespace engine;

int main() {
    MatchingEngine engine;
    
    std::cout << "=== Matching Engine Stress Test (1 Million Orders) ===\n\n";

    const int NUM_ORDERS = 9'000'000;
    std::mt19937 rng(42);
    std::uniform_int_distribution<Price> priceDist(9900, 10100);
    std::uniform_int_distribution<Quantity> qtyDist(1, 100);
    std::uniform_int_distribution<int> sideDist(0, 1);

    auto start = std::chrono::high_resolution_clock::now();

    std::cout << "Submitting " << NUM_ORDERS << " orders...\n";
    for (int i = 0; i < NUM_ORDERS; ++i) {
        Side side = sideDist(rng) == 0 ? Side::Buy : Side::Sell;
        engine.submitLimit(static_cast<OrderId>(i), side, priceDist(rng), qtyDist(rng));
        
        if ((i + 1) % 100'000 == 0) {
            std::cout << "  Processed: " << (i + 1) << " orders\n";
        }
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto durationMs = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    double ordersPerSec = (static_cast<double>(NUM_ORDERS) / durationMs) * 1000;

    std::cout << "\n=== Results ===\n";
    std::cout << "Total orders processed: " << engine.totalOrders() << "\n";
    std::cout << "Total trades executed: " << engine.totalTrades() << "\n";
    std::cout << "Time elapsed: " << durationMs << " ms\n";
    std::cout << "Throughput: " << static_cast<int>(ordersPerSec) << " orders/sec\n";
    std::cout << "Book bid levels: " << engine.book().bidLevelCount() << ", ask levels: " << engine.book().askLevelCount() << "\n";

    return 0; 
}