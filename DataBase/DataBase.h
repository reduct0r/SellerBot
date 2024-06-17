#pragma once
#include <pqxx/pqxx>
#include <json/json.h>
#include <vector>
#include <iostream>
#include "../Product/Product.h"

class DataBase {
public:
    explicit DataBase(const std::string& connectionString); // Конструктор
    std::vector<Product> fetchProductsFromDb();             // Парсер БД
    const std::vector<Product>& getProducts() const;        // Геттер товаров
    const std::vector<std::string>& getCategories() const;  // Геттер категорий

    void confirmOrder(const std::string& productName); // Обновление БД при заказе товаров

private:
    std::vector<Product> products;          // Каталог всех товаров
    std::vector<std::string> categories;    // Список категорий
    std::string connectionString;           // Строка подключения к БД

    // Методы консольного меню
    void askModeAndInitialize();
    void adminMode();
    void userMode();
    void explainModes();
};