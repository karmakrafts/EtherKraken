// Copyright 2026 Karma Krafts
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <atomic>
#include <gtest/gtest.h>
#include <numeric>
#include <thread>
#include <vector>

#include "util/kraken_atomic_queue.h"

struct TestStruct {
    uint64_t value;
};

GTEST_TEST(ms_queue, create_and_destroy) {
    kraken_ms_queue_t queue {};
    ASSERT_EQ(KRAKEN_OK, kraken_ms_queue_create(sizeof(TestStruct), &queue));
    ASSERT_EQ(sizeof(TestStruct), queue.element_size);
    ASSERT_EQ(KRAKEN_OK, kraken_ms_queue_destroy(&queue));
}

GTEST_TEST(ms_queue, alloc_and_free) {
    kraken_ms_queue_t* queue = nullptr;
    ASSERT_EQ(KRAKEN_OK, kraken_ms_queue_alloc(sizeof(TestStruct), &queue));
    ASSERT_NE(nullptr, queue);
    ASSERT_EQ(KRAKEN_OK, kraken_ms_queue_free(queue));
}

GTEST_TEST(ms_queue, is_empty) {
    kraken_ms_queue_t queue {};
    ASSERT_EQ(KRAKEN_OK, kraken_ms_queue_create(sizeof(TestStruct), &queue));

    bool empty = false;
    ASSERT_EQ(KRAKEN_OK, kraken_ms_queue_is_empty(&queue, &empty));
    ASSERT_TRUE(empty);

    constexpr TestStruct value {.value = 1337};
    ASSERT_EQ(KRAKEN_OK, kraken_ms_queue_enqueue(&queue, &value));

    ASSERT_EQ(KRAKEN_OK, kraken_ms_queue_is_empty(&queue, &empty));
    ASSERT_FALSE(empty);

    TestStruct peek_value {};
    ASSERT_EQ(KRAKEN_OK, kraken_ms_queue_peek(&queue, &peek_value));
    ASSERT_EQ(1337, peek_value.value);

    ASSERT_EQ(KRAKEN_OK, kraken_ms_queue_is_empty(&queue, &empty));
    ASSERT_FALSE(empty);

    TestStruct read_value {};
    ASSERT_EQ(KRAKEN_OK, kraken_ms_queue_dequeue(&queue, &read_value));

    ASSERT_EQ(KRAKEN_OK, kraken_ms_queue_is_empty(&queue, &empty));
    ASSERT_TRUE(empty);

    ASSERT_EQ(KRAKEN_OK, kraken_ms_queue_destroy(&queue));
}

GTEST_TEST(ms_queue, peek_empty) {
    kraken_ms_queue_t queue {};
    ASSERT_EQ(KRAKEN_OK, kraken_ms_queue_create(sizeof(TestStruct), &queue));

    TestStruct read_value {};
    ASSERT_EQ(KRAKEN_ERR_INVALID_OP, kraken_ms_queue_peek(&queue, &read_value));

    ASSERT_EQ(KRAKEN_OK, kraken_ms_queue_destroy(&queue));
}

GTEST_TEST(ms_queue, dequeue_empty) {
    kraken_ms_queue_t queue {};
    ASSERT_EQ(KRAKEN_OK, kraken_ms_queue_create(sizeof(TestStruct), &queue));

    TestStruct read_value {};
    ASSERT_EQ(KRAKEN_ERR_INVALID_OP, kraken_ms_queue_dequeue(&queue, &read_value));

    ASSERT_EQ(KRAKEN_OK, kraken_ms_queue_destroy(&queue));
}

GTEST_TEST(ms_queue, fifo_order) {
    kraken_ms_queue_t queue {};
    ASSERT_EQ(KRAKEN_OK, kraken_ms_queue_create(sizeof(TestStruct), &queue));

    for(uint64_t i = 0; i < 100; ++i) {
        TestStruct val {.value = i};
        ASSERT_EQ(KRAKEN_OK, kraken_ms_queue_enqueue(&queue, &val));
    }

    for(uint64_t i = 0; i < 100; ++i) {
        TestStruct read_val {};
        ASSERT_EQ(KRAKEN_OK, kraken_ms_queue_dequeue(&queue, &read_val));
        ASSERT_EQ(i, read_val.value);
    }

    ASSERT_EQ(KRAKEN_OK, kraken_ms_queue_destroy(&queue));
}

GTEST_TEST(ms_queue, hazard_pointer_retirement) {
    kraken_ms_queue_t queue {};
    ASSERT_EQ(KRAKEN_OK, kraken_ms_queue_create(sizeof(uint64_t), &queue));

    // The implementation has RETIRE_THRESHOLD = 16.
    // Performing many operations should trigger retirement and reclamation.
    constexpr int num_ops = 1000;
    for(int i = 0; i < num_ops; ++i) {
        uint64_t val = i;
        ASSERT_EQ(KRAKEN_OK, kraken_ms_queue_enqueue(&queue, &val));
        uint64_t out_val;
        ASSERT_EQ(KRAKEN_OK, kraken_ms_queue_dequeue(&queue, &out_val));
        ASSERT_EQ(static_cast<uint64_t>(i), out_val);
    }

    ASSERT_EQ(KRAKEN_OK, kraken_ms_queue_destroy(&queue));
}

GTEST_TEST(ms_queue, stress_concurrent) {
    kraken_ms_queue_t queue {};
    ASSERT_EQ(KRAKEN_OK, kraken_ms_queue_create(sizeof(uint64_t), &queue));

    constexpr int num_producers = 4;
    constexpr int num_consumers = 4;
    constexpr int ops_per_producer = 10000;
    std::atomic producers_done(0);
    std::atomic<uint64_t> total_dequeued(0);
    std::atomic<uint64_t> sum_dequeued(0);

    std::vector<std::thread> producers;
    producers.reserve(num_producers);
    for(int i = 0; i < num_producers; ++i) {
        producers.emplace_back([&queue, i, &producers_done] {
            for(int j = 0; j < ops_per_producer; ++j) {
                uint64_t val = static_cast<uint64_t>(i) * ops_per_producer + j;
                while(kraken_ms_queue_enqueue(&queue, &val) != KRAKEN_OK) {
                    std::this_thread::yield();
                }
            }
            producers_done.fetch_add(1);
        });
    }

    std::vector<std::thread> consumers;
    consumers.reserve(num_consumers);
    for(int i = 0; i < num_consumers; ++i) {
        consumers.emplace_back([&queue, &producers_done, &total_dequeued, &sum_dequeued] {
            while(true) {
                uint64_t val;
                if(kraken_ms_queue_dequeue(&queue, &val) == KRAKEN_OK) {
                    total_dequeued.fetch_add(1);
                    sum_dequeued.fetch_add(val);
                }
                else if(producers_done.load() == num_producers) {
                    // Check if it's actually empty
                    bool empty;
                    kraken_ms_queue_is_empty(&queue, &empty);
                    if(empty)
                        break;
                }
                else {
                    std::this_thread::yield();
                }
            }
        });
    }

    for(auto& t : producers)
        t.join();
    for(auto& t : consumers)
        t.join();

    constexpr uint64_t expected_count = static_cast<uint64_t>(num_producers) * ops_per_producer;
    uint64_t expected_sum = 0;
    for(uint64_t i = 0; i < expected_count; ++i)
        expected_sum += i;

    ASSERT_EQ(expected_count, total_dequeued.load());
    ASSERT_EQ(expected_sum, sum_dequeued.load());

    ASSERT_EQ(KRAKEN_OK, kraken_ms_queue_destroy(&queue));
}