#include "CategoryState.h"
#include "../MainMenu/MainMenuState.h"
#include "../Catalog/CatalogState.h"
#include "../../Bot/Bot.h"
#include <regex>

CategoryState::CategoryState(TgBot::Bot& bot, const std::string& category, DataBase& dataBase)
    : bot(bot), category(category), products(dataBase.getProducts()) { }

void CategoryState::handleStart(TgBot::Message::Ptr message) {
    auto chatId = message->chat->id;
    TgBot::InlineKeyboardMarkup::Ptr keyboard(new TgBot::InlineKeyboardMarkup);

    std::vector<TgBot::InlineKeyboardButton::Ptr> row1;

    TgBot::InlineKeyboardButton::Ptr button1(new TgBot::InlineKeyboardButton);
    TgBot::InlineKeyboardButton::Ptr button2(new TgBot::InlineKeyboardButton);

    button1->text = u8"Назад в каталог";
    button1->callbackData = "back_to_catalog";
    button2->text = u8"🔍 Поиск и Фильтры";
    button2->callbackData = "sort";

    row1.push_back(button1);
    row1.push_back(button2);
    keyboard->inlineKeyboard.push_back(row1);

    for (const auto& product : products) {

        if (product.getCategory() == category) {
            std::vector<TgBot::InlineKeyboardButton::Ptr> row;
            TgBot::InlineKeyboardButton::Ptr button(new TgBot::InlineKeyboardButton);
            std::string str = std::to_string(product.getPrice());
            button->text = product.getName() + "   " + str.erase(str.find_last_not_of('0') + 2, std::string::npos) + u8" руб.";
            button->callbackData = "product_" + product.getName(); // Уникальный идентификатор для продукта
            row.push_back(button);
            keyboard->inlineKeyboard.push_back(row);
        }
    }

    bot.getApi().sendMessage(chatId, u8"Продукты в категории<b> " + category + "</b>", false, 0, keyboard, "HTML");
}

void CategoryState::handleMenuQ(TgBot::CallbackQuery::Ptr query, std::shared_ptr<TelegramState>& currentState, DataBase& dataBase)
{
    if (query->data == "search_items") {
        bot.getApi().sendMessage(query->message->chat->id, u8"Введите ключевое слово для поиска товаров:");
        bot.getApi().answerCallbackQuery(query->id);
        waitingForKeyword = true;
    }
    else if (query->data == "cancel_search") {
        handleStart(query->message);
        bot.getApi().answerCallbackQuery(query->id);
    }
    else if (query->data.rfind(u8"product_", 0) == 0) {
        std::string productName = query->data.substr(8); // Получаем имя продукта
        auto it = std::find_if(products.begin(), products.end(), [&productName](const Product& product) {
            return product.getName() == productName;
        });

        if (it != products.end()) {
            const Product& product = *it;
            TgBot::InlineKeyboardMarkup::Ptr keyboard(new TgBot::InlineKeyboardMarkup);
            std::vector<TgBot::InlineKeyboardButton::Ptr> buttons;

            TgBot::InlineKeyboardButton::Ptr backButton(new TgBot::InlineKeyboardButton);
            backButton->text = u8"Назад";
            backButton->callbackData = "back_to_category_" + product.getCategory();
            buttons.push_back(backButton);

            TgBot::InlineKeyboardButton::Ptr addToCartButton(new TgBot::InlineKeyboardButton);
            if (product.getAvailableQuantity() > 0) {
                addToCartButton->text = u8"🛒 Добавить в корзину";
            }
            else {
                addToCartButton->text = u8"Нет в наличии😥";
            }

            addToCartButton->callbackData = "add_to_cart_" + productName;
            buttons.push_back(addToCartButton);

            keyboard->inlineKeyboard.push_back(buttons);

            std::string productString = product.getName() + u8"\n<b>Цена:</b> " + std::to_string(product.getPrice()).erase(std::to_string(product.getPrice()).find_last_not_of('0') + 2, std::string::npos) +
                u8" руб.\n<b>Описание:</b>\n" + product.getDescription() + u8"\n<b>Доставка через</b> " + product.getDeliveryInfo() + u8" д.\n" +
                u8"\n<b>Характеристики:</b>\n" + product.getCharacteristics();

            // Отправка сообщения с фотографией, описанием, названием и ценой
            bot.getApi().sendPhoto(query->message->chat->id, product.getImageUrl(), productString, 0, keyboard, "HTML");

            bot.getApi().answerCallbackQuery(query->id);
        }
    }
    else if (query->data == "sort") {
        TgBot::InlineKeyboardMarkup::Ptr filterKeyboard(new TgBot::InlineKeyboardMarkup);

        std::vector<TgBot::InlineKeyboardButton::Ptr> searchButtonRow;
        TgBot::InlineKeyboardButton::Ptr searchButton(new TgBot::InlineKeyboardButton);
        searchButton->text = u8"🔎 Поиск";
        searchButton->callbackData = "search_items";
        searchButtonRow.push_back(searchButton);
        filterKeyboard->inlineKeyboard.push_back(searchButtonRow);

        std::vector<TgBot::InlineKeyboardButton::Ptr> ascButtonRow;
        TgBot::InlineKeyboardButton::Ptr ascButton(new TgBot::InlineKeyboardButton);
        ascButton->text = u8"🔺 По возрастанию цены";
        ascButton->callbackData = "sort_price_asc";
        ascButtonRow.push_back(ascButton);
        filterKeyboard->inlineKeyboard.push_back(ascButtonRow);

        std::vector<TgBot::InlineKeyboardButton::Ptr> descButtonRow;
        TgBot::InlineKeyboardButton::Ptr descButton(new TgBot::InlineKeyboardButton);
        descButton->text = u8"🔻 По убыванию цены";
        descButton->callbackData = "sort_price_desc";
        descButtonRow.push_back(descButton);
        filterKeyboard->inlineKeyboard.push_back(descButtonRow);

        bot.getApi().sendMessage(query->message->chat->id, u8"Выберите метод сортировки или поиска:", false, 0, filterKeyboard);
        bot.getApi().answerCallbackQuery(query->id);
    }
    else if (query->data == "search_items") {
        bot.getApi().sendMessage(query->message->chat->id, u8"Введите ключевое слово для поиска товаров:");
        bot.getApi().answerCallbackQuery(query->id);
    }
    else if (query->data == "sort_price_asc") {
        bot.getApi().deleteMessage(query->message->chat->id, query->message->messageId - 1);
        sortProducts(true);
        bot.getApi().deleteMessage(query->message->chat->id, query->message->messageId);
        handleStart(query->message);
    }
    else if (query->data == "sort_price_desc") {
        bot.getApi().deleteMessage(query->message->chat->id, query->message->messageId - 1);
        sortProducts(false);
        bot.getApi().deleteMessage(query->message->chat->id, query->message->messageId);
        handleStart(query->message);
    }
    else if (query->data == "cancel_search") {
        handleStart(query->message);
        bot.getApi().answerCallbackQuery(query->id);
    }
    else if (query->data == "back_to_catalog") { // назад из товаров
        bot.getApi().deleteMessage(query->message->chat->id, query->message->messageId);
        bot.getApi().answerCallbackQuery(query->id);
        currentState = std::make_shared<CatalogState>(bot, dataBase);
        currentState->handleStart(query->message);
    }

}

