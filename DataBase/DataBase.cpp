#include "DataBase.h"

DataBase::DataBase(const std::string& connectionString) : connectionString(connectionString) {
    this->categories = { "Printers", "Scanners" };
    this->products = fetchProductsFromDb();
}


std::vector<Product> DataBase::fetchProductsFromDb() {
    std::vector<Product> products;
    pqxx::connection conn(connectionString);

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
                Json::CharReaderBuilder readerBuilder;
                std::string parseErrors;
                std::string specsData = row["specs"].c_str();
                std::istringstream specsStream(specsData);

                if (!Json::parseFromStream(readerBuilder, specsStream, &specsJson, &parseErrors)) {
                    std::cerr << "Error parsing JSON from specs column: " << parseErrors << std::endl;
                    continue;
                }


                std::string characteristics = "";

                if (specsJson.isObject()) {
                    for (const auto& key : specsJson.getMemberNames()) {
                        characteristics += "<i>" + key + "</i>:\n";
                        Json::Value subGroup = specsJson[key];
                        for (const auto& subKey : subGroup.getMemberNames()) {
                            characteristics += "  " + subKey + ": " + subGroup[subKey].asString() + "\n";
                        }
                    }
                }
                else {
                    std::cerr << "Expected JSON object in 'specs' but found different type." << std::endl;
                    continue;
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

const std::vector<Product>& DataBase::getProducts() const {
    return products;
}

const std::vector<std::string>& DataBase::getCategories() const {
    return categories;
}
