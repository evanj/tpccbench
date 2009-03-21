#include <cassert>
#include <cstring>
#include <string>

#include "clock.h"
#include "mocktpccdb.h"
#include "randomgenerator.h"
#include "stupidunit.h"
#include "tpccclient.h"
#include "tpccdb.h"

using std::string;
using std::vector;

class MockClock : public Clock {
public:
    virtual void getDateTimestamp(char* now) {
        memcpy(now, NOW, DATETIME_SIZE+1);
    }

    virtual int64_t getMicroseconds() { assert(false); return -1; }

    static const char NOW[DATETIME_SIZE+1];
};
const char MockClock::NOW[DATETIME_SIZE+1] = "20080718083852";

class TPCCClientTest : public Test {
public:
    TPCCClientTest() :
            generator_(new tpcc::MockRandomGenerator()),
            db_(new tpcc::MockTPCCDB()),
            client_(new MockClock(), generator_, db_, Item::NUM_ITEMS, WAREHOUSES,
                    District::NUM_PER_WAREHOUSE, Customer::NUM_PER_DISTRICT) {}

protected:
    static const int WAREHOUSES = 5;

    static const int SMALL_ITEMS = 50;
    static const int SMALL_DISTRICTS = 2;
    static const int SMALL_CUSTOMERS = 20;

    TPCCClient* makeSmallClient() {
        generator_ = new tpcc::MockRandomGenerator();
        db_ = new tpcc::MockTPCCDB();
        return new TPCCClient(new MockClock(), generator_, db_, SMALL_ITEMS, 1, SMALL_DISTRICTS,
                SMALL_CUSTOMERS);
    }

    tpcc::MockRandomGenerator* generator_;
    tpcc::MockTPCCDB* db_;
    TPCCClient client_;

    tpcc::NURandC c_;
};

TEST_F(TPCCClientTest, DoStockLevel) {
    client_.doStockLevel();
    EXPECT_EQ(1, db_->w_id_);
    EXPECT_EQ(1, db_->d_id_);
    EXPECT_EQ(TPCCClient::MIN_STOCK_LEVEL_THRESHOLD, db_->stock_level_threshold_);

    generator_->minimum_ = false;
    client_.doStockLevel();
    EXPECT_EQ(WAREHOUSES, db_->w_id_);
    EXPECT_EQ(District::NUM_PER_WAREHOUSE, db_->d_id_);
    EXPECT_EQ(TPCCClient::MAX_STOCK_LEVEL_THRESHOLD, db_->stock_level_threshold_);
}

TEST_F(TPCCClientTest, DoOrderStatus) {
    client_.doOrderStatus();
    EXPECT_EQ(1, db_->w_id_);
    EXPECT_EQ(1, db_->d_id_);
    EXPECT_EQ("BARBARBAR", db_->c_last_);

    generator_->minimum_ = false;
    client_.doOrderStatus();
    EXPECT_EQ(WAREHOUSES, db_->w_id_);
    EXPECT_EQ(District::NUM_PER_WAREHOUSE, db_->d_id_);
    EXPECT_EQ(72, db_->order_status_c_id_);
}

TEST_F(TPCCClientTest, DoOrderStatusSmall) {
    TPCCClient* small = makeSmallClient();
    generator_->minimum_ = false;
    small->doOrderStatus();
    EXPECT_EQ(1, db_->w_id_);
    EXPECT_EQ(SMALL_DISTRICTS, db_->d_id_);
    EXPECT_EQ(4, db_->order_status_c_id_);
    delete small;
}

TEST_F(TPCCClientTest, DoDelivery) {
    client_.doDelivery();
    EXPECT_EQ(1, db_->w_id_);
    EXPECT_EQ(1, db_->delivery_carrier_id_);
    EXPECT_EQ(MockClock::NOW, db_->now_);

    generator_->minimum_ = false;
    client_.doDelivery();
    EXPECT_EQ(WAREHOUSES, db_->w_id_);
    EXPECT_EQ(Order::MAX_CARRIER_ID, db_->delivery_carrier_id_);
    EXPECT_EQ(MockClock::NOW, db_->now_);
}

TEST_F(TPCCClientTest, DoPayment) {
    client_.doPayment();
    EXPECT_EQ(1, db_->w_id_);
    EXPECT_EQ(1, db_->d_id_);
    EXPECT_EQ(1, db_->c_w_id_);
    EXPECT_EQ(1, db_->c_d_id_);
    EXPECT_EQ("BARBARBAR", db_->c_last_);
    EXPECT_EQ(TPCCClient::MIN_PAYMENT_AMOUNT, db_->h_amount_);
    EXPECT_EQ(MockClock::NOW, db_->now_);

    generator_->minimum_ = false;
    client_.doPayment();
    EXPECT_EQ(WAREHOUSES, db_->w_id_);
    EXPECT_EQ(District::NUM_PER_WAREHOUSE, db_->d_id_);
    EXPECT_EQ(WAREHOUSES-1, db_->c_w_id_);
    EXPECT_EQ(District::NUM_PER_WAREHOUSE, db_->c_d_id_);
    EXPECT_EQ(72, db_->c_id_);
    EXPECT_EQ(TPCCClient::MAX_PAYMENT_AMOUNT, db_->h_amount_);
    EXPECT_EQ(MockClock::NOW, db_->now_);
}

