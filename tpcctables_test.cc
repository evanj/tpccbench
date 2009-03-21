#include <cstring>

#include "tpcctables.h"
#include "stupidunit.h"

using std::vector;

TEST(CustomerByNameOrdering, Simple) {
    CustomerByNameOrdering lessThan;
    Customer a;
    Customer b;
    a.c_w_id = 1;
    b.c_w_id = 2;
    EXPECT_TRUE(lessThan(&a, &b));
    EXPECT_FALSE(lessThan(&b, &a));

    b.c_w_id = 1;
    a.c_d_id = 1;
    b.c_d_id = 2;
    EXPECT_TRUE(lessThan(&a, &b));
    EXPECT_FALSE(lessThan(&b, &a));

    b.c_d_id = 1;
    strcpy(a.c_last, "Afoo");
    strcpy(b.c_last, "Bfoo");
    EXPECT_TRUE(lessThan(&a, &b));
    EXPECT_FALSE(lessThan(&b, &a));

    b.c_last[0] = 'A';
    strcpy(a.c_first, "Afoo");
    strcpy(b.c_first, "Bfoo");
    EXPECT_TRUE(lessThan(&a, &b));
    EXPECT_FALSE(lessThan(&b, &a));

    b.c_first[0] = 'A';
    EXPECT_FALSE(lessThan(&a, &b));
    EXPECT_FALSE(lessThan(&b, &a));
}

static const char CUSTOMER_FIRST[] = "foo4567890";
static const char CUSTOMER_MIDDLE[] = "AB";
static const char CUSTOMER_LAST[] = "bar4567890123456";
static const float CUSTOMER_BALANCE = 123.45f;
static const float CUSTOMER_DISCOUNT = 0.0003f;
static const char NOW[] = "20080708012243";
static const float AMOUNT = 1.23f;
static const float W_TAX = 0.0001f;
static const float D_TAX = 0.0002f;
static const char ITEM_NAME[] = "12345678901234";
static const char ITEM_DATA[] = "ORIGINAL9012345678901234";
static const char STOCK_DIST[] = "12345678901234";
static const char W_NAME[] = "wname";
static const char D_NAME[] = "dname";
static const char STREET[Address::MAX_STREET+1] = "maxstreet01234567890";

class TPCCTablesTest : public Test {
public:
    TPCCTablesTest() : undo_(NULL) {}

    static const int32_t W_ID = Warehouse::MAX_WAREHOUSE_ID;
    static const int32_t D_ID = 2;
    static const int32_t C_ID = 3;
    static const int32_t CARRIER_ID = 4;
    static const int32_t O_ID = 3972;
    static const int32_t SUPPLY_W_ID = 5;
    static const int32_t QUANTITY = 6;
    static const int32_t ITEM_IM_ID = 52;
    static const float ITEM_PRICE = 1.09f;

    void makeDistrict(int32_t w_id, int32_t d_id, int32_t next_o_id) {
        District d;
        d.d_w_id = w_id;
        d.d_id = d_id;
        d.d_next_o_id = next_o_id;
        d.d_tax = D_TAX;
        d.d_ytd = 0;
        strcpy(d.d_name, D_NAME);
        tables_.insertDistrict(d);
    }

    void makeOrderLine(int32_t w_id, int32_t d_id, int32_t o_id, int32_t number, int32_t i_id) {
        OrderLine line;
        line.ol_w_id = w_id;
        line.ol_d_id = d_id;
        line.ol_o_id = o_id;
        line.ol_number = number;
        line.ol_i_id = i_id;
        line.ol_supply_w_id = SUPPLY_W_ID;
        line.ol_quantity = QUANTITY;
        line.ol_amount = AMOUNT;
        line.ol_delivery_d[0] = '\0';
        tables_.insertOrderLine(line);
    }

    void makeStock(int32_t w_id, int32_t i_id, int32_t quantity, bool original) {
        Stock stock;
        stock.s_w_id = w_id;
        stock.s_i_id = i_id;
        stock.s_quantity = quantity;
        strcpy(stock.s_data, "01234567890123456789012345678901234567890123456789");
        stock.s_ytd = 0;
        stock.s_order_cnt = 0;
        stock.s_remote_cnt = 0;
        if (original) {
            memcpy(stock.s_data + 25, "ORIGINAL", 8);
        }
        for (int i = 0; i < District::NUM_PER_WAREHOUSE; ++i) {
            strcpy(stock.s_dist[i], STOCK_DIST);
        }
        tables_.insertStock(stock);
    }

    void makeCustomer(int32_t w_id, int32_t d_id, int32_t c_id, const char* c_last, const char* c_first) {
        Customer customer;
        customer.c_w_id = w_id;
        customer.c_d_id = d_id;
        customer.c_id = c_id;
        customer.c_balance = CUSTOMER_BALANCE;
        customer.c_discount = CUSTOMER_DISCOUNT;
        customer.c_ytd_payment = 0.0f;
        customer.c_payment_cnt = 0;
        customer.c_delivery_cnt = 0;
        strcpy(customer.c_first, c_first);
        strcpy(customer.c_middle, CUSTOMER_MIDDLE);
        strcpy(customer.c_last, c_last);
        strcpy(customer.c_credit, Customer::BAD_CREDIT);
        strcpy(customer.c_street_2, STREET);
        customer.c_data[0] = '\0';
        tables_.insertCustomer(customer);
    }

    void makeOrder(int32_t w_id, int32_t d_id, int32_t o_id, int32_t c_id, int32_t num_lines) {
        Order order;
        order.o_w_id = w_id;
        order.o_d_id = d_id;
        order.o_id = o_id;
        order.o_c_id = c_id;
        order.o_ol_cnt = num_lines;
        strcpy(order.o_entry_d, NOW);
        order.o_carrier_id = Order::NULL_CARRIER_ID;
        tables_.insertOrder(order);
    }

