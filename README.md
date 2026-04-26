# exchange

something drastically different from eventual consistency and idempotent sagas with DLQs for Kafka

## matching engine
- no mutexes to slow down the pump
- no dynamic heap allocation because we don't do dirty bulk
- no architecture in the benchmarks - just the raw power of tits

### We don’t just process orders, we PR them