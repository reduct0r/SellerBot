#pragma once
#include <string>

class Product {
public:
    Product(const std::string& name, const std::string& category, double price, int availableQuantity,
        const std::string& description, const std::string& characteristics, const std::string& deliveryInfo, const std::string& imageUrl);

    const std::string& getName() const;
    const std::string& getCategory() const;
    double getPrice() const;
    int getAvailableQuantity() const;
    const std::string& getDescription() const;
    const std::string& getCharacteristics() const;
    const std::string& getDeliveryInfo() const;
    const std::string& getImageUrl() const;

private:
    std::string name;
    std::string category;
    double price;
    int availableQuantity;
    std::string description;
    std::string characteristics;
    std::string deliveryInfo;
    std::string imageUrl;
};
