//in the name of ALLAH
//YA MAHDI

#ifndef PROTOCOL_H
#define PROTOCOL_H

const int REQ_LOGIN = 101;
const int REQ_REGISTER = 102;
const int REQ_PUBLISH = 103;
const int REQ_BUY = 104;
const int REQ_SEARCH = 105;
const int REQ_LOGOUT = 106;
const int REQ_FORGOT_PASSWORD = 107;
const int REQ_RESET_PASSWORD = 108;
const int REQ_ADD_REVIEW = 109;
const int REQ_EDIT_REVIEW = 110;
const int REQ_ADD_TO_CART = 111;
const int REQ_REMOVE_FROM_CART = 112;
const int REQ_CREATE_SHELF = 113;
const int REQ_ADD_BOOK_TO_SHELF = 114;
const int REQ_SAVE_BOOK = 115;
const int REQ_UPDATE_BOOK = 116;
const int REQ_DEACTIVATE_BOOK = 117;
const int REQ_BLOCK_USER = 118;
const int REQ_UNBLOCK_USER = 119;
const int REQ_DELETE_BOOK = 120;
const int REQ_DELETE_REVIEW = 121;
const int REQ_SET_FAVORITE_GENRES = 122;
const int REQ_CHECKOUT_CART = 123;

const int RES_SUCCESS = 201;
const int RES_FAIL = 202;
const int RES_LOGIN_OK = 203;
const int RES_REGISTER_OK = 204;
const int RES_SEARCH_RESULT = 205;
const int RES_USER_BLOCKED = 206;

const int NOTIFY_NEW_EVENT = 301;
const int NOTIFY_NEW_BOOK = 302;
const int NOTIFY_PURCHASE_MADE = 303;
const int NOTIFY_REVIEW_UPDATED = 304;

#endif
