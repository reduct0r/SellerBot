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
        else if (query->data.rfind("product_", 0) == 0) {
            std::string productName = query->data.substr(8); // Получаем имя продукта
            auto it = std::find_if(products.begin(), products.end(), [&productName](const Product& product) {
                return product.getName() == productName;
                });

            if (it != products.end()) {
                const Product& product = *it;
                TgBot::InlineKeyboardMarkup::Ptr keyboard(new TgBot::InlineKeyboardMarkup);
                std::vector<TgBot::InlineKeyboardButton::Ptr> buttons;

                TgBot::InlineKeyboardButton::Ptr addToCartButton(new TgBot::InlineKeyboardButton);
                addToCartButton->text = u8"Добавить в корзину";
                addToCartButton->callbackData = "add_to_cart_" + productName;
                buttons.push_back(addToCartButton);

                TgBot::InlineKeyboardButton::Ptr backButton(new TgBot::InlineKeyboardButton);
                backButton->text = u8"Назад";
                backButton->callbackData = "back_to_category_" + product.getCategory();
                buttons.push_back(backButton);

                keyboard->inlineKeyboard.push_back(buttons);

                // Отправка сообщения с фотографией, описанием, названием и ценой
                telegramBot.getApi().sendPhoto(query->message->chat->id, product.getImageUrl(),
                    product.getName() + "\n" + product.getDescription() + u8"\nЦена: " + std::to_string(product.getPrice()), 0, keyboard);

                telegramBot.getApi().answerCallbackQuery(query->id);
            }
        }
        else if (query->data.rfind("back_to_category_", 0) == 0) {
            std::string category = query->data.substr(17);
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