// MainMenuState.cpp

#include "MainMenuState.h"
#include "../Catalog/CatalogState.h"
#include <tgbot/tgbot.h>
#include <iostream>

// Стартовое меню
StartState::StartState(TgBot::Bot& bot) : bot(bot) { }

void StartState::handleStart(TgBot::Message::Ptr message) {
    auto chatId = message->chat->id;
    TgBot::InlineKeyboardMarkup::Ptr keyboard(new TgBot::InlineKeyboardMarkup);
    std::vector<TgBot::InlineKeyboardButton::Ptr> row0;
    row0.push_back(TgBot::InlineKeyboardButton::Ptr(new TgBot::InlineKeyboardButton));
    row0[0]->text = u8"📝 Каталог";
    row0[0]->callbackData = "catalog";
    std::vector<TgBot::InlineKeyboardButton::Ptr> row1;
    row1.push_back(TgBot::InlineKeyboardButton::Ptr(new TgBot::InlineKeyboardButton));
    row1[0]->text = u8"🛒 Моя корзина";
    row1[0]->callbackData = "cart";
    std::vector<TgBot::InlineKeyboardButton::Ptr> row2;
    row2.push_back(TgBot::InlineKeyboardButton::Ptr(new TgBot::InlineKeyboardButton));
    row2[0]->text = u8"❓ Тех. поддержка";
    row2[0]->callbackData = "support";
    keyboard->inlineKeyboard.push_back(row0);
    keyboard->inlineKeyboard.push_back(row1);
    keyboard->inlineKeyboard.push_back(row2);

    auto sentMessage = bot.getApi().sendMessage(chatId, u8"Привет! Выберите одну из опций ниже:", false, 0, keyboard);

    // Проверка на отсутствие закрепленного сообщения перед закреплением свежего.
    if (!bot.getApi().getChat(chatId)->pinnedMessage) {
        bot.getApi().pinChatMessage(chatId, sentMessage->messageId);
    }
}

void StartState::handleMenu(TgBot::Message::Ptr message) {
}

void StartState::handleMenuQ(TgBot::CallbackQuery::Ptr query, std::shared_ptr<TelegramState>& currentState, DataBase& dataBase)
{
    if (query->data == "support") { // поддержка
        this->bot.getApi().sendMessage(query->message->chat->id, u8"Техническая поддержка: @support");
        bot.getApi().answerCallbackQuery(query->id);
    }
}

