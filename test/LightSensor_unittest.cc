#include "../LightSensor.h"
#include "gtest/gtest.h"

using ::testing::Return;

class LightSensorTest : public ::testing::Test {
    protected:
        ArduinoMock* arduinoMock;
        LightSensor* tested;

    public:
        virtual void SetUp() {
            arduinoMock = arduinoMockInstance();
            tested = NULL;
        }

        virtual void TearDown() {
            if (tested) {
                delete tested;
                tested = NULL;
            }
            releaseArduinoMock();
        }
};

TEST_F(LightSensorTest, Constructor) {

    EXPECT_CALL(*arduinoMock, analogRead(12))
        .WillOnce(Return(1));

    EXPECT_CALL(*arduinoMock, millis())
        .WillOnce(Return(123));

    tested = new LightSensor(12);
    EXPECT_EQ(false, tested->isDark());
    EXPECT_EQ(true, tested->isLight());
}

TEST_F(LightSensorTest, Read) {
    EXPECT_CALL(*arduinoMock, analogRead(12))
        .WillOnce(Return(110))
        .WillOnce(Return(110))
        .WillOnce(Return(9));

    EXPECT_CALL(*arduinoMock, millis())
        .WillOnce(Return(123)) // init
        .WillOnce(Return(124)) // 1 - first read
        .WillOnce(Return(125)) // 2 - no read
        .WillOnce(Return(225));// 3 - dark

    tested = new LightSensor(12);
    EXPECT_EQ(false, tested->isDark());
    EXPECT_EQ(true, tested->isLight());

    // first read
    EXPECT_EQ(false, tested->read());
    EXPECT_EQ(false, tested->isDark());
    EXPECT_EQ(true, tested->isLight());

    // no read
    EXPECT_EQ(false, tested->read());
    EXPECT_EQ(false, tested->isDark());
    EXPECT_EQ(true, tested->isLight());

    // dark
    EXPECT_EQ(true, tested->read());
    EXPECT_EQ(true, tested->isDark());
    EXPECT_EQ(false, tested->isLight());

}
