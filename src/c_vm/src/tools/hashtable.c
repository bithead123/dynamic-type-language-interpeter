#include "hashtable.h"
#include "string.h"

#define HASHTABLE_MAX_LOAD_TO_GROW 0.75

void hashtable_init(Hashtable* t) {
    t->capacity = 0;
    t->count = 0;
    t->entries = NULL;
};

void destroy_hashtable(Hashtable* t) {
    free(t->entries);
    hashtable_init(t);
};

Entry* find_entry(Entry* entries, int capacity, ObjString* key) {
    uint32_t index = key->hash % capacity;
    
    Entry* tombstone = NULL;

    for (;;) {
        Entry* entry = &entries[index];

            if (entry->key == NULL) {
              if (IS_NULL(entry->value)) {
                return tombstone != NULL ? tombstone : entry;
              } else {
                if (tombstone == NULL) tombstone = entry;
              }
            } else if (entry->key == key) {
              return entry;
            }

        index = (index + 1) & (capacity - 1);
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

    t->count=0;

    if (t->entries == NULL) {
        t->entries = entries;
        t->count = 0;
        t->capacity = newCapacity;
        return;
    }

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

    if (t->entries != NULL) {
        free(t->entries);
    }
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

ObjString* hashtable_find_string(Hashtable* t, const char* chars, int length, uint32_t hash) {
    if (t->count <= 0) return NULL;

    uint32_t index = hash % t->capacity;
    for (;;) {
        Entry* en = &t->entries[index];
        if (en->key == NULL) {
            if (IS_NULL(en->value)) return NULL;
        }
        else if (en->key->length == length &&
            en->key->hash == hash &&
            memcmp(en->key->chars, chars, length) == 0) {
                return en->key;
        }

        index = (index + 1) % t->capacity;
    }
};