#include <gtest/gtest.h>
#include "board_config.h"

TEST(BoardConfigTest, NativeBoardNameIsDefined) {
    EXPECT_STREQ(BOARD_NAME, "Native (unit tests)");
}

TEST(BoardConfigTest, NativeHasNoModem) {
    EXPECT_EQ(HAS_MODEM, 0);
}

TEST(BoardConfigTest, NativePinsAreMinusOne) {
    EXPECT_EQ(PIR1_PIN, -1);
    EXPECT_EQ(PIR2_PIN, -1);
    EXPECT_EQ(PIR3_PIN, -1);
    EXPECT_EQ(REED1_PIN, -1);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}