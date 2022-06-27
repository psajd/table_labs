
#ifndef UNTITLED2_TABLE_H
#define UNTITLED2_TABLE_H

#include <stdio.h>
#include <stdlib.h>
#pragma pack(push, 1)
typedef struct Item {
    int key1_index, key2_index ;
    int str_len;
} Item;
#pragma pack(pop)

typedef struct KeySpace1 {
    int busy;
    int key;
    int item_offset;
} KeySpace1;

typedef struct KeySpace2 {
    int busy;
    int key;
    int item_offset;
} KeySpace2;

typedef struct Table {
    int ks1_offset;
    int ks2_offset;
    int msize1, msize2;
    int csize1, csize2;
    char *filename;
    FILE *file;

} Table;

int file_size(FILE *fp);

int file_check(FILE *a);

int search_ks1(Table *table, int k);

int delete_ks1(Table *table, int k);

int insert_ks1(Table *table, int k, Item *item);

int search_ks2(Table *table, int k);

int delete_ks2(Table *table, int k);

int insert_ks2(Table *table, int k, Item *item);

int table_destroyer(Table *table);

struct Table table_initializer(int size1, int size2, const char *filename);

int insert(Table *table, char *str, int k1, int k2);

char *find(Table *table, int k1, int k2);

char *find_all(Table *table, int k);

int delete(Table *table, int k1, int k2);

int delete_all(Table *table, int k, int ks);

void print_table(Table table);

struct Table find_range(Table *table, int l, int r);

char* get_str(Table *table, int offset, int ks);

#endif //UNTITLED2_TABLE_H
