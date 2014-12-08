#pragma once
#define TRADE_ITEM_MAX 12
struct trade_item_info
{
	unsigned char slot;
	unsigned int count;
};
struct trade_info
{
	struct trade_item_info trade_items[TRADE_ITEM_MAX];
	unsigned int trade_money;
	unsigned char trade_count;
	unsigned char selectslot;
	struct userobject* user;
	unsigned char ok;
};
