#include "DataBase.h"

DataBase::DataBase(const std::string& connectionString) : connectionString(connectionString) {
    this->categories = { "Printers", "Scanners" };
    this->products = fetchProductsFromDb();
    askModeAndInitialize();
}
void DataBase::askModeAndInitialize() {

    while (true) {
        int choice = 0;
        std::cout << "Please select a mode: \n";
        std::cout << "[1] Admin Mode\n";
        std::cout << "[2] User Mode\n";
        std::cout << "[3] What is the difference?\n";

        std::cin >> choice;

        if (std::cin.fail() || (choice < 1 || choice > 3)) {
            std::cout << "Invalid choice. Please select again.\n";
            std::cin.clear(); // Clear error flags
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Discard invalid input
            continue;
        }

        switch (choice) {
        case 1:
            adminMode();
            return;
        case 2:
            userMode();
            return;
        case 3:
            explainModes();
            break;
        default:
            std::cout << "Invalid choice. Please select again.\n";
            break;
        }
    }
}

void DataBase::adminMode() {
    std::cout << "Admin Mode Activated. Current categories are: \n";

    for (const auto& category : categories) {
        std::cout << "- " << category << "\n";
    }

    while (true) {
        std::cout << "Would you like to add a new category? (yes/no): ";
        std::string response;
        std::cin >> response;

        if (response == "yes" || response == "Yes") {
            std::cout << "Enter the name of the new category (type 'done' to finish): ";
            std::string newCategory;

            while (true) {
                std::cin >> newCategory;
                if (newCategory == "done") {
                    break;
                }
                categories.push_back(newCategory);
                std::cout << "New category added: " << newCategory << "\n";
                std::cout << "Enter another category or type 'done' to finish: ";
            }
        }
        else if (response == "no" || response == "No") {
            break;
        }
        else {
            std::cout << "Invalid response, please answer 'yes' or 'no'.\n";
        }
    }
}

void DataBase::userMode() {
    std::cout << "User Mode Activated. No administrative actions will be performed.\n";
}

void DataBase::explainModes() {
    std::cout << "Admin Mode allows you to view and add new categories to the existing list.\n";
    std::cout << "User Mode does not perform any administrative actions.\n";
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