    void makeWarehouse(int32_t w_id) {
        Warehouse warehouse;
        warehouse.w_id = w_id;
        warehouse.w_tax = W_TAX;
        warehouse.w_ytd = 0;
        strcpy(warehouse.w_name, W_NAME);
        strcpy(warehouse.w_street_1, STREET);
        tables_.insertWarehouse(warehouse);
    }

    void makeItem(int32_t i_id) {
        Item item;
        item.i_id = i_id;
        item.i_im_id = ITEM_IM_ID;
        item.i_price = ITEM_PRICE;
        strcpy(item.i_name, ITEM_NAME);
        strcpy(item.i_data, ITEM_DATA);
        tables_.insertItem(item);
    }

    // Sets up a database and parameters for a successful call to new order.
    void makeNewOrderSuccess(vector<NewOrderItem>* items) {
        makeWarehouse(W_ID);
        makeDistrict(W_ID, D_ID, 22);
        makeCustomer(W_ID, D_ID, C_ID, CUSTOMER_LAST, CUSTOMER_FIRST);
        makeItem(1);
        makeItem(2);
        makeStock(W_ID, 1, 18, true);
        makeStock(W_ID-1, 2, 19, false);

        items->resize(2);
        (*items)[0].i_id = 1;
        (*items)[0].ol_supply_w_id = W_ID;
        (*items)[0].ol_quantity = 9;
        (*items)[1].i_id = 2;
        (*items)[1].ol_supply_w_id = W_ID-1;
        (*items)[1].ol_quantity = 9;
    }

    OrderStatusOutput order_status_output_;
    TPCCTables tables_;
    TPCCUndo* undo_;
};
// Non-integral constants must be defined in a .cc file. Needed for Mac OS X.
// http://www.research.att.com/~bs/bs_faq2.html#in-class
const float TPCCTablesTest::ITEM_PRICE;

TEST_F(TPCCTablesTest, InsertItemSuccess) {
    makeItem(1);

    Item* i = tables_.findItem(1);
    EXPECT_EQ(1, i->i_id);
    EXPECT_EQ(ITEM_IM_ID, i->i_im_id);
    EXPECT_EQ(ITEM_PRICE, i->i_price);
    EXPECT_TRUE(strcmp(ITEM_NAME, i->i_name) == 0);
    EXPECT_TRUE(strcmp(ITEM_DATA, i->i_data) == 0);
}

TEST_F(TPCCTablesTest, InsertItemDuplicate) {
    makeItem(1);
    EXPECT_DEATH(makeItem(1));
}

TEST_F(TPCCTablesTest, InsertWarehouseSuccess) {
    makeWarehouse(W_ID);
    Warehouse* w = tables_.findWarehouse(W_ID);
    EXPECT_EQ(W_ID, w->w_id);
}

TEST_F(TPCCTablesTest, InsertWarehouseDuplicate) {
    makeWarehouse(W_ID);
    EXPECT_DEATH(makeWarehouse(W_ID));
}

TEST_F(TPCCTablesTest, InsertStockSuccess) {
    makeStock(W_ID, 123, 0, false);
    // Same item id, different warehouse id
    makeStock(42, 123, 0, false);

    Stock* s = tables_.findStock(42, 123);
    EXPECT_EQ(42, s->s_w_id);
    EXPECT_EQ(123, s->s_i_id);

    s = tables_.findStock(W_ID, 123);
    EXPECT_EQ(W_ID, s->s_w_id);
    EXPECT_EQ(123, s->s_i_id);
}

TEST_F(TPCCTablesTest, InsertDistrictSuccess) {
    makeDistrict(W_ID, 10, 0);
    // Same district id, different warehouse id
    makeDistrict(42, 10, 0);

    District* d = tables_.findDistrict(42, 10);
    EXPECT_EQ(42, d->d_w_id);
    EXPECT_EQ(10, d->d_id);

    d = tables_.findDistrict(W_ID, 10);
    EXPECT_EQ(W_ID, d->d_w_id);
    EXPECT_EQ(10, d->d_id);
}

TEST_F(TPCCTablesTest, InsertCustomerSuccess) {
    makeCustomer(W_ID, 10, 42, CUSTOMER_LAST, CUSTOMER_FIRST);
    // different district
    makeCustomer(W_ID, 1, 42, CUSTOMER_LAST, CUSTOMER_FIRST);
    // different warehouse
    makeCustomer(1, 10, 42, CUSTOMER_LAST, CUSTOMER_FIRST);

    Customer* c = tables_.findCustomer(W_ID, 10, 42);
    EXPECT_EQ(42, c->c_id);
    EXPECT_EQ(10, c->c_d_id);
    EXPECT_EQ(W_ID, c->c_w_id);

    c = tables_.findCustomer(W_ID, 1, 42);
    EXPECT_EQ(42, c->c_id);
    EXPECT_EQ(1, c->c_d_id);
    EXPECT_EQ(W_ID, c->c_w_id);

    c = tables_.findCustomer(1, 10, 42);
    EXPECT_EQ(42, c->c_id);
    EXPECT_EQ(10, c->c_d_id);
    EXPECT_EQ(1, c->c_w_id);
}

