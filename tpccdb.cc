#include "tpccdb.h"

// Non-integral constants must be defined in a .cc file. Needed for Mac OS X.
// http://www.research.att.com/~bs/bs_faq2.html#in-class
const float Item::MIN_PRICE;
const float Item::MAX_PRICE;
const float Warehouse::MIN_TAX;
const float Warehouse::MAX_TAX;
const float Warehouse::INITIAL_YTD;
const float District::MIN_TAX;
const float District::MAX_TAX;
const float District::INITIAL_YTD;  // different from Warehouse
const float Customer::MIN_DISCOUNT;
const float Customer::MAX_DISCOUNT;
const float Customer::INITIAL_BALANCE;
const float Customer::INITIAL_CREDIT_LIM;
const float Customer::INITIAL_YTD_PAYMENT;
const char Customer::GOOD_CREDIT[] = "GC";
const char Customer::BAD_CREDIT[] = "BC";
const float OrderLine::MIN_AMOUNT;
const float OrderLine::MAX_AMOUNT;
const char NewOrderOutput::INVALID_ITEM_STATUS[] = "Item number is not valid";
const float History::INITIAL_AMOUNT;
