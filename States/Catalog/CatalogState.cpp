#include "CatalogState.h"
#include "../MainMenu/MainMenuState.h"
#include "../Category/CategoryState.h"

CatalogState::CatalogState(TgBot::Bot& bot, const std::vector<std::string>& categories, const std::vector<Product>& products)
    : bot(bot), categories(categories), products(products) { }

void CatalogState::handleStart(TgBot::Message::Ptr message) {
    auto chatId = message->chat->id;
    TgBot::InlineKeyboardMarkup::Ptr keyboard(new TgBot::InlineKeyboardMarkup);

    for (const auto& category : categories) {
        std::vector<TgBot::InlineKeyboardButton::Ptr> row;
        TgBot::InlineKeyboardButton::Ptr button(new TgBot::InlineKeyboardButton);
        button->text = category;
        button->callbackData = "category_" + category; // Уникальный идентификатор для категории
        row.push_back(button);
        keyboard->inlineKeyboard.push_back(row);
    }

    bot.getApi().sendMessage(chatId, u8"Категории продуктов:", false, 0, keyboard);
}

void CatalogState::handleMenuQ(TgBot::CallbackQuery::Ptr query) {

}

void CatalogState::handleMenu(TgBot::Message::Ptr message) {

}