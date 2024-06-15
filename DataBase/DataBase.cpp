#include "DataBase.h"

std::vector<Product> fetchProductsFromDb(const std::string& connectionString) {
    std::vector<Product> products;

    try {
        pqxx::connection conn(connectionString);
        if (conn.is_open()) {
            std::cout << "DataBase started: DataBase connection ready\n";
            pqxx::work txn(conn);

            std::string query = "SELECT name, category, price, amount, description, specs, delivery, link FROM products";
            pqxx::result result = txn.exec(query);

            for (const auto& row : result) {
                std::string name = row["name"].c_str();
                std::string category = row["category"].c_str();
                double price = row["price"].as<double>();
                int availableQuantity = row["amount"].as<int>();
                std::string description = row["description"].c_str();

                // Parsing JSON from specs column
                Json::Value specsJson;
                Json::Reader reader;
                if (!reader.parse(row["specs"].c_str(), specsJson)) {
                    std::cerr << "Error parsing JSON from specs column" << std::endl;
                    continue;
                }

                // Formatting characteristics string
                std::string characteristics = "";
                for (const auto& key : specsJson.getMemberNames()) {
                    characteristics += " " + key + " : " + specsJson[key].asString() + "\n";
                }

                std::string deliveryInfo = std::to_string(row["delivery"].as<int>());
                std::string imageUrl = row["link"].c_str();

                products.emplace_back(name, category, price, availableQuantity, description, characteristics, deliveryInfo, imageUrl);
            }
        }
        else {
            std::cerr << "Can't open database" << std::endl;
        }
    }
    catch (const std::exception& e) {
        std::cerr << "DataBase error: " << e.what() << std::endl;
    }

    return products;
}