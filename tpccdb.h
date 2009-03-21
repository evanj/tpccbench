#ifndef TPCCDB_H__
#define TPCCDB_H__

#include <stdint.h>

#include <cstring>
#include <vector>

struct Item {
    static const int MIN_IM = 1;
    static const int MAX_IM = 10000;
    static const float MIN_PRICE = 1.00;
    static const float MAX_PRICE = 100.00;
    static const int MIN_NAME = 14;
    static const int MAX_NAME = 24;
    static const int MIN_DATA = 26;
    static const int MAX_DATA = 50;
    static const int NUM_ITEMS = 100000;

    int32_t i_id;
    int32_t i_im_id;
    float i_price;
    char i_name[MAX_NAME+1];
    char i_data[MAX_DATA+1];
};

struct Warehouse {
    static const float MIN_TAX = 0;
    static const float MAX_TAX = 0.2000f;
    static const float INITIAL_YTD = 300000.00f;
    static const int MIN_NAME = 6;
    static const int MAX_NAME = 10;
    static const int MIN_STREET = 10;
    static const int MAX_STREET = 20;
    static const int MIN_CITY = 10;
    static const int MAX_CITY = 20;
    static const int STATE = 2;
    static const int ZIP = 9;
    // TPC-C 1.3.1 (page 11) requires 2*W. This permits testing up to 50 warehouses. This is an
    // arbitrary limit created to pack ids into integers.
    static const int MAX_WAREHOUSE_ID = 100;

    int32_t w_id;
    float w_tax;
    float w_ytd;
    char w_name[MAX_NAME+1];
    char w_street_1[MAX_STREET+1];
    char w_street_2[MAX_STREET+1];
    char w_city[MAX_CITY+1];
    char w_state[STATE+1];
    char w_zip[ZIP+1];
};

struct District {
    static const float MIN_TAX = 0;
    static const float MAX_TAX = 0.2000f;
    static const float INITIAL_YTD = 30000.00;  // different from Warehouse
    static const int INITIAL_NEXT_O_ID = 3001;
    static const int MIN_NAME = 6;
    static const int MAX_NAME = 10;
    static const int MIN_STREET = 10;
    static const int MAX_STREET = 20;
    static const int MIN_CITY = 10;
    static const int MAX_CITY = 20;
    static const int STATE = 2;
    static const int ZIP = 9;
    static const int NUM_PER_WAREHOUSE = 10;

    int32_t d_id;
    int32_t d_w_id;
    float d_tax;
    float d_ytd;
    int32_t d_next_o_id;
    char d_name[MAX_NAME+1];
    char d_street_1[MAX_STREET+1];
    char d_street_2[MAX_STREET+1];
    char d_city[MAX_CITY+1];
    char d_state[STATE+1];
    char d_zip[ZIP+1];
};

struct Stock {
    static const int MIN_QUANTITY = 10;
    static const int MAX_QUANTITY = 100;
    static const int DIST = 24;
    static const int MIN_DATA = 26;
    static const int MAX_DATA = 50;
    static const int NUM_STOCK_PER_WAREHOUSE = 100000;

    int32_t s_i_id;
    int32_t s_w_id;
    int32_t s_quantity;
    int32_t s_ytd;
    int32_t s_order_cnt;
    int32_t s_remote_cnt;
    char s_dist[District::NUM_PER_WAREHOUSE][DIST+1];
    char s_data[MAX_DATA+1];
};

// YYYY-MM-DD HH:MM:SS This is supposed to be a date/time field from Jan 1st 1900 -
// Dec 31st 2100 with a resolution of 1 second. See TPC-C 1.3.1.
static const int DATETIME_SIZE = 14;

