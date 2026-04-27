# exchange

something drastically different from eventual consistency and idempotent sagas with DLQs for Kafka

## matching engine
- no mutexes to slow down the pump
- no dynamic heap allocation because we don't do dirty bulk
- no architecture in the benchmarks - just the raw power of tits

## high level implementation:

- object pool: pre-allocated memory pool that avoids heap allocation on the hot path, free-list (vector used as s stack) tracks available slots - no new/delete during order processing
- order: order data + bookPosition iterator ( O(1) ) removal from the price level's linked list
- priceLevel: groups resting order at a single price. O(1)
- orderBook: core data structure with two sorted maps: bids (descending), asks (ascending), orderLookup - for O(1) cancel lookups

### We don’t just process orders, we PR them