void CategoryState::sortProducts(bool ascending) {
    if (ascending) {
        std::sort(products.begin(), products.end(), [](const Product& a, const Product& b) {
            return a.getPrice() < b.getPrice();
            });
    }
    else {
        std::sort(products.begin(), products.end(), [](const Product& a, const Product& b) {
            return a.getPrice() > b.getPrice();
            });
    }
}

void CategoryState::handleMenu(TgBot::Message::Ptr message) {
    if (waitingForKeyword) {
        handleSearch(message); 
    }
}

std::string normalizeString(const std::string& str) {
    // Приведение к нижнему регистру
    std::string normalizedStr = str;
    std::transform(normalizedStr.begin(), normalizedStr.end(), normalizedStr.begin(), ::tolower);

    // Удаление лишних пробелов
    std::regex multipleSpaces("\\s+");
    normalizedStr = std::regex_replace(normalizedStr, multipleSpaces, " ");

    return normalizedStr;
}

void CategoryState::handleSearch(TgBot::Message::Ptr message) {
    std::string keyword = normalizeString(message->text);
    std::vector<Product> filteredProducts;

    // Фильтрация продуктов по ключевому слову
    for (const auto& product : products) {
        std::string productName = normalizeString(product.getName());
        std::string productDescription = normalizeString(product.getDescription());

        if (productName.find(keyword) != std::string::npos || productDescription.find(keyword) != std::string::npos) {
            filteredProducts.push_back(product);
        }
    }

    TgBot::InlineKeyboardMarkup::Ptr keyboard(new TgBot::InlineKeyboardMarkup);

    if (filteredProducts.empty()) {
        bot.getApi().sendMessage(message->chat->id, u8"Ничего не найдено.");
    }
    else {
        // Создание первой строки кнопок "назад" и "отменить фильтр"
        std::vector<TgBot::InlineKeyboardButton::Ptr> firstRow;

        TgBot::InlineKeyboardButton::Ptr backButton(new TgBot::InlineKeyboardButton);
        backButton->text = u8"Назад";
        backButton->callbackData = "back_to_catalog";
        firstRow.push_back(backButton);

        TgBot::InlineKeyboardButton::Ptr cancelButton(new TgBot::InlineKeyboardButton);
        cancelButton->text = u8"Отменить фильтр";
        cancelButton->callbackData = "cancel_search";
        firstRow.push_back(cancelButton);

        keyboard->inlineKeyboard.push_back(firstRow);

        // Создание второго ряда для кнопок с продуктами, по одной кнопке в ряд
        for (const auto& product : filteredProducts) {
            std::vector<TgBot::InlineKeyboardButton::Ptr> productRow;

            TgBot::InlineKeyboardButton::Ptr productButton(new TgBot::InlineKeyboardButton);
            productButton->text = product.getName();
            productButton->callbackData = "product_" + product.getName();
            productRow.push_back(productButton);

            keyboard->inlineKeyboard.push_back(productRow);
        }

        bot.getApi().sendMessage(message->chat->id, u8"Результаты поиска в категории " + category, false, 0, keyboard);
    }

    waitingForKeyword = false;
}