#include <gtest/gtest.h>
#include "../ImageBuffer.h"


/**
 * Test of initial state of the buffer
 */
TEST(ImageBufferTest, InitialConditions) {
    ImageBuffer<int, 4> buffer;

    EXPECT_EQ(buffer.getSize(), 4);
}

TEST(ImageBufferTest, DifferentSize) {
    ImageBuffer<int, 4> buffer;

    EXPECT_EQ(buffer.getSize(), 5);
}