#include "randomgenerator.h"
#include "tpccgenerator.h"
#include "tpcctables.h"
#include "stupidunit.h"

using std::vector;

static const char NOW[] = "20080708012243";

class TPCCGeneratorTest : public Test {
public:
    TPCCGeneratorTest() :
            random_(new tpcc::MockRandomGenerator()),
            generator_(random_, NOW, Item::NUM_ITEMS, District::NUM_PER_WAREHOUSE,
                    Customer::NUM_PER_DISTRICT, NewOrder::INITIAL_NUM_PER_DISTRICT) {}

protected:
    Item item_;
    Warehouse warehouse_;
    tpcc::MockRandomGenerator* random_;
    TPCCTables tables_;

    TPCCGenerator generator_;
};

TEST_F(TPCCGeneratorTest, GenerateItemMin) {
    generator_.generateItem(42, false, &item_);
    EXPECT_EQ(42, item_.i_id);
    EXPECT_EQ(Item::MIN_IM, item_.i_im_id);
    EXPECT_EQ(Item::MIN_PRICE, item_.i_price);
    EXPECT_EQ(Item::MIN_NAME, strlen(item_.i_name));
    EXPECT_EQ(Item::MIN_DATA, strlen(item_.i_data));
    EXPECT_EQ(NULL, strstr(item_.i_data, "ORIGINAL"));

    generator_.generateItem(42, true, &item_);
    EXPECT_EQ(Item::MIN_DATA, strlen(item_.i_data));
    EXPECT_EQ(0, memcmp(item_.i_data, "ORIGINAL", 8));
}

TEST_F(TPCCGeneratorTest, GenerateItemMax) {
    random_->minimum_ = false;
    generator_.generateItem(43, false, &item_);
    EXPECT_EQ(43, item_.i_id);
    EXPECT_EQ(Item::MAX_IM, item_.i_im_id);
    EXPECT_EQ(Item::MAX_PRICE, item_.i_price);
    EXPECT_EQ(Item::MAX_NAME, strlen(item_.i_name));
    EXPECT_EQ(Item::MAX_DATA, strlen(item_.i_data));
    EXPECT_EQ(NULL, strstr(item_.i_data, "ORIGINAL"));

    generator_.generateItem(43, true, &item_);
    EXPECT_EQ(Item::MAX_DATA, strlen(item_.i_data));
    EXPECT_EQ(0, memcmp(item_.i_data + Item::MAX_DATA - 8, "ORIGINAL", 8));
}

TEST_F(TPCCGeneratorTest, MakeItemsTable) {
    TPCCGenerator small(new tpcc::MockRandomGenerator(), NOW, 10, District::NUM_PER_WAREHOUSE,
            Customer::NUM_PER_DISTRICT, NewOrder::INITIAL_NUM_PER_DISTRICT);
    small.makeItemsTable(&tables_);

    int original_count = 0;
    for (int i = 1; i <= 10; ++i) {
        Item* item = tables_.findItem(i);
        EXPECT_EQ(i, item->i_id);
        if (strstr(item->i_data, "ORIGINAL") != NULL) {
            original_count += 1;
        }
    }

    EXPECT_EQ(1, original_count);
}

TEST_F(TPCCGeneratorTest, GenerateWarehouseMin) {
    generator_.generateWarehouse(42, &warehouse_);
    EXPECT_EQ(42, warehouse_.w_id);
    EXPECT_EQ(Warehouse::MIN_TAX, warehouse_.w_tax);
    EXPECT_EQ(Warehouse::INITIAL_YTD, warehouse_.w_ytd);
    EXPECT_EQ(Warehouse::MIN_NAME, strlen(warehouse_.w_name));
    EXPECT_EQ(Address::MIN_STREET, strlen(warehouse_.w_street_1));
    EXPECT_EQ(Address::MIN_STREET, strlen(warehouse_.w_street_2));
    EXPECT_EQ(Address::MIN_CITY, strlen(warehouse_.w_city));
    EXPECT_EQ(Address::STATE, strlen(warehouse_.w_state));
    EXPECT_EQ(Address::ZIP, strlen(warehouse_.w_zip));
    EXPECT_EQ(0, memcmp(warehouse_.w_zip + Address::ZIP - 5, "11111", 5));
}

