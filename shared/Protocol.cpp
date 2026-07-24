//in the name of ALLAH
//YA MAHDI

#include "Protocol.h"
#include <QVector>

namespace {
    struct CommandEntry {
        Command command;
        QString name;
    };

    const QVector<CommandEntry> commandTable = {
        {Command::SUCCESS, "SUCCESS"},
        {Command::FAIL, "FAIL"},

        {Command::LOGIN, "LOGIN"},
        {Command::SIGNUP_PUBLISHER, "SIGNUP_PUBLISHER"},
        {Command::SIGNUP_NORMALUSER, "SIGNUP_NORMALUSER"},
        {Command::FORGOT_PASS_CHECK, "FORGOT_PASS_CHECK"},
        {Command::FORGOT_PASS_UPDATE, "FORGOT_PASS_UPDATE"},
        {Command::LOGOUT, "LOGOUT"},
        {Command::SET_FAVORITE_GENRES, "SET_FAVORITE_GENRES"},

        {Command::GET_PUBLISHER_BOOKS, "GET_PUBLISHER_BOOKS"},
        {Command::PUBLISHER_BOOKS_RESULT, "PUBLISHER_BOOKS_RESULT"},
        {Command::GET_BOOK_DETAILS, "GET_BOOK_DETAILS"},
        {Command::BOOK_DETAILS_RESULT, "BOOK_DETAILS_RESULT"},
        {Command::ADD_BOOK_METADATA, "ADD_BOOK_METADATA"},
        {Command::ADD_BOOK_RESULT, "ADD_BOOK_RESULT"},
        {Command::UPLOAD_PDF_START, "UPLOAD_PDF_START"},
        {Command::UPLOAD_PDF_CHUNK, "UPLOAD_PDF_CHUNK"},
        {Command::UPLOAD_PDF_END, "UPLOAD_PDF_END"},
        {Command::UPLOAD_PDF_RESULT, "UPLOAD_PDF_RESULT"},
        {Command::EDIT_BOOK, "EDIT_BOOK"},
        {Command::EDIT_BOOK_RESULT, "EDIT_BOOK_RESULT"},
        {Command::TOGGLE_BOOK_STATUS, "TOGGLE_BOOK_STATUS"},
        {Command::TOGGLE_STATUS_RESULT, "TOGGLE_STATUS_RESULT"},
        {Command::DELETE_BOOK, "DELETE_BOOK"},
        {Command::SEARCH_STOREPAGE, "SEARCH_STOREPAGE"},
        {Command::SEARCH_RESULT, "SEARCH_RESULT"},
        {Command::GET_BOOK_SUMMARY, "GET_BOOK_SUMMARY"},
        {Command::BOOK_SUMMARY, "BOOK_SUMMARY"},

        {Command::BUY, "BUY"},
        {Command::GET_CART_ITEMS, "GET_CART_ITEMS"},
        {Command::CART_RESULT, "CART_RESULT"},
        {Command::GET_CART_BOOK_SUMMARY, "GET_CART_BOOK_SUMMARY"},
        {Command::CART_BOOK_SUMMARY, "CART_BOOK_SUMMARY"},
        {Command::ADD_CART, "ADD_CART"},
        {Command::REMOVE_CART, "REMOVE_CART"},
        {Command::CHECKOUT_CART, "CHECKOUT_CART"},
        {Command::CHECKOUT_RESULT, "CHECKOUT_RESULT"},

        {Command::GET_MY_BOOKS, "GET_MY_BOOKS"},
        {Command::MY_BOOKS_RESULT, "MY_BOOKS_RESULT"},
        {Command::GET_SAVED_BOOKS, "GET_SAVED_BOOKS"},
        {Command::SAVED_BOOKS_RESULT, "SAVED_BOOKS_RESULT"},
        {Command::GET_SHELVES, "GET_SHELVES"},
        {Command::SHELVES_RESULT, "SHELVES_RESULT"},
        {Command::GET_SHELF_BOOKS, "GET_SHELF_BOOKS"},
        {Command::SHELF_BOOKS_RESULT, "SHELF_BOOKS_RESULT"},
        {Command::GET_MY_BOOK_INFO, "GET_MY_BOOK_INFO"},
        {Command::MY_BOOK_INFO_RESULT, "MY_BOOK_INFO_RESULT"},
        {Command::GET_SAVED_BOOK_INFO, "GET_SAVED_BOOK_INFO"},
        {Command::SAVED_BOOK_INFO_RESULT, "SAVED_BOOK_INFO_RESULT"},
        {Command::GET_SHELF_BOOK_INFO, "GET_SHELF_BOOK_INFO"},
        {Command::SHELF_BOOK_INFO_RESULT, "SHELF_BOOK_INFO_RESULT"},
        {Command::ADD_SHELF, "ADD_SHELF"},
        {Command::ADD_SHELF_RESULT, "ADD_SHELF_RESULT"},
        {Command::EDIT_SHELF, "EDIT_SHELF"},
        {Command::EDIT_SHELF_RESULT, "EDIT_SHELF_RESULT"},
        {Command::DELETE_SHELF, "DELETE_SHELF"},
        {Command::DELETE_SHELF_RESULT, "DELETE_SHELF_RESULT"},
        {Command::REMOVE_FROM_SAVED, "REMOVE_FROM_SAVED"},
        {Command::REMOVE_SAVED_RESULT, "REMOVE_SAVED_RESULT"},
        {Command::REMOVE_FROM_SHELF, "REMOVE_FROM_SHELF"},
        {Command::REMOVE_FROM_SHELF_RESULT, "REMOVE_FROM_SHELF_RESULT"},
        {Command::ASSIGN_TO_SHELF, "ASSIGN_TO_SHELF"},
        {Command::ASSIGN_SHELF_RESULT, "ASSIGN_SHELF_RESULT"},
        {Command::DOWNLOAD_BOOK, "DOWNLOAD_BOOK"},
        {Command::FILE_START, "FILE_START"},
        {Command::FILE_CHUNK, "FILE_CHUNK"},
        {Command::FILE_END, "FILE_END"},
        {Command::UPDATE_LAST_PAGE, "UPDATE_LAST_PAGE"},
        {Command::SAVE_BOOK, "SAVE_BOOK"},
        {Command::GET_COMMENTS_LIST, "GET_COMMENTS_LIST"},
        {Command::COMMENTS_LIST, "COMMENTS_LIST"},
        {Command::GET_COMMENT_DETAILS, "GET_COMMENT_DETAILS"},
        {Command::COMMENT_DETAILS, "COMMENT_DETAILS"},
        {Command::ADD_COMMENT, "ADD_COMMENT"},
        {Command::EDIT_COMMENT, "EDIT_COMMENT"},
        {Command::DELETE_COMMENT, "DELETE_COMMENT"},
        {Command::COMMENT_UPDATED, "COMMENT_UPDATED"},

        {Command::GET_DATA_USERPROFILE, "GET_DATA_USERPROFILE"},
        {Command::USER_PROFILE_INFO, "USER_PROFILE_INFO"},
        {Command::GET_HISTORY_IDS, "GET_HISTORY_IDS"},
        {Command::HISTORY_IDS, "HISTORY_IDS"},
        {Command::GET_HISTORY_BOOK, "GET_HISTORY_BOOK"},
        {Command::HISTORY_BOOK_INFO, "HISTORY_BOOK_INFO"},
        {Command::UPDATE_BALANCE, "UPDATE_BALANCE"},
        {Command::UPDATE_PROFILE, "UPDATE_PROFILE"},
        {Command::UPDATE_PROFILE_RESULT, "UPDATE_PROFILE_RESULT"},

        {Command::GET_PUB_GENERAL_STATS, "GET_PUB_GENERAL_STATS"},
        {Command::PUB_GENERAL_RESULT, "PUB_GENERAL_RESULT"},
        {Command::GET_PUB_TOP_BOOKS_IDS, "GET_PUB_TOP_BOOKS_IDS"},
        {Command::PUB_TOP_IDS_RESULT, "PUB_TOP_IDS_RESULT"},
        {Command::GET_PUB_BOOK_INFO_TOP, "GET_PUB_BOOK_INFO_TOP"},
        {Command::PUB_BOOK_INFO_TOP_RESULT, "PUB_BOOK_INFO_TOP_RESULT"},
        {Command::GET_PUB_LOWEST_BOOKS_IDS, "GET_PUB_LOWEST_BOOKS_IDS"},
        {Command::PUB_LOWEST_IDS_RESULT, "PUB_LOWEST_IDS_RESULT"},
        {Command::GET_PUB_BOOK_INFO_LOWEST, "GET_PUB_BOOK_INFO_LOWEST"},
        {Command::PUB_BOOK_INFO_LOWEST_RESULT, "PUB_BOOK_INFO_LOWEST_RESULT"},
        {Command::GET_PUB_SALES_CHART, "GET_PUB_SALES_CHART"},
        {Command::PUB_SALES_CHART_RESULT, "PUB_SALES_CHART_RESULT"},
        {Command::GET_PUB_SCORE_CHART, "GET_PUB_SCORE_CHART"},
        {Command::PUB_SCORE_CHART_RESULT, "PUB_SCORE_CHART_RESULT"},

        {Command::SEARCH_USERS, "SEARCH_USERS"},
        {Command::USERS_LIST_RESULT, "USERS_LIST_RESULT"},
        {Command::GET_USER_INFO, "GET_USER_INFO"},
        {Command::USER_INFO_RESULT, "USER_INFO_RESULT"},
        {Command::DELETE_USER, "DELETE_USER"},
        {Command::DELETE_USER_RESULT, "DELETE_USER_RESULT"},
        {Command::BLOCK_USER, "BLOCK_USER"},
        {Command::BLOCK_USER_RESULT, "BLOCK_USER_RESULT"},
        {Command::UNBLOCK_USER, "UNBLOCK_USER"},
        {Command::UNBLOCK_USER_RESULT, "UNBLOCK_USER_RESULT"},
    };
}

QString commandToString(Command command) {
    for (int i = 0; i < commandTable.size(); i++) {
        if (commandTable.at(i).command == command) {
            return commandTable.at(i).name;
        }
    }
    return QString();
}

Command stringToCommand(const QString &text) {
    for (int i = 0; i < commandTable.size(); i++) {
        if (commandTable.at(i).name == text) {
            return commandTable.at(i).command;
        }
    }
    return Command::UNKNOWN;
}
