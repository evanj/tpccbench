#ifndef TPCCCLIENT_H__
#define TPCCCLIENT_H__

#include <stdint.h>

namespace tpcc {
class RandomGenerator;
}

class Clock;
class TPCCDB;

// Generates transactions according to the TPC-C specification. This ignores the fact that
// terminals have a fixed w_id, d_id, and that requests should be made after a minimum keying time
// and a think time.
class TPCCClient {
public:
    // Owns clock, generator and db.
    TPCCClient(Clock* clock, tpcc::RandomGenerator* generator, TPCCDB* db, int num_items,
            int num_warehouses, int districts_per_warehouse, int customers_per_district);
    ~TPCCClient();

    void doStockLevel();
    void doOrderStatus();
    void doDelivery();
    void doPayment();
    void doNewOrder();

    void doOne();

    static const int32_t MIN_STOCK_LEVEL_THRESHOLD = 10;
    static const int32_t MAX_STOCK_LEVEL_THRESHOLD = 20;
    // TODO: Should these constants be part of tpccdb.h?
    static const float MIN_PAYMENT_AMOUNT = 1.00;
    static const float MAX_PAYMENT_AMOUNT = 5000.00;
    static const int32_t MAX_OL_QUANTITY = 10;

private:
    int32_t generateWarehouse();
    int32_t generateDistrict();
    int32_t generateCID();
    int32_t generateItemID();

    Clock* clock_;
    tpcc::RandomGenerator* generator_;
    TPCCDB* db_;
    int num_items_;
    int num_warehouses_;
    int districts_per_warehouse_;
    int customers_per_district_;
};

#endif