struct Customer {
    static const float INITIAL_CREDIT_LIM = 50000.00;
    static const float MIN_DISCOUNT = 0.0000;
    static const float MAX_DISCOUNT = 0.5000;
    static const float INITIAL_BALANCE = -10.00;
    static const float INITIAL_YTD_PAYMENT = 10.00;
    static const int INITIAL_PAYMENT_CNT = 1;
    static const int INITIAL_DELIVERY_CNT = 0;
    static const int MIN_FIRST = 6;
    static const int MAX_FIRST = 10;
    static const int MIDDLE = 2;
    static const int MAX_LAST = 16;
    static const int MIN_STREET = 10;
    static const int MAX_STREET = 20;
    static const int MIN_CITY = 10;
    static const int MAX_CITY = 20;
    static const int STATE = 2;
    static const int ZIP = 9;
    static const int PHONE = 16;
    static const int CREDIT = 2;
    static const int MIN_DATA = 300;
    static const int MAX_DATA = 500;
    static const int NUM_PER_DISTRICT = 3000;
    static const char GOOD_CREDIT[];
    static const char BAD_CREDIT[];

    int32_t c_id;
    int32_t c_d_id;
    int32_t c_w_id;
    float c_credit_lim;
    float c_discount;
    float c_balance;
    float c_ytd_payment;
    int32_t c_payment_cnt;
    int32_t c_delivery_cnt;
    char c_first[MAX_FIRST+1];
    char c_middle[MIDDLE+1];
    char c_last[MAX_LAST+1];
    char c_street_1[MAX_STREET+1];
    char c_street_2[MAX_STREET+1];
    char c_city[MAX_CITY+1];
    char c_state[STATE+1];
    char c_zip[ZIP+1];
    char c_phone[PHONE+1];
    char c_since[DATETIME_SIZE+1];
    char c_credit[CREDIT+1];
    char c_data[MAX_DATA+1];
};

struct Order {
    static const int MIN_CARRIER_ID = 1;
    static const int MAX_CARRIER_ID = 10;
    // HACK: This is not strictly correct, but it works
    static const int NULL_CARRIER_ID = 0;
    // Less than this value, carrier != null, >= -> carrier == null
    static const int NULL_CARRIER_LOWER_BOUND = 2101;
    static const int MIN_OL_CNT = 5;
    static const int MAX_OL_CNT = 15;
    static const int INITIAL_ALL_LOCAL = 1;
    static const int INITIAL_ORDERS_PER_DISTRICT = 3000;
    // See TPC-C 1.3.1 (page 15)
    static const int MAX_ORDER_ID = 10000000;

    int32_t o_id;
    int32_t o_c_id;
    int32_t o_d_id;
    int32_t o_w_id;
    int32_t o_carrier_id;
    int32_t o_ol_cnt;
    int32_t o_all_local;
    char o_entry_d[DATETIME_SIZE+1];
};

struct OrderLine {
    static const int MIN_I_ID = 1;
    static const int MAX_I_ID = 100000;  // Item::NUM_ITEMS
    static const int INITIAL_QUANTITY = 5;
    static const float MIN_AMOUNT = 0.01f;
    static const float MAX_AMOUNT = 9999.99f;

    int32_t ol_o_id;
    int32_t ol_d_id;
    int32_t ol_w_id;
    int32_t ol_number;
    int32_t ol_i_id;
    int32_t ol_supply_w_id;
    int32_t ol_quantity;
    float ol_amount;
    char ol_delivery_d[DATETIME_SIZE+1];
    char ol_dist_info[Stock::DIST+1];
};

struct NewOrder {
    static const int INITIAL_NUM_PER_DISTRICT = 900;

    int32_t no_w_id;
    int32_t no_d_id;
    int32_t no_o_id;
};

struct History {
    static const int MIN_DATA = 12;
    static const int MAX_DATA = 24;
    static const float INITIAL_AMOUNT = 10.00f;

    int32_t h_c_id;
    int32_t h_c_d_id;
    int32_t h_c_w_id;
    int32_t h_d_id;
    int32_t h_w_id;
    float h_amount;
    char h_date[DATETIME_SIZE+1];
    char h_data[MAX_DATA];
};

// Data returned by the "order status" transaction.
struct OrderStatusOutput {
    // From customer
    int32_t c_id;  // unclear if this needs to be returned
    float c_balance;

    // From order
    int32_t o_id;
    int32_t o_carrier_id;

    struct OrderLineSubset {
        int32_t ol_i_id;
        int32_t ol_supply_w_id;
        int32_t ol_quantity;
        float ol_amount;
        char ol_delivery_d[DATETIME_SIZE+1];
    };

    std::vector<OrderLineSubset> lines;

