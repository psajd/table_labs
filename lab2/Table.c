#include "Table.h"
#include <string.h>

struct Table table_initializer(int size1, int size2, const char *filename) {
    struct Table table;
    table.msize1 = size1;
    table.msize2 = size2;
    table.csize1 = 0;
    table.csize2 = 0;
    table.ks1_offset = 0;
    table.ks2_offset = (int) sizeof(KeySpace1) * size1;
    int filename_length = (int) strlen(filename);
    table.filename = malloc(filename_length + 1);
    strcpy(table.filename, filename);
    table.file = fopen(table.filename, "wb+");

    struct KeySpace1 *keySpace1 = malloc(sizeof(KeySpace1) * size1);
    for (int i = 0; i < size1; i++) {
        keySpace1[i].key = 0;
        keySpace1[i].item_offset = 0;
        keySpace1[i].busy = 0;
    }
    fwrite(keySpace1, sizeof(KeySpace1), size1, table.file);
    free(keySpace1);

    struct KeySpace2 *keySpace2 = malloc(sizeof(KeySpace2) * size2);
    for (int i = 0; i < size2; i++) {
        keySpace2[i].key = 0;
        keySpace2[i].item_offset = 0;
        keySpace2[i].busy = 0;
    }
    fwrite(keySpace2, sizeof(KeySpace2), size2, table.file);
    free(keySpace2);

    return table;
}

