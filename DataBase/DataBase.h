#pragma once
#include <pqxx/pqxx>
#include <json/json.h>
#include <vector>
#include <iostream>
#include "../Product/Product.h"

class DataBase {
public:
    explicit DataBase(const std::string& connectionString);
    std::vector<Product> fetchProductsFromDb();
    const std::vector<Product>& getProducts() const;
    const std::vector<std::string>& getCategories() const;

private:
    std::vector<Product> products;          // каталог всех товаров
    std::vector<std::string> categories;    // список категорий
    std::string connectionString;

    void askModeAndInitialize();
    void adminMode();
    void userMode();
    void explainModes();
};