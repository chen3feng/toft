#include "toft/system/time/clock.h"
#include "thirdparty/gtest/gtest.h"

namespace toft {

TEST(RealtimeClock, Test)
{
    int64_t us = RealtimeClock.MicroSeconds();
    ASSERT_GT(us, 0);
}

} // namespace toft
