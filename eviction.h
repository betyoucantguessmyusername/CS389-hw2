//By Monica Moniot and Alyssa Riceman
#ifndef EVICTION_H
#define EVICTION_H
#include "cache.h"
#include "book.h"
#include "types.h"



void create_evictor(Evictor* evictor, evictor_type policy);
constexpr Index get_evictor_mem_size(evictor_type policy, Index entry_capacity) {
	if(policy == FIFO or policy == LIFO or policy == LRU or policy == MRU or policy == CLOCK or policy == SLRU) {
		return 0;
	} else {//RANDOM
		return sizeof(Index)*entry_capacity;
	}
}

void add_evict_item    (Evictor* evictor, Bookmark item_i, Evict_item* item, Book* book);
void remove_evict_item (Evictor* evictor, Bookmark item_i, Evict_item* item, Book* book);
void touch_evict_item  (Evictor* evictor, Bookmark item_i, Evict_item* item, Book* book);
Bookmark get_evict_item(Evictor* evictor, Book* book);//also removes item
#endif