TEST_F(TPCCGeneratorTest, GenerateWarehouseMax) {
    random_->minimum_ = false;
    generator_.generateWarehouse(43, &warehouse_);
    EXPECT_EQ(43, warehouse_.w_id);
    EXPECT_EQ(Warehouse::MAX_TAX, warehouse_.w_tax);
    EXPECT_EQ(Warehouse::INITIAL_YTD, warehouse_.w_ytd);
    EXPECT_EQ(Warehouse::MAX_NAME, strlen(warehouse_.w_name));
    EXPECT_EQ(Address::MAX_STREET, strlen(warehouse_.w_street_1));
    EXPECT_EQ(Address::MAX_STREET, strlen(warehouse_.w_street_2));
    EXPECT_EQ(Address::MAX_CITY, strlen(warehouse_.w_city));
    EXPECT_EQ(Address::STATE, strlen(warehouse_.w_state));
    EXPECT_EQ(Address::ZIP, strlen(warehouse_.w_zip));
    EXPECT_EQ(0, memcmp(warehouse_.w_zip + Address::ZIP - 5, "11111", 5));
}

TEST_F(TPCCGeneratorTest, GenerateStock) {
    Stock stock;
    generator_.generateStock(42, 97, false, &stock);
    EXPECT_EQ(42, stock.s_i_id);
    EXPECT_EQ(97, stock.s_w_id);
    EXPECT_EQ(Stock::MIN_QUANTITY, stock.s_quantity);
    EXPECT_EQ(0, stock.s_ytd);
    EXPECT_EQ(0, stock.s_order_cnt);
    EXPECT_EQ(0, stock.s_remote_cnt);
    for (int i = 0; i < District::NUM_PER_WAREHOUSE; ++i) {
        EXPECT_EQ(Stock::DIST, strlen(stock.s_dist[i]));
    }
    EXPECT_EQ(Stock::MIN_DATA, strlen(stock.s_data));

    random_->minimum_ = false;
    generator_.generateStock(42, 97, true, &stock);
    EXPECT_EQ(Stock::MAX_QUANTITY, stock.s_quantity);
    EXPECT_EQ(Stock::MAX_DATA, strlen(stock.s_data));
    EXPECT_EQ(0, memcmp(stock.s_data + Stock::MAX_DATA - 8 , "ORIGINAL", 8));
}

TEST_F(TPCCGeneratorTest, GenerateDistrict) {
    District district;
    generator_.generateDistrict(9, 97, &district);
    EXPECT_EQ(9, district.d_id);
    EXPECT_EQ(97, district.d_w_id);
    EXPECT_EQ(District::MIN_TAX, district.d_tax);
    EXPECT_EQ(District::INITIAL_YTD, district.d_ytd);
    EXPECT_EQ(District::INITIAL_NEXT_O_ID, district.d_next_o_id);
    EXPECT_EQ(District::MIN_NAME, strlen(district.d_name));
    EXPECT_EQ(Address::MIN_STREET, strlen(district.d_street_1));
    EXPECT_EQ(Address::MIN_STREET, strlen(district.d_street_2));
    EXPECT_EQ(Address::MIN_CITY, strlen(district.d_city));
    EXPECT_EQ(Address::STATE, strlen(district.d_state));
    EXPECT_EQ(Address::ZIP, strlen(district.d_zip));
    EXPECT_EQ(0, memcmp(district.d_zip + Address::ZIP - 5, "11111", 5));

    random_->minimum_ = false;
    generator_.generateDistrict(9, 97, &district);
    EXPECT_EQ(District::MAX_TAX, district.d_tax);
    EXPECT_EQ(District::MAX_NAME, strlen(district.d_name));
    EXPECT_EQ(Address::MAX_STREET, strlen(district.d_street_1));
    EXPECT_EQ(Address::MAX_STREET, strlen(district.d_street_2));
    EXPECT_EQ(Address::MAX_CITY, strlen(district.d_city));
}

TEST_F(TPCCGeneratorTest, GenerateSmallDistrict) {
    TPCCGenerator small(new tpcc::MockRandomGenerator(), NOW, 50, 5, 10, 2);

    District district;
    small.generateDistrict(5, 97, &district);
    EXPECT_EQ(11, district.d_next_o_id);
}

