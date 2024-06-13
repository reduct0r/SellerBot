// MainMenuState.cpp

#include "MainMenuState.h"
#include "../Catalog/CatalogState.h"
#include <tgbot/tgbot.h>
#include <iostream>


StartState::StartState(TgBot::Bot& bot) : bot(bot) { }

void StartState::handleStart(TgBot::Message::Ptr message) {
    auto chatId = message->chat->id;
    //bot.getApi().sendMessage(chatId, u8"Привет! Выберите одну из опций ниже:", false, 0, TgBot::InlineKeyboardMarkup::Ptr(
    //    new TgBot::InlineKeyboardMarkup()), "HTML");

    TgBot::InlineKeyboardMarkup::Ptr keyboard(new TgBot::InlineKeyboardMarkup);
    std::vector<TgBot::InlineKeyboardButton::Ptr> row0;
    row0.push_back(TgBot::InlineKeyboardButton::Ptr(new TgBot::InlineKeyboardButton));
    row0[0]->text = u8"Каталог";
    row0[0]->callbackData = "catalog";
    std::vector<TgBot::InlineKeyboardButton::Ptr> row1;
    row1.push_back(TgBot::InlineKeyboardButton::Ptr(new TgBot::InlineKeyboardButton));
    row1[0]->text = u8"Моя корзина";
    row1[0]->callbackData = "cart";
    std::vector<TgBot::InlineKeyboardButton::Ptr> row2;
    row2.push_back(TgBot::InlineKeyboardButton::Ptr(new TgBot::InlineKeyboardButton));
    row2[0]->text = u8"Тех. поддержка";
    row2[0]->callbackData = "support";
    keyboard->inlineKeyboard.push_back(row0);
    keyboard->inlineKeyboard.push_back(row1);
    keyboard->inlineKeyboard.push_back(row2);

    bot.getApi().sendMessage(chatId, u8"Привет! Выберите одну из опций ниже:", false, 0, keyboard);
}

void StartState::handleMenu(TgBot::Message::Ptr message) {
    handleStart(message); // просто вызываем тот же метод, что и при /start
}

void StartState::handleMenuQ(TgBot::CallbackQuery::Ptr query)
{
}

MenuState::MenuState(TgBot::Bot& bot) : bot(bot) { }

void MenuState::handleStart(TgBot::Message::Ptr message) {
    // Handle menu options if needed
}

void MenuState::handleMenu(TgBot::Message::Ptr message) {
    handleStart(message); // просто вызываем тот же метод, что и при /start
}

void MenuState::handleMenuQ(TgBot::CallbackQuery::Ptr query)
{
}