    // From customer
    char c_first[Customer::MAX_FIRST+1];
    char c_middle[Customer::MIDDLE+1];
    char c_last[Customer::MAX_LAST+1];

    // From order
    char o_entry_d[DATETIME_SIZE+1];
};

struct NewOrderItem {
    int32_t i_id;
    int32_t ol_supply_w_id;
    int32_t ol_quantity;
};

struct NewOrderOutput {
    // Zero initialize everything. This avoids copying uninitialized data around when
    // serializing/deserializing.
    NewOrderOutput() : w_tax(0), d_tax(0), o_id(0), c_discount(0), total(0) {
        memset(c_last, 0, sizeof(c_last));
        memset(c_credit, 0, sizeof(c_credit));
        memset(status, 0, sizeof(status));
    }

    float w_tax;
    float d_tax;

    // From district d_next_o_id
    int32_t o_id;

    float c_discount;

    // TODO: Client can compute this from other values.
    float total;

    struct ItemInfo {
        static const char BRAND = 'B';
        static const char GENERIC = 'G';

        int32_t s_quantity;
        float i_price;
        // TODO: Client can compute this from other values.
        float ol_amount;
        char brand_generic;
        char i_name[Item::MAX_NAME+1];
    };

    std::vector<ItemInfo> items;
    char c_last[Customer::MAX_LAST+1];
    char c_credit[Customer::CREDIT+1];

    static const int MAX_STATUS = 25;
    static const char INVALID_ITEM_STATUS[];
    char status[MAX_STATUS+1];
};

struct PaymentOutput {
    // Return entire tuples since Payment requires most of the data. This returns more than
    // necessary, but is easy.
    Warehouse warehouse;
    District district;
    Customer customer;
};

struct DeliveryOrderInfo {
    int32_t d_id;
    int32_t o_id;
};

// Interface to the TPC-C transaction implementation.
class TPCCDB {
public:
    virtual ~TPCCDB() {}

    // Executes the TPC-C "slev" transaction. From the last 20 orders, returns the number of rows in
    // the STOCK table that have S_QUANTITY < threshold. See TPC-C 2.8 (page 43).
    virtual int stockLevel(int32_t warehouse_id, int32_t district_id, int32_t threshold) = 0;

    // Executes the TPC-C order status transaction. Find the customer's last order and check the
    // delivery date of each item on the order. See TPC-C 2.6 (page 36).
    virtual void orderStatus(int32_t warehouse_id, int32_t district_id, int32_t customer_id,
            OrderStatusOutput* output) = 0;

    // Executes the TPC-C order status transaction. Find the customer's last order and check the
    // delivery date of each item on the order. See TPC-C 2.6 (page 36).
    virtual void orderStatus(int32_t warehouse_id, int32_t district_id, const char* c_last,
            OrderStatusOutput* output) = 0;

    // Executes the TPC-C new order transaction. Enter the new order for customer_id into the
    // database. See TPC-C 2.4 (page 27). Returns true if the transaction commits.
    virtual bool newOrder(int32_t warehouse_id, int32_t district_id, int32_t customer_id,
            const std::vector<NewOrderItem>& items, const char* now,
            NewOrderOutput* output) = 0;

    // Executes the TPC-C payment transaction. Add h_amount to the customer's account.
    // See TPC-C 2.5 (page 32).
    virtual void payment(int32_t warehouse_id, int32_t district_id, int32_t c_warehouse_id,
            int32_t c_district_id, int32_t customer_id, float h_amount, const char* now,
            PaymentOutput* output) = 0;

    // Executes the TPC-C payment transaction. Add h_amount to the customer's account.
    // See TPC-C 2.5 (page 32).
    virtual void payment(int32_t warehouse_id, int32_t district_id, int32_t c_warehouse_id,
            int32_t c_district_id, const char* c_last, float h_amount, const char* now,
            PaymentOutput* output) = 0;

    // Executes the TPC-C delivery transaction. Delivers the oldest undelivered transaction in each
    // district in warehouse_id. See TPC-C 2.7 (page 39).
    virtual void delivery(int32_t warehouse_id, int32_t carrier_id, const char* now,
            std::vector<DeliveryOrderInfo>* orders) = 0;
};

#endif