TEST_F(TPCCGeneratorTest, GenerateCustomer) {
    Customer customer;
    generator_.generateCustomer(43, 97, 105, false, &customer);
    EXPECT_EQ(43, customer.c_id);
    EXPECT_EQ(97, customer.c_d_id);
    EXPECT_EQ(105, customer.c_w_id);
    EXPECT_EQ(Customer::INITIAL_CREDIT_LIM, customer.c_credit_lim);
    EXPECT_EQ(Customer::MIN_DISCOUNT, customer.c_discount);
    EXPECT_EQ(Customer::INITIAL_BALANCE, customer.c_balance);
    EXPECT_EQ(Customer::INITIAL_YTD_PAYMENT, customer.c_ytd_payment);
    EXPECT_EQ(Customer::INITIAL_PAYMENT_CNT, customer.c_payment_cnt);
    EXPECT_EQ(Customer::INITIAL_DELIVERY_CNT, customer.c_delivery_cnt);
    EXPECT_EQ(Customer::MIN_FIRST, strlen(customer.c_first));
    EXPECT_EQ(0, strcmp("OE", customer.c_middle));
    EXPECT_EQ(0, strcmp("BARPRESABLE", customer.c_last));
    EXPECT_EQ(Address::MIN_STREET, strlen(customer.c_street_1));
    EXPECT_EQ(Address::MIN_STREET, strlen(customer.c_street_2));
    EXPECT_EQ(Address::MIN_CITY, strlen(customer.c_city));
    EXPECT_EQ(Address::STATE, strlen(customer.c_state));
    EXPECT_EQ(Address::ZIP, strlen(customer.c_zip));
    EXPECT_EQ(0, memcmp(customer.c_zip + Address::ZIP - 5, "11111", 5));
    EXPECT_EQ(Customer::PHONE, strlen(customer.c_phone));
    EXPECT_EQ(0, strcmp(NOW, customer.c_since));
    EXPECT_EQ(0, strcmp(Customer::GOOD_CREDIT, customer.c_credit));
    EXPECT_EQ(Customer::MIN_DATA, strlen(customer.c_data));

    random_->minimum_ = false;
    generator_.generateCustomer(1001, 97, 105, true, &customer);
    EXPECT_EQ(Customer::MAX_DISCOUNT, customer.c_discount);
    EXPECT_EQ(Customer::MAX_FIRST, strlen(customer.c_first));
    // NURand(255, 0, 999) with C = 0 -> 23
    EXPECT_EQ(0, strcmp("BARABLEPRI", customer.c_last));
    EXPECT_EQ(Address::MAX_STREET, strlen(customer.c_street_1));
    EXPECT_EQ(Address::MAX_STREET, strlen(customer.c_street_2));
    EXPECT_EQ(Address::MAX_CITY, strlen(customer.c_city));
    EXPECT_EQ(0, strcmp(Customer::BAD_CREDIT, customer.c_credit));
    EXPECT_EQ(Customer::MAX_DATA, strlen(customer.c_data));
}

TEST_F(TPCCGeneratorTest, GenerateOrder) {
    Order order;
    generator_.generateOrder(19, 43, 97, 105, false, &order);
    EXPECT_EQ(19, order.o_id);
    EXPECT_EQ(43, order.o_c_id);
    EXPECT_EQ(97, order.o_d_id);
    EXPECT_EQ(105, order.o_w_id);
    EXPECT_EQ(Order::MIN_CARRIER_ID, order.o_carrier_id);
    EXPECT_EQ(Order::MIN_OL_CNT, order.o_ol_cnt);
    EXPECT_EQ(Order::INITIAL_ALL_LOCAL, order.o_all_local);
    EXPECT_EQ(0, strcmp(NOW, order.o_entry_d));

    random_->minimum_ = false;
    generator_.generateOrder(19, 43, 97, 105, false, &order);
    EXPECT_EQ(Order::MAX_CARRIER_ID, order.o_carrier_id);
    EXPECT_EQ(Order::MAX_OL_CNT, order.o_ol_cnt);

    generator_.generateOrder(19, 43, 97, 105, true, &order);
    EXPECT_EQ(Order::NULL_CARRIER_ID, order.o_carrier_id);
}

TEST_F(TPCCGeneratorTest, GenerateOrderLine) {
    OrderLine orderline;
    generator_.generateOrderLine(5, 19, 43, 97, false, &orderline);
    EXPECT_EQ(19, orderline.ol_o_id);
    EXPECT_EQ(43, orderline.ol_d_id);
    EXPECT_EQ(97, orderline.ol_w_id);
    EXPECT_EQ(5, orderline.ol_number);
    EXPECT_EQ(OrderLine::MIN_I_ID, orderline.ol_i_id);
    EXPECT_EQ(97, orderline.ol_supply_w_id);
    EXPECT_EQ(0, strcmp(NOW, orderline.ol_delivery_d));
    EXPECT_EQ(OrderLine::INITIAL_QUANTITY, orderline.ol_quantity);
    EXPECT_EQ(0.00, orderline.ol_amount);
    EXPECT_EQ(Stock::DIST, strlen(orderline.ol_dist_info));

    generator_.generateOrderLine(5, 19, 43, 97, true, &orderline);
    EXPECT_EQ(OrderLine::MIN_AMOUNT, orderline.ol_amount);
    EXPECT_EQ(0, strlen(orderline.ol_delivery_d));

    random_->minimum_ = false;
    generator_.generateOrderLine(5, 19, 43, 97, true, &orderline);
    EXPECT_EQ(OrderLine::MAX_I_ID, orderline.ol_i_id);
    EXPECT_EQ(OrderLine::MAX_AMOUNT, orderline.ol_amount);
    EXPECT_EQ(0, strlen(orderline.ol_delivery_d));
}

