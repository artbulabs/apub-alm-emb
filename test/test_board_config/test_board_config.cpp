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
    EXPECT_EQ(REED2_PIN, -1);
}

TEST(BoardConfigTest, NativeCameraPinsAreMinusOne) {
    EXPECT_EQ(CAM_PWDN_PIN,  -1);
    EXPECT_EQ(CAM_RESET_PIN, -1);
    EXPECT_EQ(CAM_XCLK_PIN,  -1);
    EXPECT_EQ(CAM_SIOD_PIN,  -1);
    EXPECT_EQ(CAM_SIOC_PIN,  -1);
    EXPECT_EQ(CAM_Y9_PIN,    -1);
    EXPECT_EQ(CAM_Y8_PIN,    -1);
    EXPECT_EQ(CAM_Y7_PIN,    -1);
    EXPECT_EQ(CAM_Y6_PIN,    -1);
    EXPECT_EQ(CAM_Y5_PIN,    -1);
    EXPECT_EQ(CAM_Y4_PIN,    -1);
    EXPECT_EQ(CAM_Y3_PIN,    -1);
    EXPECT_EQ(CAM_Y2_PIN,    -1);
    EXPECT_EQ(CAM_VSYNC_PIN, -1);
    EXPECT_EQ(CAM_HREF_PIN,  -1);
    EXPECT_EQ(CAM_PCLK_PIN,  -1);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
