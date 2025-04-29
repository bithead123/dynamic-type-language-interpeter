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

uint32_t index = key->hash & (capacity - 1);

  Entry* tombstone = NULL;
  
  for (;;) {
    Entry* entry = &entries[index];

//> find-tombstone
    if (entry->key == NULL) {
      if (IS_NULL(entry->value)) {
        // Empty entry.
        return tombstone != NULL ? tombstone : entry;
      } else {
        // We found a tombstone.
        if (tombstone == NULL) tombstone = entry;
      }
    } else if (entry->key == key) {
      // We found the key.
      return entry;
    }

    index = (index + 1) & (capacity - 1);
  }
}

void adjust_capacity(Hashtable* table, int capacity) {
    Entry* entries = ALLOCATE(Entry, capacity);
    for (int i = 0; i < capacity; i++) {
      entries[i].key = NULL;
      entries[i].value = NULL_VAL;
    }
  //> re-hash
  
  //> resize-init-count
    table->count = 0;
  //< resize-init-count
    for (int i = 0; i < table->capacity; i++) {
      Entry* entry = &table->entries[i];
      if (entry->key == NULL) continue;
  
      Entry* dest = find_entry(entries, capacity, entry->key);
      dest->key = entry->key;
      dest->value = entry->value;
  //> resize-increment-count
      table->count++;
  //< resize-increment-count
    }

    free(table->entries);
    table->entries = entries;
    table->capacity = capacity;
}

bool hashtable_set(Hashtable* table, ObjString* key, Value value) {
    if (table->count + 1 > table->capacity * HASHTABLE_MAX_LOAD_TO_GROW) {
        int capacity = GROW_CAPACITY(table->capacity);
        adjust_capacity(table, capacity);
      }
    
    //< table-set-grow
      Entry* entry = find_entry(table->entries, table->capacity, key);
      bool isNewKey = entry->key == NULL;
    /* Hash Tables table-set < Hash Tables set-increment-count
      if (isNewKey) table->count++;
    */
    //> set-increment-count
      if (isNewKey && IS_NULL(entry->value)) table->count++;
    //< set-increment-count
    
      entry->key = key;
      entry->value = value;
      return isNewKey;
};

void hashtable_copy(Hashtable* from, Hashtable* to) {
    for (int i = 0; i < from->capacity; i++) {
        Entry* e = &from->entries[i];
        if (e->key == NULL) continue;
        hashtable_set(to, e->key, e->value);
    }
};

bool hashtable_get(Hashtable* table, ObjString* key, Value* value) {
    if (table->count == 0) return false;

    Entry* entry = find_entry(table->entries, table->capacity, key);
    if (entry->key == NULL) return false;
  
    *value = entry->value;
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