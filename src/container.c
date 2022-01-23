#include "container.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const String *new_string(const char *source, int length) {
  String *string = calloc(1, sizeof(String));
  string->head = source;
  string->length = length;
  return string;
}

const String *char_to_string(const char *source) {
  return new_string(source, strlen(source));
}

char *string_to_char(const String *source) {
  char *p = calloc(source->length + 1, sizeof(char));
  memcpy(p, source->head, source->length);
  p[source->length] = '\0';
  return p;
}

const String *string_concat(const String *source1, const String *source2) {
  char *p = calloc(source1->length + source2->length, sizeof(char));
  memcpy(p, source1->head, source1->length);
  memcpy(p + source1->length, source2->head, source2->length);
  return new_string(p, source1->length + source2->length);
}

bool string_compare(const String *string1, const String *string2) {
  if (string1->length != string2->length) {
    return false;
  }

  return memcmp(string1->head, string2->head, string1->length) == 0;
}

//
// List
//
ListNode *new_list_node(void *body) {
  ListNode *node = calloc(1, sizeof(ListNode));
  node->body = body;
  return node;
}

ListNode *list_head_to_tail(ListNode *head) {
  while (head->next)
    head = head->next;
  return head;
}

void list_merge(ListNode *list1, ListNode *list2) { list1->next = list2; }

ListNode *list_push_back(ListNode *tail, void *element) {
  ListNode *node = new_list_node(element);
  tail->next = node;
  return node;
}

ListNode *list_push_front(ListNode *head, void *element) {
  ListNode *node = new_list_node(element);
  node->next = head;
  return node;
}

//
// Vector
//
struct Vector {
  void **head;
  int length;
  int reserved;
};

Vector *new_vector(int initialSize) {
  Vector *vector = calloc(1, sizeof(Vector));
  vector->head = calloc(initialSize, sizeof(void *));
  vector->reserved = initialSize;
  return vector;
}

void vector_push_back(Vector *vector, void *element) {
  if (vector->length >= vector->reserved) {
    void **old = vector->head;
    void **new = calloc(vector->reserved * 2, sizeof(void *));
    for (int i = 0; i < vector->length; i++) {
      new[i] = old[i];
    }
    vector->head = new;
    vector->reserved *= 2;
  }

  vector->head[vector->length] = element;
  vector->length++;
}

void *vector_get(Vector *vector, int index) {
  if (index < 0 || index >= vector->length) {
    ERROR("配列外参照エラー");
  }

  return vector->head[index];
}
void vector_set(Vector *vector, int index, void *element) {
  if (index < 0 || index >= vector->length) {
    ERROR("配列外参照エラー");
  }

  vector->head[index] = element;
}

int vector_length(Vector *vector) { return vector->length; }

//
// HashTable
//
struct HashTable {
  ListNode *table[256];
};

int hash(const String *source) {
  int sum = 0;
  for (int i = 0; i < source->length; i++) {
    sum += *(source->head + i);
  }
  return sum % 256;
}

typedef struct KeyValuePair KeyValuePair;
struct KeyValuePair {
  const String *key;
  void *value;
};

KeyValuePair *new_key_value_pair(const String *key, void *value) {
  KeyValuePair *pair = calloc(1, sizeof(KeyValuePair));
  pair->key = key;
  pair->value = value;
  return pair;
}

HashTable *new_hash_table(void) { return calloc(1, sizeof(HashTable)); }

void hash_table_store(HashTable *table, const String *key, void *data) {
  const int keyhash = hash(key);

  ListNode head;
  ListNode *node = &head;
  node->next = table->table[keyhash];
  while (node->next) {
    node = node->next;
  }
  KeyValuePair *pair = new_key_value_pair(key, data);
  list_push_back(node, pair);

  //あるkeyhashに初めて要素が追加されたとき、table[keyhash]は空なので更新する
  //初めてでないときに更新しても問題ないので場合分けはしない
  table->table[keyhash] = head.next;
}

bool hash_table_contain(HashTable *table, const String *key) {
  return hash_table_find(table, key) != NULL;
}

void *hash_table_find(HashTable *table, const String *key) {
  const int keyhash = hash(key);
  ListNode *node = table->table[keyhash];
  while (node) {
    KeyValuePair *pair = node->body;
    if (string_compare(pair->key, key)) {
      return pair->value;
    }
    node = node->next;
  }
  return NULL;
}
