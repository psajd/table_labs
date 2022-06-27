

#ifndef UNTITLED2_UTILS_H
#define UNTITLED2_UTILS_H
#include <stdio.h>
#include "Table.h"
#include <string.h>


void print_info();
int insert_in(struct Table *table);
int find_in(struct Table *table);
int delete_in(struct Table *table);
int find_all_in(struct Table *table);
int delete_all_in(struct Table* table);
int range_in(struct Table *table);
int cli_read_int(int *value, int  size);
int cli_read_str(char *str, int  size);
#endif //UNTITLED2_UTILS_H
