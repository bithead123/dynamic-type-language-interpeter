#include "hashtable.h"

#define HASHTABLE_MAX_LOAD_TO_GROW 0.75

void init_hashtable(Hashtable* t) {
    t->capacity = 0;
    t->count = 0;
    t->entries = NULL;
};

void destroy_hashtable(Hashtable* t) {
    free(t->entries);
    init_hashtable(t);
};

Entry* find_entry(Entry* entries, int capacity, ObjString* key) {
    uint32_t index = key->hash % capacity;
    
    Entry* tombstone = NULL;

    for (;;) {
        Entry* entry = &entries[index];

            if (entry->key == NULL) {
              if (IS_NIL(entry->value)) {
                return tombstone != NULL ? tombstone : entry;
              } else {
                if (tombstone == NULL) tombstone = entry;
              }
            } else if (entry->key == key) {
              return entry;
            }
    }
}

void adjust_capacity(Hashtable* t, int newCapacity) {
    // free mem
    // recreate elems

    // create new row entries as nulls
    Entry* entries = ALLOCATE(Entry, newCapacity);
    for (int i = 0; i < newCapacity; i++) {
        entries[i].key = NULL;
        entries[i].value = NULL_VAL;
    }

    t->count++;

    // put values to new entries from old table
    for (int i = 0; i < newCapacity; i++) {
        Entry* e = &t->entries[i];
        if (e->key == NULL) continue;
        // re-set value
        Entry* dest = find_entry(entries, newCapacity, e->key);
        dest->key = e->key;
        dest->value = e->value;
        t->count++;
    }

    free(t->entries);
    t->capacity = newCapacity;
    t->entries = entries;
}

bool hashtable_set(Hashtable* t, ObjString* key, Value value) {
    if (t->count + 1 > t->capacity * HASHTABLE_MAX_LOAD_TO_GROW) {
        int capacity = GROW_CAPACITY(t->capacity);
        adjust_capacity(t, capacity);
    }

    Entry* entry = find_entry(t->entries, t->capacity, key);
    bool is_new = entry->key == NULL;
    if (is_new && IS_NULL(entry->value)) t->count++;

    entry->key = key;
    entry->value = value;
    return is_new;
};

void hashtable_copy(Hashtable* from, Hashtable* to) {
    for (int i = 0; i < from->capacity; i++) {
        Entry* e = &from->entries[i];
        if (e->key == NULL) continue;
        hashtable_set(to, e->key, e->value);
    }
};

bool hashtable_get(Hashtable* t, ObjString* key, Value* value) {
    if (t->count <= 0) return false;

    Entry* find = find_entry(t->entries, t->capacity, key);
    if (find->key == NULL) return false;
    
    *value = find->value;
    return true;
};

bool hashtable_delete(Hashtable* t, ObjString* key) {
    if (t->count <= 0) return false;

    Entry* en = find_entry(t->entries, t->capacity, key);
    if (en->key == NULL) return false;

    en->key = NULL;
    en->value = BOOL_VAl(true);
    return true;
};