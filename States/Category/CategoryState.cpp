#include "CategoryState.h"
#include "../MainMenu/MainMenuState.h"

CategoryState::CategoryState(TgBot::Bot& bot, const std::string& category, const std::vector<Product>& products)
    : bot(bot), category(category), products(products) { }

void CategoryState::handleStart(TgBot::Message::Ptr message) {
    auto chatId = message->chat->id;
    TgBot::InlineKeyboardMarkup::Ptr keyboard(new TgBot::InlineKeyboardMarkup);

    for (const auto& product : products) {
        if (product.getCategory() == category) {
            std::vector<TgBot::InlineKeyboardButton::Ptr> row;
            TgBot::InlineKeyboardButton::Ptr button(new TgBot::InlineKeyboardButton);
            button->text = product.getName();
            button->callbackData = "product_" + product.getName(); // Уникальный идентификатор для продукта
            row.push_back(button);
            keyboard->inlineKeyboard.push_back(row);
        }
    }

    bot.getApi().sendMessage(chatId, u8"Продукты в категории: " + category, false, 0, keyboard);
}

void CategoryState::handleMenu(TgBot::Message::Ptr message) {
    // Реализуйте это, если нужно навигировать внутри меню.
}