TEST_F(TPCCTablesTest, FindCustomerByName) {
    char fullLastName[Customer::MAX_LAST+1];
    memset(fullLastName, 'Z', Customer::MAX_LAST);
    fullLastName[Customer::MAX_LAST] = '\0';

    makeCustomer(W_ID, D_ID, C_ID, fullLastName, "A");
    Customer* c = tables_.findCustomerByName(W_ID, D_ID, fullLastName);
    EXPECT_EQ(C_ID, c->c_id);

    makeCustomer(W_ID, D_ID, C_ID+1, fullLastName, "B");
    c = tables_.findCustomerByName(W_ID, D_ID, fullLastName);
    EXPECT_EQ(C_ID, c->c_id);

    makeCustomer(W_ID, D_ID, C_ID+2, fullLastName, "C");
    c = tables_.findCustomerByName(W_ID, D_ID, fullLastName);
    EXPECT_EQ(C_ID+1, c->c_id);

    makeCustomer(W_ID, D_ID, C_ID+3, fullLastName, "D");
    c = tables_.findCustomerByName(W_ID, D_ID, fullLastName);
    EXPECT_EQ(C_ID+1, c->c_id);

    // This customer has the next last name.
    fullLastName[Customer::MAX_LAST-1] = static_cast<char>(fullLastName[Customer::MAX_LAST-1] + 1);
    makeCustomer(W_ID, D_ID, C_ID+4, fullLastName, "");
    fullLastName[Customer::MAX_LAST-1] = static_cast<char>(fullLastName[Customer::MAX_LAST-1] - 1);
    c = tables_.findCustomerByName(W_ID, D_ID, fullLastName);
    EXPECT_EQ(C_ID+1, c->c_id);

    // This customer has the previous last name.
    fullLastName[Customer::MAX_LAST-1] = static_cast<char>(fullLastName[Customer::MAX_LAST-1] - 1);
    makeCustomer(W_ID, D_ID, C_ID+5, fullLastName, "");
    fullLastName[Customer::MAX_LAST-1] = static_cast<char>(fullLastName[Customer::MAX_LAST-1] + 1);
    c = tables_.findCustomerByName(W_ID, D_ID, fullLastName);
    EXPECT_EQ(C_ID+1, c->c_id);
}

TEST_F(TPCCTablesTest, InsertOrderSuccess) {
    makeOrder(W_ID, 10, O_ID, 1, 1);
    // different district
    makeOrder(W_ID, 1, O_ID, 1, 1);
    // different warehouse
    makeOrder(1, 10, O_ID, 1, 1);

    Order* o = tables_.findOrder(W_ID, 10, O_ID);
    EXPECT_EQ(O_ID, o->o_id);
    EXPECT_EQ(10, o->o_d_id);
    EXPECT_EQ(W_ID, o->o_w_id);

    o = tables_.findOrder(W_ID, 1, O_ID);
    EXPECT_EQ(O_ID, o->o_id);
    EXPECT_EQ(1, o->o_d_id);
    EXPECT_EQ(W_ID, o->o_w_id);

    o = tables_.findOrder(1, 10, O_ID);
    EXPECT_EQ(O_ID, o->o_id);
    EXPECT_EQ(10, o->o_d_id);
    EXPECT_EQ(1, o->o_w_id);
}

TEST_F(TPCCTablesTest, LastOrderByCustomer) {
    makeOrder(W_ID, District::NUM_PER_WAREHOUSE, 3971, Customer::NUM_PER_DISTRICT, 1);
    makeOrder(W_ID, District::NUM_PER_WAREHOUSE, 3862, Customer::NUM_PER_DISTRICT, 1);

    Order* o = tables_.findLastOrderByCustomer(
            W_ID, District::NUM_PER_WAREHOUSE, Customer::NUM_PER_DISTRICT);
    EXPECT_EQ(3971, o->o_id);
}

TEST_F(TPCCTablesTest, InsertOrderLineSuccess) {
    makeOrderLine(W_ID, 10, O_ID, Order::MAX_OL_CNT, 0);
    // different order
    makeOrderLine(W_ID, 10, 1, Order::MAX_OL_CNT, 0);
    // different district
    makeOrderLine(W_ID, 1, O_ID, Order::MAX_OL_CNT, 0);
    // different warehouse
    makeOrderLine(1, 10, O_ID, Order::MAX_OL_CNT, 0);

    OrderLine* o = tables_.findOrderLine(W_ID, 10, O_ID, Order::MAX_OL_CNT);
    EXPECT_EQ(Order::MAX_OL_CNT, o->ol_number);
    EXPECT_EQ(O_ID, o->ol_o_id);
    EXPECT_EQ(10, o->ol_d_id);
    EXPECT_EQ(W_ID, o->ol_w_id);

    o = tables_.findOrderLine(W_ID, 10, 1, Order::MAX_OL_CNT);
    EXPECT_EQ(Order::MAX_OL_CNT, o->ol_number);
    EXPECT_EQ(1, o->ol_o_id);
    EXPECT_EQ(10, o->ol_d_id);
    EXPECT_EQ(W_ID, o->ol_w_id);

    o = tables_.findOrderLine(W_ID, 1, O_ID, Order::MAX_OL_CNT);
    EXPECT_EQ(Order::MAX_OL_CNT, o->ol_number);
    EXPECT_EQ(O_ID, o->ol_o_id);
    EXPECT_EQ(1, o->ol_d_id);
    EXPECT_EQ(W_ID, o->ol_w_id);

    o = tables_.findOrderLine(1, 10, O_ID, Order::MAX_OL_CNT);
    EXPECT_EQ(Order::MAX_OL_CNT, o->ol_number);
    EXPECT_EQ(O_ID, o->ol_o_id);
    EXPECT_EQ(10, o->ol_d_id);
    EXPECT_EQ(1, o->ol_w_id);
}

TEST_F(TPCCTablesTest, StockLevelNoDistrict) {
    EXPECT_DEATH(tables_.stockLevel(W_ID, D_ID, 10000));
}

TEST_F(TPCCTablesTest, StockLevelNoOrderLines) {
    makeDistrict(W_ID, D_ID, 21);
    EXPECT_EQ(0, tables_.stockLevel(W_ID, D_ID, 10000));
}