int file_check(FILE *a) {
    if (!a) {
        fprintf(stderr, "Can't open file");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

int insert(struct Table *table, char *str, int k1, int k2) {
    if ((search_ks1(table, k1) != -1) || (search_ks2(table, k2) != -1)) {
        free(str);
        fprintf(stderr, "Error: The table key is busy\n");
        return -1;
    }
    struct Item *temp = malloc(sizeof(struct Item));
    if (insert_ks1(table, k1, temp) == EXIT_SUCCESS && insert_ks2(table, k2, temp) == EXIT_SUCCESS) {
        fseek(table->file, file_size(table->file), SEEK_SET);
        temp->str_len = (int) strlen(str) + 1;
        fwrite(temp, sizeof(Item), 1, table->file);
        fwrite(str, sizeof(char), temp->str_len, table->file);
        free(str);
        free(temp);
        return EXIT_SUCCESS;
    } else {
        free(temp);
        free(str);
        fprintf(stderr, "Error: table overflow\n");
        return -2;
    }
}

char *get_str(Table *table, int offset, int ks) {
    if (ks == 1) {
        KeySpace1 *temp_ks1 = malloc(sizeof(KeySpace1));
        fseek(table->file, offset, SEEK_SET);
        fread(temp_ks1, sizeof(KeySpace1), 1, table->file);
        Item *temp_item = malloc(sizeof(Item));
        fseek(table->file, temp_ks1->item_offset, SEEK_SET);
        fread(temp_item, sizeof(KeySpace1), 1, table->file);
        char *str = malloc(sizeof(char) * temp_item->str_len);
        fread(str, sizeof(char), temp_item->str_len, table->file);
        free(temp_item);
        free(temp_ks1);
        return str;
    } else {
        KeySpace2 *temp_ks2 = malloc(sizeof(KeySpace2));
        fseek(table->file, offset, SEEK_SET);
        fread(temp_ks2, sizeof(KeySpace2), 1, table->file);
        Item *temp_item = malloc(sizeof(Item));
        fseek(table->file, temp_ks2->item_offset, SEEK_SET);
        fread(temp_item, sizeof(KeySpace2), 1, table->file);
        char *str = malloc(sizeof(char) * temp_item->str_len);
        fread(str, sizeof(char), temp_item->str_len, table->file);
        free(temp_item);
        free(temp_ks2);
        return str;
    }
}

char *find(struct Table *table, int k1, int k2) {
    int l1 = search_ks1(table, k1), l2 = search_ks2(table, k2);
    if ((l1 != -1) && (l2 != -1)) {
        char *str1 = get_str(table, l1, 1);
        char *str2 = get_str(table, l2, 2);
        if (!strcmp(str1, str2)) {
            free(str2);
            return str1;
        }
    }
    fprintf(stderr, "Error: there is no element in the table\n");
    return NULL;
}

char *find_all(struct Table *table, int k) {
    int l1 = search_ks1(table, k), l2 = search_ks2(table, k);
    if ((l1 == -1) && (l2 == -1)) {
        fprintf(stderr, "Error: there is no element in the table\n");
        return NULL;
    }
    char *str1, *str2;
    int str_len1 = 0, str_len2 = 0;
    if (l1 != -1) {
        str1 = get_str(table, l1, 1);
        str_len1 = (int) strlen(str1);
    }
    if (l2 != -1) {
        str2 = get_str(table, l2, 2);
        str_len2 = (int) strlen(str2);
    }
    char *result = malloc((str_len1 + str_len2 + 2) * sizeof(char));
    if (str_len1) {
        strcpy(result, str1);
        if (str_len2) {
            strcat(result, " ");
            strcat(result, str2);
        }
    } else {
        if (str_len2) {
            strcpy(result, str2);
        }
    }
    if (str_len1)
        free(str1);
    if (str_len2)
        free(str2);
    return result;
}

int delete(struct Table *table, int k1, int k2) {
    int l1 = search_ks1(table, k1), l2 = search_ks2(table, k2);
    if ((l1 != -1) && (l2 != -1)) {
        char *str1 = get_str(table, l1, 1);
        char *str2 = get_str(table, l2, 2);
        if (!strcmp(str1, str2)) {
            free(str1);
            free(str2);
            delete_ks1(table, k1);
            delete_ks2(table, k2);
            return EXIT_SUCCESS;
        }
    }
    fprintf(stderr, "Error: there is no element in the table\n");
    return -1;
}

int delete_all(struct Table *table, int k, int ks) {
    if (ks == 1) {
        int l1 = k;
        KeySpace1 *temp_ks1 = malloc(sizeof(KeySpace1));
        fseek(table->file, search_ks1(table, k), SEEK_SET);
        fread(temp_ks1, sizeof(KeySpace1), 1, table->file);

        Item *temp_item = malloc(sizeof(Item));
        fseek(table->file, temp_ks1->item_offset, SEEK_SET);
        fread(temp_item, sizeof(Item), 1, table->file);

        KeySpace2 *temp_ks2 = malloc(sizeof(KeySpace2));
        fseek(table->file, temp_item->key2_index * (int) sizeof(KeySpace2) + table->msize1 * (int) sizeof(KeySpace1),
              SEEK_SET);
        fread(temp_ks2, sizeof(KeySpace2), 1, table->file);

        int l2 = temp_ks2->key;
        free(temp_ks1);
        free(temp_ks2);
        free(temp_item);

        return delete(table, l1, l2);
    } else {
        int l2 = k;
        KeySpace2 *temp_ks2 = malloc(sizeof(KeySpace2));
        fseek(table->file, search_ks2(table, k), SEEK_SET);
        fread(temp_ks2, sizeof(KeySpace2), 1, table->file);

        Item *temp_item = malloc(sizeof(Item));
        fseek(table->file, temp_ks2->item_offset, SEEK_SET);
        fread(temp_item, sizeof(Item), 1, table->file);

        KeySpace1 *temp_ks1 = malloc(sizeof(KeySpace1));
        fseek(table->file, temp_item->key1_index * (int) sizeof(KeySpace1), SEEK_SET);
        fread(temp_ks1, sizeof(KeySpace1), 1, table->file);

        int l1 = temp_ks1->key;
        free(temp_ks1);
        free(temp_ks2);
        free(temp_item);

        return delete(table, l1, l2);
    }
}

void print_table(struct Table table) {
    printf("first keyspace");
    printf("\nKey   |   Value\n");
    for (int i = table.ks1_offset; i < table.ks2_offset; i += sizeof(struct KeySpace1)) {
        fseek(table.file, i, SEEK_SET);
        KeySpace1 *temp = malloc(sizeof(KeySpace1));
        fread(temp, sizeof(KeySpace1), 1, table.file);
        if (temp->busy) {
            char *str = get_str(&table, i, 1);
            printf("%d %15s\n", temp->key, str);
            free(str);
        }
        free(temp);
    }

    printf("\nsecond keyspace");
    printf("\nKey   |   Value\n");
    for (int i = table.ks2_offset;
         i < table.msize2 * sizeof(KeySpace2) + table.ks2_offset; i += sizeof(struct KeySpace2)) {

        fseek(table.file, i, SEEK_SET);
        KeySpace2 *temp = malloc(sizeof(KeySpace2));
        fread(temp, sizeof(KeySpace2), 1, table.file);
        if (temp->busy) {
            char *str = get_str(&table, i, 2);
            printf("%d %15s\n", temp->key, str);
            free(str);
        }
        free(temp);
    }
}

int table_destroyer(struct Table *table) {
    fclose(table->file);
    remove(table->filename);
    free(table->filename);
    return EXIT_SUCCESS;
}

struct Table find_range(struct Table *table, int l, int r) {
    struct Table result = table_initializer(table->csize1, table->csize2, "temp");

    for (int i = table->ks1_offset; i < table->ks2_offset; i += sizeof(struct KeySpace1)) {
        KeySpace1 *temp_ks1 = malloc(sizeof(KeySpace1));
        fseek(table->file, i, SEEK_SET);
        fread(temp_ks1, sizeof(KeySpace1), 1, table->file);

        if (temp_ks1->busy) {
            Item *temp_item = malloc(sizeof(Item));
            fseek(table->file, temp_ks1->item_offset, SEEK_SET);
            fread(temp_item, sizeof(Item), 1, table->file);

            KeySpace2 *temp_ks2 = malloc(sizeof(KeySpace2));
            fseek(table->file, table->ks2_offset + temp_item->key2_index * (int) sizeof(KeySpace2),SEEK_SET);
            fread(temp_ks2, sizeof(KeySpace2), 1, table->file);

            int fkey = temp_ks1->key, lkey = temp_ks2->key;
            free(temp_ks2);
            free(temp_item);
            if ((fkey >= l) && (fkey <= r)) {
                char * str = get_str(table, i, 1);
                char *temp = malloc(sizeof(char) * (int) (strlen(str) + 1));
                strcpy(temp, str);
                insert(&result, temp, fkey, lkey);
                free(str);
            }
        }
        free((temp_ks1));
    }
    return result;
}