TEST_F(TPCCClientTest, DoPaymentSmall) {
    TPCCClient* small = makeSmallClient();
    small->doPayment();
    EXPECT_EQ(1, db_->w_id_);
    EXPECT_EQ(1, db_->d_id_);
    EXPECT_EQ(1, db_->c_w_id_);
    EXPECT_EQ(1, db_->c_d_id_);
    EXPECT_EQ("BARBARBAR", db_->c_last_);

    // With > 1 warehouse, this would select a remote customer
    generator_->minimum_ = false;
    small->doPayment();
    EXPECT_EQ(1, db_->w_id_);
    EXPECT_EQ(SMALL_DISTRICTS, db_->d_id_);
    EXPECT_EQ(1, db_->c_w_id_);
    EXPECT_EQ(SMALL_DISTRICTS, db_->c_d_id_);
    EXPECT_EQ(4, db_->c_id_);
    delete small;
}

TEST_F(TPCCClientTest, DoNewOrder) {
    // minimum = rollback
    db_->new_order_committed_ = false;
    EXPECT_FALSE(client_.doNewOrder());
    EXPECT_EQ(1, db_->w_id_);
    EXPECT_EQ(1, db_->d_id_);
    EXPECT_EQ(2, db_->c_id_);
    EXPECT_EQ(MockClock::NOW, db_->now_);

    EXPECT_EQ(Order::MIN_OL_CNT, db_->items_.size());
    for (int i = 0; i < db_->items_.size(); ++i) {
        if (i+1 == db_->items_.size()) {
            EXPECT_EQ(Item::NUM_ITEMS+1, db_->items_[i].i_id);
        } else {
            EXPECT_EQ(2, db_->items_[i].i_id);
        }
        EXPECT_EQ(2, db_->items_[0].ol_supply_w_id);
        EXPECT_EQ(1, db_->items_[0].ol_quantity);
    }

    // maximum = commit
    db_->new_order_committed_ = true;
    generator_->minimum_ = false;
    EXPECT_TRUE(client_.doNewOrder());
    EXPECT_EQ(WAREHOUSES, db_->w_id_);
    EXPECT_EQ(District::NUM_PER_WAREHOUSE, db_->d_id_);
    EXPECT_EQ(72, db_->c_id_);
    EXPECT_EQ(MockClock::NOW, db_->now_);

    EXPECT_EQ(Order::MAX_OL_CNT, db_->items_.size());
    for (int i = 0; i < db_->items_.size(); ++i) {
        EXPECT_EQ(6496, db_->items_[i].i_id);
        EXPECT_EQ(WAREHOUSES, db_->items_[0].ol_supply_w_id);
        EXPECT_EQ(TPCCClient::MAX_OL_QUANTITY, db_->items_[0].ol_quantity);
    }
}

TEST_F(TPCCClientTest, DoNewOrderSmall) {
    TPCCClient* small = makeSmallClient();
    db_->new_order_committed_ = false;
    EXPECT_FALSE(small->doNewOrder());
    EXPECT_EQ(2, db_->c_id_);

    for (int i = 0; i < db_->items_.size(); ++i) {
        // Normally this would be a "remote" item
        EXPECT_EQ(1, db_->items_[0].ol_supply_w_id);
    }

    generator_->minimum_ = false;
    db_->new_order_committed_ = true;
    EXPECT_TRUE(small->doNewOrder());
    EXPECT_EQ(4, db_->c_id_);
    for (int i = 0; i < db_->items_.size(); ++i) {
        EXPECT_EQ(42, db_->items_[i].i_id);
    }
    
    delete small;
}

TEST_F(TPCCClientTest, DoNewOrderRemoteP) {
    EXPECT_DEATH(client_.remote_item_milli_p(-1));
    EXPECT_DEATH(client_.remote_item_milli_p(1001));
    client_.remote_item_milli_p(0);
    // 100% remote items; maximum nomally is all local items
    client_.remote_item_milli_p(1000);
    db_->new_order_committed_ = true;
    generator_->minimum_ = false;
    EXPECT_TRUE(client_.doNewOrder());
 
    for (int i = 0; i < db_->items_.size(); ++i) {
        EXPECT_EQ(WAREHOUSES-1, db_->items_[0].ol_supply_w_id);
    }
}

TEST_F(TPCCClientTest, Bind) {
    EXPECT_DEATH(client_.bindWarehouseDistrict(-1, 0));
    EXPECT_DEATH(client_.bindWarehouseDistrict(0, -1));
    EXPECT_DEATH(client_.bindWarehouseDistrict(0, District::NUM_PER_WAREHOUSE + 1));
    client_.bindWarehouseDistrict(0, 0);
    client_.bindWarehouseDistrict(0, District::NUM_PER_WAREHOUSE);
    EXPECT_DEATH(client_.bindWarehouseDistrict(WAREHOUSES+1, 0));
    client_.bindWarehouseDistrict(WAREHOUSES, 0);

    // fixed warehouse, random district
    db_->new_order_committed_ = false;
    client_.doNewOrder();
    EXPECT_EQ(WAREHOUSES, db_->w_id_);
    EXPECT_EQ(1, db_->d_id_);
    db_->new_order_committed_ = true;
    generator_->minimum_ = false;
    client_.doNewOrder();
    EXPECT_EQ(WAREHOUSES, db_->w_id_);
    EXPECT_EQ(District::NUM_PER_WAREHOUSE, db_->d_id_);

    // fixed warehouse and district
    client_.bindWarehouseDistrict(2, 5);
    client_.doNewOrder();
    EXPECT_EQ(2, db_->w_id_);
    EXPECT_EQ(5, db_->d_id_);
    db_->new_order_committed_ = false;
    generator_->minimum_ = true;
    client_.doNewOrder();
    EXPECT_EQ(2, db_->w_id_);
    EXPECT_EQ(5, db_->d_id_);
}

int main() {
    return TestSuite::globalInstance()->runAll();
}