TEST_F(TPCCTablesTest, StockLevelNoStock) {
    makeDistrict(W_ID, D_ID, 21);
    makeOrderLine(W_ID, D_ID, 1, 1, 92);
    EXPECT_DEATH(tables_.stockLevel(W_ID, D_ID, 10000));
}

TEST_F(TPCCTablesTest, StockLevelTwoLinesTwoItems) {
    makeDistrict(W_ID, D_ID, 21);
    makeOrderLine(W_ID, D_ID, 1, 1, 92);
    makeOrderLine(W_ID, D_ID, 1, 2, 93);
    makeStock(W_ID, 92, 100, false);
    makeStock(W_ID, 93, 1000, false);

    EXPECT_EQ(2, tables_.stockLevel(W_ID, D_ID, 10000));
    EXPECT_EQ(1, tables_.stockLevel(W_ID, D_ID, 1000));
    EXPECT_EQ(0, tables_.stockLevel(W_ID, D_ID, 100));
}

TEST_F(TPCCTablesTest, StockLevelTwoLinesOneItem) {
    makeDistrict(W_ID, D_ID, 21);
    makeOrderLine(W_ID, D_ID, 1, 1, 92);
    makeOrderLine(W_ID, D_ID, 20, 1, 92);
    makeStock(W_ID, 92, 100, false);

    EXPECT_EQ(1, tables_.stockLevel(W_ID, D_ID, 1000));
    EXPECT_EQ(0, tables_.stockLevel(W_ID, D_ID, 100));
}

TEST_F(TPCCTablesTest, StockLevelTooOld) {
    makeDistrict(W_ID, D_ID, 22);
    // This order id should not be included since 1 < 22-20
    makeOrderLine(W_ID, D_ID, 1, 7, 92);
    makeStock(W_ID, 92, 100, false);

    EXPECT_EQ(0, tables_.stockLevel(W_ID, D_ID, 100));
}

TEST_F(TPCCTablesTest, OrderStatusMissingStuff) {
    // Missing customer
    EXPECT_DEATH(tables_.orderStatus(W_ID, D_ID, C_ID, &order_status_output_));
    // No orders
    makeCustomer(W_ID, D_ID, C_ID, CUSTOMER_LAST, CUSTOMER_FIRST);
    EXPECT_DEATH(tables_.orderStatus(W_ID, D_ID, C_ID, &order_status_output_));
}

TEST_F(TPCCTablesTest, OrderStatusSuccess) {
    makeCustomer(W_ID, D_ID, C_ID, CUSTOMER_LAST, CUSTOMER_FIRST);
    makeOrder(W_ID, D_ID, O_ID, C_ID, 2);
    makeOrder(W_ID, D_ID, 9, C_ID, 1);  // not fetched
    makeOrderLine(W_ID, D_ID, O_ID, 1, 99);
    makeOrderLine(W_ID, D_ID, O_ID, 2, 98);
    makeOrderLine(W_ID, D_ID, 9, 1, 97);  // not fetched
    tables_.orderStatus(W_ID, D_ID, C_ID, &order_status_output_);

    EXPECT_EQ(C_ID, order_status_output_.c_id);
    EXPECT_EQ(CUSTOMER_BALANCE, order_status_output_.c_balance);
    EXPECT_EQ(0, strcmp(CUSTOMER_FIRST, order_status_output_.c_first));
    EXPECT_EQ(0, strcmp(CUSTOMER_MIDDLE, order_status_output_.c_middle));
    EXPECT_EQ(0, strcmp(CUSTOMER_LAST, order_status_output_.c_last));

    EXPECT_EQ(O_ID, order_status_output_.o_id);
    EXPECT_EQ(0, strcmp(NOW, order_status_output_.o_entry_d));
    EXPECT_EQ(Order::NULL_CARRIER_ID, order_status_output_.o_carrier_id);

    ASSERT_EQ(2, order_status_output_.lines.size());
    EXPECT_EQ(99, order_status_output_.lines[0].ol_i_id);
    EXPECT_EQ(98, order_status_output_.lines[1].ol_i_id);
    for (int i = 0; i < 2; ++i) {
        EXPECT_EQ(SUPPLY_W_ID, order_status_output_.lines[i].ol_supply_w_id);
        EXPECT_EQ(QUANTITY, order_status_output_.lines[i].ol_quantity);
        EXPECT_EQ(AMOUNT, order_status_output_.lines[i].ol_amount);
        EXPECT_EQ(0, strlen(order_status_output_.lines[i].ol_delivery_d));
    }
}

TEST_F(TPCCTablesTest, OrderStatusNoCustomer) {
    EXPECT_DEATH(tables_.orderStatus(W_ID, D_ID, "foo", &order_status_output_));
}

TEST_F(TPCCTablesTest, OrderStatusByName) {
    makeCustomer(W_ID, D_ID, C_ID, "A", "");
    makeOrder(W_ID, D_ID, O_ID, C_ID, 1);
    makeOrderLine(W_ID, D_ID, O_ID, 1, 99);
    tables_.orderStatus(W_ID, D_ID, "A", &order_status_output_);
    EXPECT_EQ(C_ID, order_status_output_.c_id);
}

TEST_F(TPCCTablesTest, NewOrderBadItem) {
    vector<NewOrderItem> items(1);
    items[0].i_id = 42;
    items[0].ol_supply_w_id = W_ID;
    items[0].ol_quantity = 9;
    makeDistrict(W_ID, D_ID, 22);
    makeCustomer(W_ID, D_ID, C_ID, CUSTOMER_LAST, CUSTOMER_FIRST);
    NewOrderOutput output;
    bool success = tables_.newOrder(W_ID, D_ID, C_ID, items, NOW, &output, &undo_);
    EXPECT_FALSE(success);
    EXPECT_TRUE(undo_ == NULL);  // no undo allocated: no modifications!

    EXPECT_EQ(22, output.o_id);
    EXPECT_EQ(0, strcmp(CUSTOMER_LAST, output.c_last));
    EXPECT_EQ(0, strcmp(Customer::BAD_CREDIT, output.c_credit));
    EXPECT_EQ(0, strcmp(NewOrderOutput::INVALID_ITEM_STATUS, output.status));

    // Verify that unused fields are initialized. Otherwise serialization/deserialization sucks.
    EXPECT_EQ(0, output.w_tax);
}

