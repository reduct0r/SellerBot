#include "CategoryState.h"
#include "../MainMenu/MainMenuState.h"

CategoryState::CategoryState(TgBot::Bot& bot, const std::string& category, const std::vector<Product>& products)
    : bot(bot), category(category), products(products) { }

void CategoryState::handleStart(TgBot::Message::Ptr message) {
    auto chatId = message->chat->id;
    TgBot::InlineKeyboardMarkup::Ptr keyboard(new TgBot::InlineKeyboardMarkup);

    std::vector<TgBot::InlineKeyboardButton::Ptr> row1;

    TgBot::InlineKeyboardButton::Ptr button1(new TgBot::InlineKeyboardButton);
    TgBot::InlineKeyboardButton::Ptr button2(new TgBot::InlineKeyboardButton);

    button1->text = u8"Назад в каталог";
    button1->callbackData = "back_to_catalog";
    button2->text = u8"🔍 Фильтры";
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

void CategoryState::handleMenuQ(TgBot::CallbackQuery::Ptr query)
{
    if (query->data.rfind(u8"product_", 0) == 0) {
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
            addToCartButton->text = u8"🛒 Добавить в корзину";
            addToCartButton->callbackData = "add_to_cart_" + productName;
            buttons.push_back(addToCartButton);

            keyboard->inlineKeyboard.push_back(buttons);

            // Отправка сообщения с фотографией, описанием, названием и ценой
            bot.getApi().sendPhoto(query->message->chat->id, product.getImageUrl(),
                product.getName() + "\n" + product.getDescription() + u8"\nЦена: " + std::to_string(product.getPrice()), 0, keyboard);

            bot.getApi().answerCallbackQuery(query->id);
        }
    }
    else if (query->data == "sort") {
        TgBot::InlineKeyboardMarkup::Ptr filterKeyboard(new TgBot::InlineKeyboardMarkup);
        std::vector<TgBot::InlineKeyboardButton::Ptr> filterButtons;

        TgBot::InlineKeyboardButton::Ptr ascButton(new TgBot::InlineKeyboardButton);
        ascButton->text = u8"🔺 По возрастанию цены";
        ascButton->callbackData = "sort_price_asc";

        TgBot::InlineKeyboardButton::Ptr descButton(new TgBot::InlineKeyboardButton);
        descButton->text = u8"🔻 По убыванию цены";
        descButton->callbackData = "sort_price_desc";

        filterButtons.push_back(ascButton);
        filterButtons.push_back(descButton);
        filterKeyboard->inlineKeyboard.push_back(filterButtons);

        bot.getApi().sendMessage(query->message->chat->id, u8"Выберите метод сортировки:", false, 0, filterKeyboard);
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

}

void CategoryState::handleMenu(TgBot::Message::Ptr message) {

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