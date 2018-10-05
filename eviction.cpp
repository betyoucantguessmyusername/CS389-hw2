#pragma once
//By Monica Moniot
#include <stdlib.h>
#include <cstring>
#include <cassert>
#include "eviction.h"


inline Evict_item* get_evict_item(Evict_item_locator loc, Index item_i) {
	return static_cast<Evict_item*>(loc.abs_first + item_i*loc.step_size);
}


Index INVALID_NODE = -1;
Node* get_node(Evict_item_locator loc, Index item_i) {
	return &get_evict_item(loc, item_i)->node;
}
void remove(DLL* list, Index item_i, Node* node, Evict_item_locator loc) {
	auto next_i = node->next;
	auto pre_i = node->pre;

	if(pre_i != INVALID_NODE) {
		get_node(loc, pre_i)->next = next_i;
	}
	if(next_i != INVALID_NODE) {
		get_node(loc, next_i)->pre = pre_i;
	}

	if(list->first == item_i) {
		list->first = next_i;
	}
	if(list->last == item_i) {
		list->last = pre_i;
	}
}
void append(DLL* list, Index item_i, Node* node, Evict_item_locator loc) {
	Index last = list->last;
	if(last == INVALID_NODE) {
		list->first = item_i;
	} else {
		Node* last_node = get_node(loc, last);
		last_node->next = item_i;
	}
	node->pre = last;
	node->next = INVALID_NODE;
	list->last = item_i;
}
void set_last(DLL* list, Index item_i, Node* node, Evict_item_locator loc) {
	auto last = list->first;
	if(last == list->first or item_i == list->last) {
		return;
	} else if(item_i == list->first) {
		list->first = last;
	}

	auto last_node = get_node(loc, last);
	auto next_i = node->next;
	auto pre_i = node->pre;
	auto pre_i1 = last_node->pre;

	last_node->next = next_i;
	last_node->pre = pre_i;
	if(pre_i != INVALID_NODE) {
		 get_node(loc, pre_i)->next = last;
	}
	if(next_i != INVALID_NODE) {
		 get_node(loc, next_i)->pre = last;
	}

	node->next = INVALID_NODE;
	node->pre = pre_i1;
	if(pre_i1 != INVALID_NODE) {
		get_node(loc, pre_i1)->next = item_i;
	}
}
void update(DLL* list, Index item_i, Node* node, Index new_i, Evict_item_locator loc) {
	auto next_i = node->next;
	auto pre_i = node->pre;
	if(pre_i != INVALID_NODE) {
		get_node(loc, pre_i)->next = new_i;
	}
	if(next_i != INVALID_NODE) {
		get_node(loc, next_i)->pre = new_i;
	}
}


void create_evictor(Evictor* evictor, evictor_type policy, Index init_capacity) {
	evictor->policy = policy;
	if(policy == FIFO) {
		auto list = &evictor->data.list;
		list->last = INVALID_NODE;
		list->first = INVALID_NODE;
	} else if(policy == LRU) {
		auto list = &evictor->data.list;
		list->last = INVALID_NODE;
		list->first = INVALID_NODE;
	} else {//RANDOM
		auto rand_data = &evictor->data.rand_data;
		rand_data->rand_items = new Index[init_capacity];
		rand_data->total_items = 0;
	}
}
void grow_evictor(Evictor* evictor, Index pre_size, Index new_size) {
	auto policy = evictor->policy;
	if(policy == FIFO) {

	} else if(policy == LRU) {

	} else {//RANDOM
		auto data = &evictor->data.rand_data;
		auto new_items = new Index[new_size];
		memcpy(new_items, data->rand_items, pre_size);
		delete data->rand_items;
		data->rand_items = new_items;
	}
}
void delete_evictor(Evictor* evictor) {
	auto policy = evictor->policy;
	if(policy == FIFO) {

	} else if(policy == LRU) {

	} else {//RANDOM
		auto data = &evictor->data.rand_data;
		delete data->rand_items;
		data->rand_items = NULL;
		data->total_items = 0;
	}
}

Index evict_item(Evictor* evictor, Evict_item_locator loc) {//return item to evict
	auto policy = evictor->policy;
	Index item_i = 0;
	if(policy == FIFO) {
		auto list = &evictor->data.list;
		item_i = list->first;
		remove(list, item_i, get_node(loc, item_i), loc);
	} else if(policy == LRU) {
		auto list = &evictor->data.list;
		item_i = list->first;
		remove(list, item_i, get_node(loc, item_i), loc);
	} else {//RANDOM
		auto data = &evictor->data.rand_data;
		auto rand_items = data->rand_items;
		auto rand_i0 = rand()%data->total_items;
		item_i = rand_items[rand_i0];
		//We need to delete rand_i0 from rand_items in place
		//this requires us to relink some data objects
		auto rand_i1 = data->total_items - 1;
		data->total_items = rand_i1;
		auto item_i1 = get_evict_item(loc, rand_items[rand_i1]);
		rand_items[rand_i0] = rand_items[rand_i1];
		item_i1->rand_i = rand_i0;
	}
	return item_i;
}

void add_item(Evictor* evictor, Index item_i, Evict_item* item, Evict_item_locator loc) {//item was created
	//we must init "item"
	auto policy = evictor->policy;
	if(policy == FIFO) {
		auto node = &item->node;
		append(&evictor->data.list, item_i, node, loc);
	} else if(policy == LRU) {
		auto node = &item->node;
		append(&evictor->data.list, item_i, node, loc);
	} else {//RANDOM
		auto data = &evictor->data.rand_data;
		auto rand_items = data->rand_items;
		item->rand_i = data->total_items;
		rand_items[data->total_items] = item_i;
		data->total_items += 1;
	}
}

void remove_item(Evictor* evictor, Index item_i, Evict_item* item, Evict_item_locator loc) {//item was removed
	auto policy = evictor->policy;
	if(policy == FIFO) {
		auto node = &item->node;
		remove(&evictor->data.list, item_i, node, loc);
	} else if(policy == LRU) {
		auto node = &item->node;
		remove(&evictor->data.list, item_i, node, loc);
	} else {//RANDOM
		auto data = &evictor->data.rand_data;
		auto rand_items = data->rand_items;
		//We need to delete from rand_items in place
		//this requires us to relink some data objects
		Index rand_i0 = item->rand_i;
		auto rand_i1 = data->total_items - 1;
		data->total_items = rand_i1;
		auto item_i1 = get_evict_item(loc, rand_items[rand_i1]);
		rand_items[rand_i0] = rand_items[rand_i1];
		item_i1->rand_i = rand_i0;
	}
}

void touch_item(Evictor* evictor, Index item_i, Evict_item* item, Evict_item_locator loc) {//item was touched
	auto policy = evictor->policy;
	if(policy == FIFO) {

	} else if(policy == LRU) {
		auto node = &item->node;
		set_last(&evictor->data.list, item_i, node, loc);
	} else {//RANDOM

	}
}

void update_item(Evictor* evictor, Index item_i, Evict_item* item, Index new_i, Evict_item_locator loc) {//item was moved
	auto policy = evictor->policy;
	if(policy == FIFO) {
		auto node = &item->node;
		update(&evictor->data.list, item_i, node, new_i, loc);
	} else if(policy == LRU) {
		auto node = &item->node;
		update(&evictor->data.list, item_i, node, new_i, loc);
	} else {//RANDOM
		//relink
		auto data = &evictor->data.rand_data;
		auto rand_items = data->rand_items;
		rand_items[item->rand_i] = new_i;
	}
}