TEST_F(TPCCTablesTest, NewOrderSuccess) {
    vector<NewOrderItem> items;
    makeNewOrderSuccess(&items);
    NewOrderOutput output;
    bool success = tables_.newOrder(W_ID, D_ID, C_ID, items, NOW, &output, &undo_);
    EXPECT_TRUE(success);
    EXPECT_TRUE(undo_ != NULL);

    EXPECT_EQ(W_TAX, output.w_tax);
    EXPECT_EQ(D_TAX, output.d_tax);
    EXPECT_EQ(22, output.o_id);
    EXPECT_EQ(CUSTOMER_DISCOUNT, output.c_discount);
    EXPECT_EQ(0, strcmp(CUSTOMER_LAST, output.c_last));
    EXPECT_EQ(0, strcmp(Customer::BAD_CREDIT, output.c_credit));
    EXPECT_EQ(0, strlen(output.status));
    EXPECT_EQ(ITEM_PRICE*18*(1-CUSTOMER_DISCOUNT)*(1+W_TAX+D_TAX), output.total);

    ASSERT_EQ(2, output.items.size());
    EXPECT_EQ(100, output.items[0].s_quantity);
    EXPECT_EQ(ITEM_PRICE, output.items[0].i_price);
    EXPECT_EQ(ITEM_PRICE*9, output.items[0].ol_amount);
    EXPECT_EQ(NewOrderOutput::ItemInfo::BRAND, output.items[0].brand_generic);
    EXPECT_EQ(0, strcmp(ITEM_NAME, output.items[0].i_name));
    EXPECT_EQ(10, output.items[1].s_quantity);
    EXPECT_EQ(ITEM_PRICE, output.items[1].i_price);
    EXPECT_EQ(ITEM_PRICE*9, output.items[1].ol_amount);
    EXPECT_EQ(NewOrderOutput::ItemInfo::GENERIC, output.items[1].brand_generic);
    EXPECT_EQ(0, strcmp(ITEM_NAME, output.items[1].i_name));

    // Check that the district next order was incremented
    EXPECT_EQ(23, tables_.findDistrict(W_ID, D_ID)->d_next_o_id);

    // Check that the stock levels were adjusted correctly
    Stock* s = tables_.findStock(W_ID, 1);
    EXPECT_EQ(100, s->s_quantity);
    EXPECT_EQ(9, s->s_ytd);
    EXPECT_EQ(1, s->s_order_cnt);
    EXPECT_EQ(0, s->s_remote_cnt);
    s = tables_.findStock(W_ID-1, 2);
    EXPECT_EQ(10, s->s_quantity);
    EXPECT_EQ(9, s->s_ytd);
    EXPECT_EQ(1, s->s_order_cnt);
    EXPECT_EQ(1, s->s_remote_cnt);

    Order* o = tables_.findOrder(W_ID, D_ID, 22);
    EXPECT_EQ(C_ID, o->o_c_id);
    EXPECT_EQ(Order::NULL_CARRIER_ID, o->o_carrier_id);
    EXPECT_EQ(2, o->o_ol_cnt);
    EXPECT_EQ(0, o->o_all_local);
    EXPECT_EQ(0, strcmp(NOW, o->o_entry_d));

    NewOrder* neworder = tables_.findNewOrder(W_ID, D_ID, 22);
    EXPECT_EQ(22, neworder->no_o_id);

    OrderLine* line = tables_.findOrderLine(W_ID, D_ID, 22, 1);
    EXPECT_EQ(1, line->ol_i_id);
    EXPECT_EQ(W_ID, line->ol_supply_w_id);
    EXPECT_EQ(9, line->ol_quantity);
    EXPECT_EQ(9*ITEM_PRICE, line->ol_amount);
    EXPECT_EQ(0, strlen(line->ol_delivery_d));
    EXPECT_EQ(0, strcmp(STOCK_DIST, line->ol_dist_info));
    line = tables_.findOrderLine(W_ID, D_ID, 22, 2);
    EXPECT_EQ(2, line->ol_i_id);
    EXPECT_EQ(W_ID-1, line->ol_supply_w_id);
    EXPECT_EQ(9, line->ol_quantity);
    EXPECT_EQ(9*ITEM_PRICE, line->ol_amount);
    EXPECT_EQ(0, strlen(line->ol_delivery_d));
    EXPECT_EQ(0, strcmp(STOCK_DIST, line->ol_dist_info));

    tables_.freeUndo(undo_);
}

