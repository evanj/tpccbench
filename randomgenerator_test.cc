#include <cstring>

#include "randomgenerator.h"
#include "stupidunit.h"

namespace tpcc {

TEST(MakeLastName, BadNum) {
    char name[MAX_LAST_NAME+1];
    EXPECT_DEATH(makeLastName(-1, name));
    EXPECT_DEATH(makeLastName(1000, name));
}

TEST(MakeLastName, Success) {
    char name[MAX_LAST_NAME+1];
    makeLastName(371, name);
    EXPECT_EQ(0, strcmp("PRICALLYOUGHT", name));
    makeLastName(178, name);
    EXPECT_EQ(0, strcmp("OUGHTCALLYATION", name));
}

class RandomGeneratorTest : public Test {
protected:
    MockRandomGenerator generator_;
};

TEST_F(RandomGeneratorTest, NumberExcluding) {
    EXPECT_DEATH(generator_.numberExcluding(1, 1, 1));
    EXPECT_EQ(1, generator_.numberExcluding(0, 1, 0));
    EXPECT_EQ(0, generator_.numberExcluding(0, 1, 1));

    generator_.minimum_ = false;
    EXPECT_EQ(2, generator_.numberExcluding(0, 2, 1));
}

TEST_F(RandomGeneratorTest, astring) {
    char buffer[11];
    generator_.astring(buffer, 5, 10);
    EXPECT_EQ(0, strcmp("aaaaa", buffer));

    generator_.minimum_ = false;
    generator_.astring(buffer, 5, 10);
    EXPECT_EQ(0, strcmp("zzzzzzzzzz", buffer));
}

TEST_F(RandomGeneratorTest, nstring) {
    char buffer[6];
    generator_.nstring(buffer, 1, 5);
    EXPECT_EQ(0, strcmp("0", buffer));

    generator_.minimum_ = false;
    generator_.nstring(buffer, 1, 5);
    EXPECT_EQ(0, strcmp("99999", buffer));
}

TEST_F(RandomGeneratorTest, LastName) {
    char name[MAX_LAST_NAME+1];
    generator_.lastName(name, 3000);
    EXPECT_EQ(0, strcmp("BARBARBAR", name));

    generator_.minimum_ = false;
    generator_.lastName(name, 3000);
    EXPECT_EQ(0, strcmp("BARABLEPRI", name));
}

TEST_F(RandomGeneratorTest, LastNameLimited) {
    char name[MAX_LAST_NAME+1];
    generator_.minimum_ = false;
    generator_.lastName(name, 100);
    EXPECT_EQ(0, strcmp("BARESEESE", name));
}

}  // namespace tpcc

int main() {
    return TestSuite::globalInstance()->runAll();
}
