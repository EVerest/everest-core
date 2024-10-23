#include <gtest/gtest.h>

#include <ringbuffer.h>

TEST(RingbufferTest, test_new_simple) {
  ringbuffer_t* rb = ringbuffer_init(4096);
  ASSERT_TRUE(rb != nullptr);
  EXPECT_EQ((size_t)4096, ringbuffer_available(rb));
  EXPECT_EQ((size_t)0, ringbuffer_size(rb));
  ringbuffer_free(rb);
}

TEST(RingbufferTest, test_insert_basic) {
  ringbuffer_t* rb = ringbuffer_init(16);

  uint8_t buffer[10] = {0x01, 0x02, 0x03, 0x04, 0x05,
                        0x06, 0x07, 0x08, 0x09, 0x0A};
  ringbuffer_insert(rb, buffer, 10);
  EXPECT_EQ((size_t)10, ringbuffer_size(rb));
  EXPECT_EQ((size_t)6, ringbuffer_available(rb));

  uint8_t peek[10] = {0};
  size_t peeked = ringbuffer_peek(rb, 0, peek, 10);
  EXPECT_EQ((size_t)10, ringbuffer_size(rb));  // Ensure size doesn't change
  EXPECT_EQ((size_t)6, ringbuffer_available(rb));
  EXPECT_EQ((size_t)10, peeked);
  ASSERT_TRUE(0 == memcmp(buffer, peek, peeked));

  ringbuffer_free(rb);
}

TEST(RingbufferTest, test_insert_full) {
  ringbuffer_t* rb = ringbuffer_init(5);

  uint8_t aa[] = {0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA};
  uint8_t bb[] = {0xBB, 0xBB, 0xBB, 0xBB, 0xBB};
  uint8_t peek[5] = {0};

  size_t added = ringbuffer_insert(rb, aa, 7);
  EXPECT_EQ((size_t)5, added);
  EXPECT_EQ((size_t)0, ringbuffer_available(rb));
  EXPECT_EQ((size_t)5, ringbuffer_size(rb));

  added = ringbuffer_insert(rb, bb, 5);
  EXPECT_EQ((size_t)0, added);
  EXPECT_EQ((size_t)0, ringbuffer_available(rb));
  EXPECT_EQ((size_t)5, ringbuffer_size(rb));

  size_t peeked = ringbuffer_peek(rb, 0, peek, 5);
  EXPECT_EQ((size_t)5, peeked);
  EXPECT_EQ((size_t)0, ringbuffer_available(rb));
  EXPECT_EQ((size_t)5, ringbuffer_size(rb));

  ASSERT_TRUE(0 == memcmp(aa, peek, peeked));

  ringbuffer_free(rb);
}

TEST(RingbufferTest, test_multi_insert_delete) {
  ringbuffer_t* rb = ringbuffer_init(16);
  EXPECT_EQ((size_t)16, ringbuffer_available(rb));
  EXPECT_EQ((size_t)0, ringbuffer_size(rb));

  // Insert some bytes

  uint8_t aa[] = {0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA};
  size_t added = ringbuffer_insert(rb, aa, sizeof(aa));
  EXPECT_EQ((size_t)8, added);
  EXPECT_EQ((size_t)8, ringbuffer_available(rb));
  EXPECT_EQ((size_t)8, ringbuffer_size(rb));

  uint8_t bb[] = {0xBB, 0xBB, 0xBB, 0xBB, 0xBB};
  ringbuffer_insert(rb, bb, sizeof(bb));
  EXPECT_EQ((size_t)3, ringbuffer_available(rb));
  EXPECT_EQ((size_t)13, ringbuffer_size(rb));

  uint8_t content[] = {0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
                       0xAA, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB};
  uint8_t peek[16] = {0};
  size_t peeked = ringbuffer_peek(rb, 0, peek, 16);
  EXPECT_EQ((size_t)13, peeked);
  ASSERT_TRUE(0 == memcmp(content, peek, peeked));

  // Delete some bytes

  ringbuffer_delete(rb, sizeof(aa));
  EXPECT_EQ((size_t)11, ringbuffer_available(rb));
  EXPECT_EQ((size_t)5, ringbuffer_size(rb));

  // Add some more to wrap buffer

  uint8_t cc[] = {0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC};
  ringbuffer_insert(rb, cc, sizeof(cc));
  EXPECT_EQ((size_t)2, ringbuffer_available(rb));
  EXPECT_EQ((size_t)14, ringbuffer_size(rb));

  uint8_t content2[] = {0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xCC, 0xCC};
  peeked = ringbuffer_peek(rb, 0, peek, 7);
  EXPECT_EQ((size_t)7, peeked);
  ASSERT_TRUE(0 == memcmp(content2, peek, peeked));

  // Pop buffer

  memset(peek, 0, 16);
  size_t popped = ringbuffer_pop(rb, peek, 7);
  EXPECT_EQ((size_t)7, popped);
  EXPECT_EQ((size_t)9, ringbuffer_available(rb));
  ASSERT_TRUE(0 == memcmp(content2, peek, peeked));

  // Add more again to check head motion

  uint8_t dd[] = {0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD};
  added = ringbuffer_insert(rb, dd, sizeof(dd));
  EXPECT_EQ((size_t)8, added);
  EXPECT_EQ((size_t)1, ringbuffer_available(rb));

  // Delete everything

  ringbuffer_delete(rb, 16);
  EXPECT_EQ((size_t)16, ringbuffer_available(rb));
  EXPECT_EQ((size_t)0, ringbuffer_size(rb));

  // Add small token

  uint8_t ae[] = {0xAE, 0xAE, 0xAE};
  added = ringbuffer_insert(rb, ae, sizeof(ae));
  EXPECT_EQ((size_t)13, ringbuffer_available(rb));

  // Get everything

  popped = ringbuffer_pop(rb, peek, 16);
  EXPECT_EQ(added, popped);
  EXPECT_EQ((size_t)16, ringbuffer_available(rb));
  EXPECT_EQ((size_t)0, ringbuffer_size(rb));
  ASSERT_TRUE(0 == memcmp(ae, peek, popped));

  ringbuffer_free(rb);
}