TEST_F(TPCCTablesTest, NewOrderPartitionSuccess) {
    makeWarehouse(W_ID);
    makeDistrict(W_ID, D_ID, 22);
    makeCustomer(W_ID, D_ID, C_ID, CUSTOMER_LAST, CUSTOMER_FIRST);
    makeItem(1);
    makeItem(2);
    makeStock(W_ID, 1, 18, false);
    makeStock(W_ID-1, 2, 19, true);

    vector<NewOrderItem> items(2);
    items[0].i_id = 1;
    items[0].ol_supply_w_id = W_ID;
    items[0].ol_quantity = 9;
    items[1].i_id = 2;
    items[1].ol_supply_w_id = W_ID-1;
    items[1].ol_quantity = 9;
    struct NewOrderOutput output;
    bool success = tables_.newOrderHome(W_ID, D_ID, C_ID, items, NOW, &output, NULL);
    EXPECT_TRUE(success);

    ASSERT_EQ(2, output.items.size());
    // brand/generic is computed at the home warehouse. s_quantity comes from remote
    EXPECT_EQ(100, output.items[0].s_quantity);
    EXPECT_EQ(NewOrderOutput::ItemInfo::GENERIC, output.items[0].brand_generic);
    EXPECT_EQ(0, output.items[1].s_quantity);
    EXPECT_EQ(NewOrderOutput::ItemInfo::BRAND, output.items[1].brand_generic);

    // Home warehouse items are modified, but the remote warehouse items are not
    Stock* home_s = tables_.findStock(W_ID, 1);
    EXPECT_EQ(100, home_s->s_quantity);
    EXPECT_EQ(9, home_s->s_ytd);
    EXPECT_EQ(1, home_s->s_order_cnt);
    EXPECT_EQ(0, home_s->s_remote_cnt);
    Stock* remote_s = tables_.findStock(W_ID-1, 2);
    EXPECT_EQ(19, remote_s->s_quantity);
    EXPECT_EQ(0, remote_s->s_ytd);
    EXPECT_EQ(0, remote_s->s_order_cnt);
    EXPECT_EQ(0, remote_s->s_remote_cnt);

    // Execute the remote part
    vector<int32_t> remote_quantities;
    success = tables_.newOrderRemote(W_ID, W_ID-1, items, &remote_quantities, NULL);
    EXPECT_TRUE(success);

    // Remote warehouse item is updated
    EXPECT_EQ(10, remote_s->s_quantity);
    EXPECT_EQ(9, remote_s->s_ytd);
    EXPECT_EQ(1, remote_s->s_order_cnt);
    EXPECT_EQ(1, remote_s->s_remote_cnt);

    // Combining the quantities to produce the correct final output
    TPCCDB::newOrderCombine(remote_quantities, &output);
    EXPECT_EQ(10, output.items[1].s_quantity);
}

TEST_F(TPCCTablesTest, NewOrderAllLocal) {
    makeWarehouse(W_ID);
    makeDistrict(W_ID, D_ID, 22);
    makeCustomer(W_ID, D_ID, C_ID, CUSTOMER_LAST, CUSTOMER_FIRST);
    makeItem(1);
    makeStock(W_ID, 1, 18, true);

    vector<NewOrderItem> items(1);
    items[0].i_id = 1;
    items[0].ol_supply_w_id = W_ID;
    items[0].ol_quantity = 9;
    struct NewOrderOutput output;
    bool success = tables_.newOrder(W_ID, D_ID, C_ID, items, NOW, &output, NULL);

    EXPECT_TRUE(success);
    Order* o = tables_.findOrder(W_ID, D_ID, 22);
    EXPECT_EQ(1, o->o_all_local);
}

TEST_F(TPCCTablesTest, NewOrderUndo) {
    // TODO: Reduce some of this duplication with NewOrderSuccess and NewOrderPartitionSuccess?
    vector<NewOrderItem> items;
    makeNewOrderSuccess(&items);
    NewOrderOutput output;
    bool success = tables_.newOrder(W_ID, D_ID, C_ID, items, NOW, &output, &undo_);
    EXPECT_TRUE(success);

    // The transaction should have modified the database
    EXPECT_EQ(23, tables_.findDistrict(W_ID, D_ID)->d_next_o_id);

    // Undo the transaction: everything should be back the way it was
    tables_.applyUndo(undo_);
    EXPECT_EQ(22, tables_.findDistrict(W_ID, D_ID)->d_next_o_id);

    Stock* s = tables_.findStock(W_ID, 1);
    EXPECT_EQ(0, s->s_order_cnt);
    s = tables_.findStock(W_ID-1, 2);
    EXPECT_EQ(0, s->s_order_cnt);

    EXPECT_EQ(NULL, tables_.findOrder(W_ID, D_ID, 22));
    EXPECT_EQ(NULL, tables_.findNewOrder(W_ID, D_ID, 22));
    EXPECT_EQ(NULL, tables_.findOrderLine(W_ID, D_ID, 22, 1));
    EXPECT_EQ(NULL, tables_.findOrderLine(W_ID, D_ID, 22, 2));
}

