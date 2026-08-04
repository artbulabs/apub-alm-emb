#include <gtest/gtest.h>
#include "AlarmStateMachine.h"

// ---------------------------------------------------------------------------
// Initial state
// ---------------------------------------------------------------------------

TEST(AlarmStateMachineTest, InitialStateIsDisarmed) {
    AlarmStateMachine sm;
    EXPECT_EQ(sm.getState(), AlarmState::Disarmed);
}

TEST(AlarmStateMachineTest, InitialLastSensorIsNull) {
    AlarmStateMachine sm;
    EXPECT_EQ(sm.getLastTriggeredSensor(), nullptr);
}

// ---------------------------------------------------------------------------
// Disarmed state transitions
// ---------------------------------------------------------------------------

TEST(AlarmStateMachineTest, DisarmedToArmedOnArm) {
    AlarmStateMachine sm;
    sm.onArm();
    EXPECT_EQ(sm.getState(), AlarmState::Armed);
}

TEST(AlarmStateMachineTest, DisarmedIgnoresSensorTrigger) {
    AlarmStateMachine sm;
    sm.onSensorTriggered("PIR1");
    EXPECT_EQ(sm.getState(), AlarmState::Disarmed);
    EXPECT_EQ(sm.getLastTriggeredSensor(), nullptr);
}

TEST(AlarmStateMachineTest, DisarmedIgnoresDisarm) {
    AlarmStateMachine sm;
    sm.onDisarm();
    EXPECT_EQ(sm.getState(), AlarmState::Disarmed);
}

// ---------------------------------------------------------------------------
// Armed state transitions
// ---------------------------------------------------------------------------

TEST(AlarmStateMachineTest, ArmedToDisarmedOnDisarm) {
    AlarmStateMachine sm;
    sm.onArm();
    sm.onDisarm();
    EXPECT_EQ(sm.getState(), AlarmState::Disarmed);
}

TEST(AlarmStateMachineTest, ArmedToTriggeredOnSensorEvent) {
    AlarmStateMachine sm;
    sm.onArm();
    sm.onSensorTriggered("PIR1");
    EXPECT_EQ(sm.getState(), AlarmState::Triggered);
}

TEST(AlarmStateMachineTest, ArmedStoresSensorId) {
    AlarmStateMachine sm;
    sm.onArm();
    sm.onSensorTriggered("REED1");
    EXPECT_STREQ(sm.getLastTriggeredSensor(), "REED1");
}

TEST(AlarmStateMachineTest, ArmedIgnoresSubsequentTriggers) {
    AlarmStateMachine sm;
    sm.onArm();
    sm.onSensorTriggered("PIR1");
    sm.onSensorTriggered("PIR2");
    // State stays Triggered, first sensor ID preserved
    EXPECT_EQ(sm.getState(), AlarmState::Triggered);
    EXPECT_STREQ(sm.getLastTriggeredSensor(), "PIR1");
}

// ---------------------------------------------------------------------------
// Triggered state transitions
// ---------------------------------------------------------------------------

TEST(AlarmStateMachineTest, TriggeredToArmedOnNotificationSent) {
    AlarmStateMachine sm;
    sm.onArm();
    sm.onSensorTriggered("PIR1");
    sm.onNotificationSent();
    EXPECT_EQ(sm.getState(), AlarmState::Armed);
}

TEST(AlarmStateMachineTest, TriggeredToArmedOnNotificationFailed) {
    AlarmStateMachine sm;
    sm.onArm();
    sm.onSensorTriggered("PIR1");
    sm.onNotificationFailed();
    EXPECT_EQ(sm.getState(), AlarmState::Armed);
}

TEST(AlarmStateMachineTest, TriggeredIgnoresDisarm) {
    AlarmStateMachine sm;
    sm.onArm();
    sm.onSensorTriggered("PIR1");
    sm.onDisarm();
    EXPECT_EQ(sm.getState(), AlarmState::Triggered);
}

// ---------------------------------------------------------------------------
// State name helper
// ---------------------------------------------------------------------------

TEST(AlarmStateMachineTest, StateNameDisarmed) {
    AlarmStateMachine sm;
    EXPECT_STREQ(sm.getStateName(), "Disarmed");
}

TEST(AlarmStateMachineTest, StateNameArmed) {
    AlarmStateMachine sm;
    sm.onArm();
    EXPECT_STREQ(sm.getStateName(), "Armed");
}

TEST(AlarmStateMachineTest, StateNameTriggered) {
    AlarmStateMachine sm;
    sm.onArm();
    sm.onSensorTriggered("PIR1");
    EXPECT_STREQ(sm.getStateName(), "Triggered");
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