TEST_F(TPCCGeneratorTest, GenerateHistory) {
    History history;
    generator_.generateHistory(5, 19, 43, &history);
    EXPECT_EQ(5, history.h_c_id);
    EXPECT_EQ(19, history.h_c_d_id);
    EXPECT_EQ(19, history.h_d_id);
    EXPECT_EQ(43, history.h_c_w_id);
    EXPECT_EQ(43, history.h_w_id);
    EXPECT_EQ(History::INITIAL_AMOUNT, history.h_amount);
    EXPECT_EQ(0, strcmp(NOW, history.h_date));
    EXPECT_EQ(History::MIN_DATA, strlen(history.h_data));

    random_->minimum_ = false;
    generator_.generateHistory(5, 19, 43, &history);
    EXPECT_EQ(History::MAX_DATA, strlen(history.h_data));
}

TEST_F(TPCCGeneratorTest, MakeWarehouse) {
    TPCCGenerator small(new tpcc::MockRandomGenerator(), NOW, 10, 5, 10, 2);
    small.makeWarehouse(&tables_, 42);
    Warehouse* w = tables_.findWarehouse(42);
    EXPECT_EQ(42, w->w_id);

    int original_count = 0;
    for (int i = 1; i <= 10; ++i) {
        Stock* stock = tables_.findStock(42, i);
        EXPECT_EQ(i, stock->s_i_id);
        EXPECT_EQ(42, stock->s_w_id);
        if (strstr(stock->s_data, "ORIGINAL") != NULL) {
            original_count += 1;
        }
    }
    EXPECT_EQ(1, original_count);

    for (int i = 1; i <= 5; ++i) {
        District* district = tables_.findDistrict(42, i);
        EXPECT_EQ(i, district->d_id);
        EXPECT_EQ(42, district->d_w_id);
    }

    // Check one district's customers
    int bc_count = 0;
    for (int i = 1; i <= 10; ++i) {
        Customer* customer = tables_.findCustomer(42, 5, i);
        EXPECT_EQ(i, customer->c_id);
        EXPECT_EQ(5, customer->c_d_id);
        EXPECT_EQ(42, customer->c_w_id);
        if (strcmp(Customer::BAD_CREDIT, customer->c_credit) == 0) {
            bc_count += 1;
        }

        // Check the history for this customer
        bool found = false;
        const vector<const History*>& history = tables_.history();
        for (int j = 0; j < history.size(); ++j) {
            if (history[j]->h_c_id == i && history[j]->h_c_d_id == 5 && history[j]->h_c_w_id == 42) {
                found = true;
                break;
            }
        }
        EXPECT_TRUE(found);
    }
    EXPECT_EQ(1, bc_count);

    // Check the district's orders
    for (int o_id = 1; o_id <= 10; ++o_id) {
        Order* order = tables_.findOrder(42, 5, o_id);
        EXPECT_EQ(o_id, order->o_id);
        // c_id is not random with the MockRNG
        EXPECT_EQ(o_id, order->o_c_id);
        EXPECT_EQ(5, order->o_d_id);
        EXPECT_EQ(42, order->o_w_id);

        // Check the corresponding order lines
        for (int ol_number = 1; ol_number <= order->o_ol_cnt; ++ol_number) {
            OrderLine* orderline = tables_.findOrderLine(42, 5, o_id, ol_number);
            EXPECT_EQ(ol_number, orderline->ol_number);
            EXPECT_EQ(o_id, orderline->ol_o_id);
            // c_id is not random with the MockRNG
            EXPECT_EQ(5, order->o_d_id);
            EXPECT_EQ(42, order->o_w_id);

            // The last 2 orders should be new orders    
            if (o_id <= 8) {
                // ol_amount = 0.00 if this is not a new order
                EXPECT_EQ(0.00f, orderline->ol_amount);
                EXPECT_EQ(0, strcmp(NOW, orderline->ol_delivery_d));
            } else {
                EXPECT_EQ(OrderLine::MIN_AMOUNT, orderline->ol_amount);
                EXPECT_EQ(0, strlen(orderline->ol_delivery_d));
            }
        }

        // The last 2 orders should be new orders    
        if (o_id <= 8) {
            EXPECT_EQ(NULL, tables_.findNewOrder(42, 5, o_id));
        } else {
            NewOrder* no = tables_.findNewOrder(42, 5, o_id);
            EXPECT_EQ(42, no->no_w_id);
            EXPECT_EQ(5, no->no_d_id);
            EXPECT_EQ(o_id, no->no_o_id);
        }
    }
}

int main() {
    return TestSuite::globalInstance()->runAll();
}