TEST_F(TPCCTablesTest, PaymentSuccess) {
    makeWarehouse(W_ID);
    makeDistrict(W_ID, D_ID, 22);
    makeCustomer(W_ID-1, D_ID-1, C_ID, CUSTOMER_LAST, CUSTOMER_FIRST);

    PaymentOutput output;
    tables_.payment(W_ID, D_ID, W_ID-1, D_ID-1, C_ID, 123.45f, NOW, &output, NULL);

    // Verify that YTD has been incremented
    EXPECT_EQ(123.45f, tables_.findWarehouse(W_ID)->w_ytd);
    EXPECT_EQ(123.45f, tables_.findDistrict(W_ID, D_ID)->d_ytd);

    EXPECT_EQ(0, strcmp(STREET, output.w_street_1));
    EXPECT_EQ(0, strcmp(STREET, output.c_street_2));
    EXPECT_EQ(CUSTOMER_BALANCE-123.45f, output.c_balance);
    EXPECT_EQ(CUSTOMER_BALANCE-123.45f, tables_.findCustomer(W_ID-1, D_ID-1, C_ID)->c_balance);
    EXPECT_EQ(123.45f, tables_.findCustomer(W_ID-1, D_ID-1, C_ID)->c_ytd_payment);
    EXPECT_EQ(1, tables_.findCustomer(W_ID-1, D_ID-1, C_ID)->c_payment_cnt);
    EXPECT_EQ(0, strcmp(Customer::BAD_CREDIT, output.c_credit));
    EXPECT_EQ(0, strcmp("(3, 1, 99, 2, 100, 123.45)\n", output.c_data));

    // Check the history table
    ASSERT_EQ(1, tables_.history().size());
    const History* h = tables_.history()[0];
    EXPECT_EQ(W_ID, h->h_w_id);
    EXPECT_EQ(D_ID, h->h_d_id);
    EXPECT_EQ(W_ID-1, h->h_c_w_id);
    EXPECT_EQ(D_ID-1, h->h_c_d_id);
    EXPECT_EQ(C_ID, h->h_c_id);
    EXPECT_EQ(123.45f, h->h_amount);
    EXPECT_EQ(0, strcmp(NOW, h->h_date));
    EXPECT_EQ(0, strcmp("wname    dname", h->h_data));

    // Another payment: check that it does the insert correctly
    tables_.payment(W_ID, D_ID, W_ID-1, D_ID-1, C_ID, 123.45f, NOW, &output, NULL);
    EXPECT_EQ(0, strcmp("(3, 1, 99, 2, 100, 123.45)\n(3, 1, 99, 2, 100, 123.45)\n",
            output.c_data));
    EXPECT_EQ(2, tables_.history().size());

    // Fill c_data to verify that truncation works correctly
    Customer* c = tables_.findCustomer(W_ID-1, D_ID-1, C_ID);
    memset(c->c_data, 'a', Customer::MAX_DATA);
    c->c_data[Customer::MAX_DATA] = '\0';
    static char match[Customer::MAX_DATA+1] = "(3, 1, 99, 2, 100, 123.45)\n";
    size_t length = strlen(match);
    memset(match+length, 'a', Customer::MAX_DATA-length);
    match[Customer::MAX_DATA] = '\0';
    tables_.payment(W_ID, D_ID, W_ID-1, D_ID-1, C_ID, 123.45f, NOW, &output, NULL);
    EXPECT_EQ(0, strcmp(match, output.c_data));
    EXPECT_EQ(3, tables_.history().size());
}

TEST_F(TPCCTablesTest, PaymentRemote) {
    makeWarehouse(W_ID);
    makeDistrict(W_ID, D_ID, 22);
    makeCustomer(W_ID-1, D_ID-1, C_ID, CUSTOMER_LAST, CUSTOMER_FIRST);

    PaymentOutput output;
    tables_.paymentHome(W_ID, D_ID, W_ID-1, D_ID-1, C_ID, 123.45f, NOW, &output, NULL);

    // Verify that YTD has been incremented
    EXPECT_EQ(123.45f, tables_.findWarehouse(W_ID)->w_ytd);
    EXPECT_EQ(123.45f, tables_.findDistrict(W_ID, D_ID)->d_ytd);

    // Warehouse and district data is complete; customer data is not
    EXPECT_EQ(0, strcmp(STREET, output.w_street_1));
    EXPECT_EQ(0, strlen(output.c_street_2));
    EXPECT_EQ(0, output.c_balance);

    // The customer has NOT been updated
    EXPECT_EQ(0, tables_.findCustomer(W_ID-1, D_ID-1, C_ID)->c_payment_cnt);

    // Remote payment
    PaymentOutput out2;
    tables_.paymentRemote(W_ID, D_ID, W_ID-1, D_ID-1, C_ID, 123.45f, &out2, NULL);
    EXPECT_EQ(0, strlen(out2.w_street_1));

    // customer has now been updated
    EXPECT_EQ(1, tables_.findCustomer(W_ID-1, D_ID-1, C_ID)->c_payment_cnt);

    TPCCDB::paymentCombine(out2, &output);
    EXPECT_EQ(0, strcmp(STREET, output.c_street_2));
}

TEST_F(TPCCTablesTest, PaymentGoodCredit) {
    makeWarehouse(W_ID);
    makeDistrict(W_ID, D_ID, 22);
    makeCustomer(W_ID-1, D_ID-1, C_ID, CUSTOMER_LAST, CUSTOMER_FIRST);
    Customer* c = tables_.findCustomer(W_ID-1, D_ID-1, C_ID);
    strcpy(c->c_credit, Customer::GOOD_CREDIT);

    PaymentOutput output;
    tables_.payment(W_ID, D_ID, W_ID-1, D_ID-1, C_ID, 123.45f, NOW, &output, NULL);
    EXPECT_EQ(0, strlen(output.c_data));
}

TEST_F(TPCCTablesTest, PaymentByName) {
    makeWarehouse(W_ID);
    makeDistrict(W_ID, D_ID, 22);
    makeCustomer(W_ID-1, D_ID-1, C_ID, CUSTOMER_LAST, CUSTOMER_FIRST);

    PaymentOutput output;
    tables_.payment(W_ID, D_ID, W_ID-1, D_ID-1, CUSTOMER_LAST, 123.45f, NOW, &output, NULL);
    EXPECT_EQ(output.c_balance, tables_.findCustomer(W_ID-1, D_ID-1, C_ID)->c_balance);
}

TEST_F(TPCCTablesTest, PaymentUndo) {
    makeWarehouse(W_ID);
    makeDistrict(W_ID, D_ID, 22);
    makeCustomer(W_ID-1, D_ID-1, C_ID, CUSTOMER_LAST, CUSTOMER_FIRST);

    PaymentOutput output;
    tables_.payment(W_ID, D_ID, W_ID-1, D_ID-1, C_ID, 123.45f, NOW, &output, &undo_);
    EXPECT_TRUE(undo_ != NULL);
    tables_.applyUndo(undo_);

    // Verify that nothing has changed
    EXPECT_EQ(0, tables_.findWarehouse(W_ID)->w_ytd);
    EXPECT_EQ(0, tables_.findDistrict(W_ID, D_ID)->d_ytd);
    EXPECT_EQ(CUSTOMER_BALANCE, tables_.findCustomer(W_ID-1, D_ID-1, C_ID)->c_balance);
    EXPECT_EQ(0, tables_.history().size());
}

