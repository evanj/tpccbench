#ifndef BENCH_MOCKTPCCDB_H__
#define BENCH_MOCKTPCCDB_H__

#include <cassert>
#include <string>
#include <vector>

#include "tpccdb.h"

namespace tpcc {

class MockTPCCDB : public TPCCDB {
public:
    MockTPCCDB() : new_order_committed_(true) {}

    virtual int32_t stockLevel(int32_t warehouse_id, int32_t district_id, int32_t threshold) {
        w_id_ = warehouse_id;
        d_id_ = district_id;
        stock_level_threshold_ = threshold;
        return 0;
    }

    virtual void orderStatus(int32_t warehouse_id, int32_t district_id, int32_t customer_id,
            OrderStatusOutput* output) {
        w_id_ = warehouse_id;
        d_id_ = district_id;
        order_status_c_id_ = customer_id;
    }

    virtual void orderStatus(int32_t warehouse_id, int32_t district_id, const char* c_last,
            OrderStatusOutput* output) {
        w_id_ = warehouse_id;
        d_id_ = district_id;
        c_last_ = c_last;
    }

    virtual bool newOrder(int32_t warehouse_id, int32_t district_id, int32_t customer_id,
            const std::vector<NewOrderItem>& items, const char* now,
            NewOrderOutput* output) {
        w_id_ = warehouse_id;
        d_id_ = district_id;
        c_id_ = customer_id;
        items_ = items;
        now_ = now;

        output->d_tax = 42;
        return new_order_committed_;
    }

    // Executes the "home warehouse" portion of the new order transaction.
    virtual bool newOrderHome(int32_t warehouse_id, int32_t district_id, int32_t customer_id,
            const std::vector<NewOrderItem>& items, const char* now,
            NewOrderOutput* output) {
        assert(false);
        return false;
    }

    // Executes the "remote warehouse" portion of the new order transaction. Modifies the stock
    // for remote_warehouse. Needs access to all the items in order to reach the same commit/abort
    // decision as the other warehouses. out_quantities is filled with stock quantities: 0 if the
    // item is from another warehouse, or s_quantity if the item is from remote_warehouse.
    virtual bool newOrderRemote(int32_t home_warehouse, int32_t remote_warehouse,
            const std::vector<NewOrderItem>& items, std::vector<int32_t>* out_quantities) {
        assert(false);
        return false;
    }


    virtual void payment(int32_t warehouse_id, int32_t district_id, int32_t c_warehouse_id,
            int32_t c_district_id, int32_t customer_id, float h_amount, const char* now,
            PaymentOutput* output) {
        w_id_ = warehouse_id;
        d_id_ = district_id;
        c_w_id_ = c_warehouse_id;
        c_d_id_ = c_district_id;
        c_id_ = customer_id;
        h_amount_ = h_amount;
        now_ = now;
    }

    virtual void payment(int32_t warehouse_id, int32_t district_id, int32_t c_warehouse_id,
            int32_t c_district_id, const char* c_last, float h_amount, const char* now,
            PaymentOutput* output) {
        w_id_ = warehouse_id;
        d_id_ = district_id;
        c_w_id_ = c_warehouse_id;
        c_d_id_ = c_district_id;
        c_last_ = c_last;
        h_amount_ = h_amount;
        now_ = now;
    }

    virtual void delivery(int32_t warehouse_id, int32_t carrier_id, const char* now,
            std::vector<DeliveryOrderInfo>* orders) {
        w_id_ = warehouse_id;
        delivery_carrier_id_ = carrier_id;
        now_ = now;
    }

    int32_t w_id_;
    int32_t d_id_;
    int32_t stock_level_threshold_;
    int32_t order_status_c_id_;
    std::string c_last_;
    int32_t delivery_carrier_id_;
    std::string now_;
    int32_t c_w_id_;
    int32_t c_d_id_;
    int32_t c_id_;
    float h_amount_;
    std::vector<NewOrderItem> items_;

    bool new_order_committed_;
};

}  // namespace tpcc
#endif
