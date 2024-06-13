#include "Bot.h"
#include "../States/MainMenu/MainMenuState.h"
#include <iostream>
#include "../States/Catalog/CatalogState.h"
#include "../States/Category/CategoryState.h"

// Примерный список продуктов
std::vector<Product> products = {
    Product("Printer1", "Printers", 100.0, 10, u8"Описание", u8"Характеристики", u8"Доставка", "URL"),
    Product("Scanner1", "Scanners", 150.0, 5, u8"Описание", u8"Характеристики", u8"Доставка", "URL"),
    Product("Printer2", "Printers", 200.0, 8, u8"Описание", u8"Характеристики", u8"Доставка", "URL")
};

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
            telegramBot.getApi().answerCallbackQuery(query->id);
        }
        // Здесь можно добавить обработку callback для продуктов
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