TEST_F(TPCCTablesTest, DeliveryNoOrders) {
    vector<DeliveryOrderInfo> orders;
    tables_.delivery(W_ID, CARRIER_ID, NOW, &orders, NULL);
    EXPECT_EQ(0, orders.size());
}

TEST_F(TPCCTablesTest, DeliveryClear) {
    // The delivery function should clear the output of previous data
    vector<DeliveryOrderInfo> orders(1);
    tables_.delivery(W_ID, CARRIER_ID, NOW, &orders, NULL);
    EXPECT_EQ(0, orders.size());
}

TEST_F(TPCCTablesTest, Delivery) {
    makeCustomer(W_ID, D_ID, C_ID, CUSTOMER_LAST, CUSTOMER_FIRST);
    makeCustomer(W_ID, D_ID-1, C_ID, CUSTOMER_LAST, CUSTOMER_FIRST);
    for (int d_id = D_ID-1; d_id <= D_ID; ++d_id) {
        tables_.insertNewOrder(W_ID, d_id, 1);
        makeOrder(W_ID, d_id, 1, C_ID, 2);
        makeOrderLine(W_ID, d_id, 1, 1, 42);
        makeOrderLine(W_ID, d_id, 1, 2, 43);
    }
    // Extra order in district D_ID should be ignored
    tables_.insertNewOrder(W_ID, D_ID, 2);

    vector<DeliveryOrderInfo> orders;
    tables_.delivery(W_ID, CARRIER_ID, NOW, &orders, NULL);

    ASSERT_EQ(2, orders.size());
    EXPECT_EQ(D_ID-1, orders[0].d_id);
    EXPECT_EQ(1, orders[0].o_id);
    EXPECT_EQ(D_ID, orders[1].d_id);
    EXPECT_EQ(1, orders[1].o_id);

    for (int d_id = D_ID-1; d_id <= D_ID; ++d_id) {
        NewOrder* neworder = tables_.findNewOrder(W_ID, d_id, 1);
        EXPECT_EQ(NULL, neworder);

        Order* o = tables_.findOrder(W_ID, d_id, 1);
        EXPECT_EQ(CARRIER_ID, o->o_carrier_id);

        OrderLine* line = tables_.findOrderLine(W_ID, d_id, 1, 1);
        EXPECT_EQ(0, strcmp(NOW, line->ol_delivery_d));
        line = tables_.findOrderLine(W_ID, d_id, 1, 2);
        EXPECT_EQ(0, strcmp(NOW, line->ol_delivery_d));

        Customer* c = tables_.findCustomer(W_ID, d_id, C_ID);
        EXPECT_EQ(2*AMOUNT+CUSTOMER_BALANCE, c->c_balance);
        EXPECT_EQ(1, c->c_delivery_cnt);
    }
}

TEST_F(TPCCTablesTest, DeliveryTwoWarehouses) {
    makeCustomer(W_ID, D_ID, C_ID, CUSTOMER_LAST, CUSTOMER_FIRST);
    makeCustomer(W_ID-1, D_ID, C_ID, CUSTOMER_LAST, CUSTOMER_FIRST);
    for (int w_id = W_ID-1; w_id <= W_ID; ++w_id) {
        for (int o_id = 1; o_id <= 2; ++o_id) {
            tables_.insertNewOrder(w_id, D_ID, o_id);
            makeOrder(w_id, D_ID, o_id, C_ID, 1);
            makeOrderLine(w_id, D_ID, o_id, 1, 42);
        }
    }

    vector<DeliveryOrderInfo> orders;
    tables_.delivery(W_ID-1, CARRIER_ID, NOW, &orders, NULL);
    EXPECT_EQ(1, orders.size());
    tables_.delivery(W_ID-1, CARRIER_ID, NOW, &orders, NULL);
    EXPECT_EQ(1, orders.size());
    tables_.delivery(W_ID-1, CARRIER_ID, NOW, &orders, NULL);
    EXPECT_EQ(0, orders.size());
}

TEST_F(TPCCTablesTest, DeliveryUndo) {
    makeCustomer(W_ID, D_ID, C_ID, CUSTOMER_LAST, CUSTOMER_FIRST);
    makeCustomer(W_ID, D_ID-1, C_ID, CUSTOMER_LAST, CUSTOMER_FIRST);
    tables_.insertNewOrder(W_ID, D_ID, 1);
    makeOrder(W_ID, D_ID, 1, C_ID, 1);
    makeOrderLine(W_ID, D_ID, 1, 1, 42);

    vector<DeliveryOrderInfo> orders;
    tables_.delivery(W_ID, CARRIER_ID, NOW, &orders, &undo_);
    EXPECT_TRUE(undo_ != NULL);
    tables_.applyUndo(undo_);

    // Make sure the database did not change.
    NewOrder* neworder = tables_.findNewOrder(W_ID, D_ID, 1);
    EXPECT_EQ(1, neworder->no_o_id);

    Order* o = tables_.findOrder(W_ID, D_ID, 1);
    EXPECT_EQ(Order::NULL_CARRIER_ID, o->o_carrier_id);

    OrderLine* line = tables_.findOrderLine(W_ID, D_ID, 1, 1);
    EXPECT_EQ(0, strlen(line->ol_delivery_d));

    Customer* c = tables_.findCustomer(W_ID, D_ID, C_ID);
    EXPECT_EQ(CUSTOMER_BALANCE, c->c_balance);
    EXPECT_EQ(0, c->c_delivery_cnt);
}

int main() {
    return TestSuite::globalInstance()->runAll();
}
