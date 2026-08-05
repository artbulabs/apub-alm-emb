#include <gtest/gtest.h>
#include "StubNotifier.h"
#include "CompositeNotifier.h"

// ---------------------------------------------------------------------------
// StubNotifier tests
// ---------------------------------------------------------------------------

TEST(StubNotifierTest, InitialStateIsEmpty) {
    StubNotifier stub;
    EXPECT_EQ(stub.lastAlertSensorId(), nullptr);
    EXPECT_EQ(stub.lastStatus(), nullptr);
    EXPECT_EQ(stub.alertCallCount(), 0);
}

TEST(StubNotifierTest, RecordsAlertSensorId) {
    StubNotifier stub;
    stub.sendAlert("PIR1");
    EXPECT_STREQ(stub.lastAlertSensorId(), "PIR1");
}

TEST(StubNotifierTest, CountsAlertCalls) {
    StubNotifier stub;
    stub.sendAlert("PIR1");
    stub.sendAlert("REED1");
    EXPECT_EQ(stub.alertCallCount(), 2);
}

TEST(StubNotifierTest, RecordsStatus) {
    StubNotifier stub;
    stub.sendStatus("Armed");
    EXPECT_STREQ(stub.lastStatus(), "Armed");
}

// ---------------------------------------------------------------------------
// StubNotifier tests
// ---------------------------------------------------------------------------

TEST(CompositeNotifierTest, EmptyCompositeDoesNotCrash) {
    CompositeNotifier composite;
    composite.sendAlert("PIR1");
    composite.sendStatus("Armed");
}

TEST(CompositeNotifierTest, ForwardsAlertToAllNotifiers) {
    StubNotifier stub1;
    StubNotifier stub2;
    CompositeNotifier composite;
    composite.add(&stub1);
    composite.add(&stub2);

    composite.sendAlert("PIR1");

    EXPECT_STREQ(stub1.lastAlertSensorId(), "PIR1");
    EXPECT_STREQ(stub2.lastAlertSensorId(), "PIR1");
}

TEST(CompositeNotifierTest, ForwardsStatusToAllNotifiers) {
    StubNotifier stub1;
    StubNotifier stub2;
    CompositeNotifier composite;
    composite.add(&stub1);
    composite.add(&stub2);

    composite.sendStatus("Disarmed");

    EXPECT_STREQ(stub1.lastStatus(), "Disarmed");
    EXPECT_STREQ(stub2.lastStatus(), "Disarmed");
}

TEST(CompositeNotifierTest, CountsCallsPerNotifier) {
    StubNotifier stub1;
    StubNotifier stub2;
    CompositeNotifier composite;
    composite.add(&stub1);
    composite.add(&stub2);

    composite.sendAlert("PIR1");
    composite.sendAlert("PIR2");

    EXPECT_EQ(stub1.alertCallCount(), 2);
    EXPECT_EQ(stub2.alertCallCount(), 2);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}