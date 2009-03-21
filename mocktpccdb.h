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
        return 42;
    }

    virtual void orderStatus(int32_t warehouse_id, int32_t district_id, int32_t customer_id,
            OrderStatusOutput* output) {
        w_id_ = warehouse_id;
        d_id_ = district_id;
        order_status_c_id_ = customer_id;
        setOrderOutput(output);
    }

    virtual void orderStatus(int32_t warehouse_id, int32_t district_id, const char* c_last,
            OrderStatusOutput* output) {
        w_id_ = warehouse_id;
        d_id_ = district_id;
        c_last_ = c_last;
        setOrderOutput(output);
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

    virtual bool newOrderHome(int32_t warehouse_id, int32_t district_id, int32_t customer_id,
            const std::vector<NewOrderItem>& items, const char* now,
            NewOrderOutput* output) {
        assert(false);
        return false;
    }

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
        setPaymentOutput(output);
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
        setPaymentOutput(output);
    }

    virtual void paymentHome(int32_t warehouse_id, int32_t district_id, int32_t c_warehouse_id,
            int32_t c_district_id, int32_t c_id, float h_amount, const char* now,
            PaymentOutput* output) {
        assert(false);
    }
    virtual void paymentRemote(int32_t warehouse_id, int32_t district_id, int32_t c_warehouse_id,
            int32_t c_district_id, int32_t c_id, float h_amount, PaymentOutput* output) {
        assert(false);
    }
    virtual void paymentRemote(int32_t warehouse_id, int32_t district_id, int32_t c_warehouse_id,
            int32_t c_district_id, const char* c_last, float h_amount, PaymentOutput* output) {
        assert(false);
    }

    virtual void delivery(int32_t warehouse_id, int32_t carrier_id, const char* now,
            std::vector<DeliveryOrderInfo>* orders) {
        w_id_ = warehouse_id;
        delivery_carrier_id_ = carrier_id;
        now_ = now;

        orders->resize(1);
        (*orders)[0].d_id = 1;
        (*orders)[0].o_id = 42;
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

private:
    void setOrderOutput(OrderStatusOutput* output) {
        output->c_balance = 1.23f;
        // Null terminate to avoid uninitialized data warnings
        output->c_first[0] = '\0';
        output->c_middle[0] = '\0';
        output->c_last[0] = '\0';
        output->o_entry_d[0] = '\0';
    }

#define TERMINATE_ADDRESS(struct, prefix) \
    struct->prefix ## street_1[0] = '\0'; \
    struct->prefix ## street_2[0] = '\0'; \
    struct->prefix ## city[0] = '\0'; \
    struct->prefix ## state[0] = '\0'; \
    struct->prefix ## zip[0] = '\0'

    void setPaymentOutput(PaymentOutput* output) {
        output->c_balance = 1.23f;
        // Null terminate to avoid uninitialized data warnings
        
        TERMINATE_ADDRESS(output, w_);
        TERMINATE_ADDRESS(output, d_);
        TERMINATE_ADDRESS(output, c_);

        output->c_first[0] = '\0';
        output->c_middle[0] = '\0';
        output->c_last[0] = '\0';
        output->c_phone[0] = '\0';
        output->c_since[0] = '\0';
        output->c_credit[0] = '\0';
        output->c_data[0] = '\0';
    }

#undef TERMINATE_ADDRESS
};

}  // namespace tpcc
#endif
