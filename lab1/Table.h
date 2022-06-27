#ifndef LAV_TABLE_H
#define LAV_TABLE_H

#include <stdio.h>
#include <stdlib.h>

struct Item
{
    char *info;
    int key1, key2;
};

struct KeySpace1
{
    int key;
    struct Item *item;
    int busy;
};

struct KeySpace2
{
    int busy;
    int key;
    struct Item *item;
};

struct Table
{
    struct KeySpace1 *ks1;
    struct KeySpace2 *ks2;
    int msize1, msize2;
    int csize1, csize2;
};

int search_ks1(struct Table *table, int k);

int delete_ks1(struct Table *table, int k);

int insert_ks1(struct Table *table, int k, struct Item* item);

int search_ks2(struct Table *table, int k);

int delete_ks2(struct Table *table, int k);

int insert_ks2(struct Table *table, int k, struct Item* item);

int table_destroyer(struct Table *table);

struct Table table_initializer(int size1, int size2);

int insert(struct Table *table, char *str, int k1, int k2);

char* find(struct Table *table, int k1, int k2);

char* find_all(struct Table *table, int k);

int delete(struct Table *table, int k1, int k2);

int delete_all(struct Table *table, int k, int ks);

void print_table(struct Table table);

struct Table find_range(struct Table* table, int l, int r);

#endif //LAV_TABLE_H