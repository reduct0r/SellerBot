#include <pqxx/pqxx>
#include <json/json.h>
#include <vector>
#include <iostream>
#include "../Product/Product.h"

std::vector<Product> fetchProductsFromDb(const std::string& connectionString);