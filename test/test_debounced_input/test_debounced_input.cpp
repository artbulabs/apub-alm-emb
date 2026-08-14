#include <gtest/gtest.h>
#include "DebouncedInput.h"

// ---------------------------------------------------------------------------
// Helpers - simulate pin state via a simple global variable.
// In production code, this would be e.g. []{ return digitalRead(PIR1_PIN); }
// ---------------------------------------------------------------------------
static bool g_pinState = false;
static bool readPin() { return g_pinState; }

// ---------------------------------------------------------------------------
// Tests
// ---------------------------------------------------------------------------

TEST(DebouncedInputTest, InitialStateIsLow) {
    g_pinState = false;
    DebouncedInput input(readPin, 50);
    EXPECT_FALSE(input.getState());
}

TEST(DebouncedInputTest, StateDoesNotChangeBeforeDebounceTime) {
    g_pinState = false;
    DebouncedInput input(readPin, 50);

    g_pinState = true;          // pin goes HIGH
    bool changed = input.update(0);   // t = 0 ms
    EXPECT_FALSE(changed);      // too early, not stable yet
    EXPECT_FALSE(input.getState());   // still reports LOW
}

TEST(DebouncedInputTest, StateChangesAfterDebounceTime) {
    g_pinState = false;
    DebouncedInput input(readPin, 50);

    g_pinState = true;
    input.update(0);            // t = 0 ms, pin just went HIGH

    bool changed = input.update(51);  // t = 51 ms, stable long enough
    EXPECT_TRUE(changed);       // now it should report change
    EXPECT_TRUE(input.getState());    // and stable state is HIGH
}

TEST(DebouncedInputTest, BouncingSignalIsIgnored) {
    g_pinState = false;
    DebouncedInput input(readPin, 50);

    // Simulate bouncing: rapid HIGH-LOW-HIGH within debounce window
    g_pinState = true;
    input.update(0);

    g_pinState = false;         // bounce back to LOW at t=10ms
    input.update(10);

    g_pinState = true;          // bounce to HIGH again at t=20ms
    input.update(20);

    // At t=30ms still within debounce window from last change
    bool changed = input.update(30);
    EXPECT_FALSE(changed);      // no stable change reported
    EXPECT_FALSE(input.getState());   // still LOW (original stable state)
}

TEST(DebouncedInputTest, StateChangesBackToLow) {
    g_pinState = false;
    DebouncedInput input(readPin, 50);

    // Go HIGH
    g_pinState = true;
    input.update(0);
    input.update(51);
    EXPECT_TRUE(input.getState());

    // Go back LOW
    g_pinState = false;
    input.update(52);
    bool changed = input.update(103); // 51ms after last change
    EXPECT_TRUE(changed);
    EXPECT_FALSE(input.getState());
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}