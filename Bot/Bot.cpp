#include "Bot.h"
#include "../States/MainMenu/MainMenuState.h"
#include <iostream>
#include "../States/Catalog/CatalogState.h"
#include "../States/Category/CategoryState.h"


// Примерный список продуктов
std::vector<Product> products = {
    Product("Printer1", "Printers", 100.0, 10, u8"Описание", u8"Характеристики", u8"Доставка", "https://ir.ozone.ru/s3/multimedia-9/6693430473.jpg"),
    Product("Scanner1", "Scanners", 150.0, 5, u8"Описание", u8"Характеристики", u8"Доставка", "https://ir.ozone.ru/s3/multimedia-9/6693430473.jpg"),
    Product("Printer2", "Printers", 200.0, 8, u8"Описание", u8"Характеристики", u8"Доставка", "https://ir.ozone.ru/s3/multimedia-9/6693430473.jpg")
};

// Примерный список категорий
std::vector<std::string> categories = { "Printers", "Scanners" };


Bot::Bot(const std::string& token) : telegramBot(token), currentState(std::make_shared<StartState>(telegramBot)) {

    telegramBot.getEvents().onCommand("start", [this](TgBot::Message::Ptr message) {
        currentState = std::make_shared<StartState>(telegramBot);                   // сброс стейта
        currentState->handleStart(message);
        });

    telegramBot.getEvents().onCommand("menu", [this](TgBot::Message::Ptr message) {
        currentState = std::make_shared<StartState>(telegramBot);                   // сброс стейта
        currentState->handleMenu(message);
        });

    // Обработка callback-запросов
    telegramBot.getEvents().onCallbackQuery([this](TgBot::CallbackQuery::Ptr query) {
        if (query->data == "catalog") {
            currentState = std::make_shared<CatalogState>(telegramBot, categories, products); 
            currentState->handleStart(query->message);
            telegramBot.getApi().answerCallbackQuery(query->id);
        }
        else if (query->data.rfind("category_", 0) == 0) {
            std::string category = query->data.substr(9); // Получаем название категории
            currentState = std::make_shared<CategoryState>(telegramBot, category, products);
            currentState->handleStart(query->message);    
        }
        else if (query->data.rfind("back_to_category_", 0) == 0) {
            std::string category = query->data.substr(17);
            currentState = std::make_shared<CategoryState>(telegramBot, category, products);
            currentState->handleStart(query->message);
            telegramBot.getApi().answerCallbackQuery(query->id);
        }
        else if (query->data.rfind("add_to_cart_", 0) == 0) {
            std::string productName = query->data.substr(12); // Получаем имя продукта
            auto it = std::find_if(products.begin(), products.end(), [&productName](const Product& product) {
                return product.getName() == productName;
                });

            if (it != products.end()) {
                this->cart.addToCart(*it);
                this->telegramBot.getApi().sendMessage(query->message->chat->id, productName + u8" x 1\nДобавлено в корзину");
            }
            telegramBot.getApi().answerCallbackQuery(query->id);
            // добавить кнопку моя корзина
        }
        // добавить обработку очитски и просмотра корзины
        else {
            currentState->handleMenuQ(query); // Обработка запросов в соответствующих меню
        }

        });
}

void Bot::run() {
    try {
        TgBot::TgLongPoll longPoll(telegramBot);
        while (true) {
            longPoll.start();
        }
    }
    catch (std::exception& e) {
        printf("error: %s\n", e.what());
    }
}

// Cart
Cart::Cart(std::vector<Product> listOfProducts, double sumOfCart) :
    listOfProducts(listOfProducts), sumOfCart(sumOfCart) {}

Cart::Cart() {}

void Cart::addToCart(const Product& product) {
    this->listOfProducts.push_back(product);
}

void Cart::clearCart(TgBot::Message::Ptr message){
    this->listOfProducts.clear();
}

void Bot::showCart(TgBot::Message::Ptr message) {

    std::string cartDetails;

    for (const auto& product : this->cart.listOfProducts) {
        cartDetails += product.getName() + " - " + std::to_string(product.getPrice()) + "\n";
    }

    if (this->cart.listOfProducts.empty()) {
        cartDetails = u8"Ваша корзина пуста.";
    }

    this->telegramBot.getApi().sendMessage(message->chat->id, cartDetails);
}

