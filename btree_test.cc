#include "btree.h"

#include <map>

#include "assert.h"
#include "stupidunit.h"

using std::pair;
using std::map;
using std::vector;

class BTreeTest : public Test {
protected:
    static const int MAX_KEYS = 3;
    static const int DEPTH_3 = MAX_KEYS+1 + (MAX_KEYS/2+1)*MAX_KEYS;

    void createDepth3() {
        for (int i = 0; i < DEPTH_3; ++i) {
            tree_.insert(i*2, i*2);
        }
    }

    typedef BPlusTree<int, int, MAX_KEYS, MAX_KEYS> TreeType;
    TreeType tree_;
    int output_;
};

TEST_F(BTreeTest, SimpleInsertAndFind) {
    EXPECT_FALSE(tree_.find(52));

    tree_.insert(52, 77);

    EXPECT_TRUE(tree_.find(52, &output_));
    EXPECT_EQ(77, output_);

    EXPECT_FALSE(tree_.find(54));
    EXPECT_FALSE(tree_.find(50));

    // Insert again = overwrite
    tree_.insert(52, 78);
    EXPECT_TRUE(tree_.find(52, &output_));
    EXPECT_EQ(78, output_);

    tree_.insert(54, 79);
    tree_.insert(50, 80);

    EXPECT_TRUE(tree_.find(52, &output_));
    EXPECT_EQ(78, output_);
    EXPECT_TRUE(tree_.find(54, &output_));
    EXPECT_EQ(79, output_);
    EXPECT_TRUE(tree_.find(50, &output_));
    EXPECT_EQ(80, output_);

    EXPECT_FALSE(tree_.find(55));
    EXPECT_FALSE(tree_.find(51));
    EXPECT_FALSE(tree_.find(0));
}

TEST_F(BTreeTest, FindLastLessThan) {
    // Empty tree: nothing to find
    EXPECT_FALSE(tree_.findLastLessThan(52));

    // Everything is >= key, nothing to find
    tree_.insert(52, 1);
    EXPECT_FALSE(tree_.findLastLessThan(52));

    // This works
    int out_key = 0;
    EXPECT_TRUE(tree_.findLastLessThan(100, &output_, &out_key));
    EXPECT_EQ(1, output_);
    EXPECT_EQ(52, out_key);

    tree_.insert(50, 2);
    EXPECT_TRUE(tree_.findLastLessThan(53, &output_, &out_key));
    EXPECT_EQ(1, output_);
    EXPECT_EQ(52, out_key);
    EXPECT_TRUE(tree_.findLastLessThan(52, &output_, &out_key));
    EXPECT_EQ(2, output_);
    EXPECT_EQ(50, out_key);
    EXPECT_TRUE(tree_.findLastLessThan(51, &output_, &out_key));
    EXPECT_EQ(2, output_);
    EXPECT_EQ(50, out_key);
    output_ = -1;
    out_key = -1;
    EXPECT_FALSE(tree_.findLastLessThan(50, &output_, &out_key));
    EXPECT_EQ(-1, output_);
    EXPECT_EQ(-1, out_key);

    tree_.insert(49, 3);
    EXPECT_TRUE(tree_.findLastLessThan(52, &output_, &out_key));
    EXPECT_EQ(2, output_);
    EXPECT_EQ(50, out_key);
}

// This test currently fails: findLastLessThan does not work when things are deleted.
TEST_F(BTreeTest, FindLastLessThanDeleted) {
    tree_.insert(52, 1);
    tree_.insert(50, 2);
    tree_.del(52);
    EXPECT_TRUE(tree_.findLastLessThan(53, &output_));
    EXPECT_EQ(2, output_);
}

TEST_F(BTreeTest, FindLastLessThan2) { 
    tree_.insert(42, -1);
    tree_.insert(1804289383, 1);
    tree_.insert(1804289383, 2);
    tree_.insert( 719885386, 3);
    tree_.insert(1804289383, 4);
    tree_.insert( 783368690, 5);
    tree_.insert( 719885386, 6);
    EXPECT_TRUE(tree_.findLastLessThan(2044897763, &output_));
    EXPECT_EQ(4, output_);
    
    tree_.insert( 304089172, 7);
    EXPECT_TRUE(tree_.findLastLessThan(1804289383, &output_));
    EXPECT_EQ(5, output_);
}

// Runs find, lower_bound, and upper_bound in both the STL set and the B-tree_.
bool doFind(const map<int, int>& std_map, const BPlusTree<int, int, 3, 3>& tree, const int key) {
    // Test the weird "find first less than". Not compatible with deletes
    map<int, int>::const_iterator i = std_map.lower_bound(key);
    int map_less_than = -2;
    if (i != std_map.begin()) {
        // Go back one
        --i;
        ASSERT(i->first < key);
        map_less_than = i->second;
    }

    int tree_less_than = -2;
    tree.findLastLessThan(key, &tree_less_than);
    ASSERT(map_less_than == tree_less_than);

    i = std_map.find(key);
    int output = 0;
    bool tree_found = tree.find(key, &output);
    if (i != std_map.end()) {
        ASSERT(tree_found && output == i->second);
        return true;
    } else {
        ASSERT(!tree_found);
        return false;
    }
}

TEST_F(BTreeTest, RandomInsertUnique) {
    srandom(static_cast<unsigned int>(time(NULL)));

    map<int, int> std_map;
    vector<int> values;

    tree_.insert(42, -1);
    std_map.insert(std::make_pair(42, -1));
    values.push_back(42);
    //~ printf("insert %d = %d\n", 42, -1);
    
    for (int loops = 0; loops < 1000; ++loops) {
        // Do an insert (alternating between new and old inserts)
        int value;
        if (loops % 2 == 0) {
            value = static_cast<int>(random());
        } else {
            value = values[random() % values.size()];
        }

        pair<map<int, int>::iterator, bool> map_result = std_map.insert(std::make_pair(value, loops+1));
        tree_.insert(value, loops+1);
        //~ printf("insert %d = %d\n", value, loops+1);
        if (map_result.second) {
            values.push_back(value);
        } else {
            map_result.first->second = loops+1;
        }

        // Find a value in the set
        value = values[random() % values.size()];
        //~ printf("find %d\n", value);
        ASSERT_TRUE(doFind(std_map, tree_, value));

        // Find a value not in the set
        value = static_cast<int>(random());
        vector<int>::iterator it = std::find(values.begin(), values.end(), value);
        //~ printf("find %d\n", value);
        ASSERT_EQ(it != values.end(), doFind(std_map, tree_, value));

        // Randomly delete a value from the set
        if (loops % 3 == 2) {
            size_t index = random() % values.size();
            value = values[index];
            values.erase(values.begin() + index);
            //~ printf("del %d\n", value);
            std_map.erase(std_map.find(value));
            tree_.del(value);
        }
    }

    for (size_t i = 0; i < values.size(); ++i) {
        ASSERT_TRUE(tree_.find(values[i], &output_));
        map<int, int>::const_iterator it = std_map.find(values[i]);
        ASSERT_EQ(it->second, output_);
    }
}

int main() {
    return TestSuite::globalInstance()->runAll();
}
