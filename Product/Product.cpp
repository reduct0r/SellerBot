#include "Product.h"

Product::Product(const std::string& name, const std::string& category, double price, int availableQuantity,
    const std::string& description, const std::string& characteristics, const std::string& deliveryInfo, const std::string& imageUrl)
    : name(name), category(category), price(price), availableQuantity(availableQuantity),
    description(description), characteristics(characteristics), deliveryInfo(deliveryInfo), imageUrl(imageUrl) {}

const std::string& Product::getName() const {
    return name;
}

const std::string& Product::getCategory() const {
    return category;
}

double Product::getPrice() const {
    return price;
}

int Product::getAvailableQuantity() const {
    return availableQuantity;
}

const std::string& Product::getDescription() const {
    return description;
}

const std::string& Product::getCharacteristics() const {
    return characteristics;
}

const std::string& Product::getDeliveryInfo() const {
    return deliveryInfo;
}

const std::string& Product::getImageUrl() const {
    return imageUrl;
}

inline void Product::decrementQuantity() {
    if (availableQuantity > 0) {
        --availableQuantity;
    }
}