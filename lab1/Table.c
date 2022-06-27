#include "Table.h"
#include <string.h>

struct Table table_initializer(int size1, int size2)
{
    struct Table table;
    table.msize1 = size1;
    table.msize2 = size2;
    table.csize1 = 0;
    table.csize2 = 0;
    table.ks1 = calloc(size1, sizeof(struct KeySpace1));
    table.ks2 = calloc(size2, sizeof(struct KeySpace2));
    return table;
}

int insert(struct Table *table, char *str, int k1, int k2)
{
    if ((search_ks1(table, k1) != -1) || (search_ks2(table, k2) != -1))
    {
        free(str);
        fprintf(stderr, "Error: The table key is busy\n");
        return -1;
    }
    struct Item *temp = malloc(sizeof(struct Item));
    temp->info = str;
    if (insert_ks1(table, k1, temp) == EXIT_SUCCESS && insert_ks2(table, k2, temp) == EXIT_SUCCESS)
    {
        return EXIT_SUCCESS;
    } else
    {
        free(str);
        fprintf(stderr, "Error: table overflow\n");
        return -2;
    }
}

char *find(struct Table *table, int k1, int k2)
{
    int l1 = search_ks1(table, k1), l2 = search_ks2(table, k2);
    if ((l1 != -1) && (l2 != -1))
    {
        if (!(strcmp(table->ks1[l1].item->info, table->ks2[l2].item->info)))
        {
            return table->ks1[l1].item->info;
        }
    }
    fprintf(stderr, "Error: there is no element in the table\n");
    return NULL;
}

char *find_all(struct Table *table, int k)
{
    int l1 = search_ks1(table, k), l2 = search_ks2(table, k), value = 0;
    if ((l1 == -1) && (l2 == -1))
    {
        fprintf(stderr, "Error: there is no element in the table\n");
        return NULL;
    }
    char *str1, *str2;
    int str_len1 = 0, str_len2 = 0;
    if (l1 != -1)
    {
        str1 = table->ks1[l1].item->info;
        str_len1 = (int) strlen(str1);
    }
    if (l2 != -1)
    {
        str2 = table->ks2[l2].item->info;
        str_len2 = (int) strlen(str2);
    }
    char *result = malloc((str_len1 + str_len2 + 2) * sizeof(char));
    if (str_len1)
    {
        strcpy(result, str1);
        if (str_len2)
        {
            strcat(result, " ");
            strcat(result, str2);
        }
    } else
    {
        if (str_len2)
        {
            strcpy(result, str2);
        }
    }
    return result;
}

int delete(struct Table *table, int k1, int k2)
{
    int l1 = search_ks1(table, k1), l2 = search_ks2(table, k2);
    if ((l1 != -1) && (l2 != -1))
    {
        if (table->ks1[l1].item->info == table->ks2[l2].item->info)
        {
            free(table->ks1[l1].item->info);
            free(table->ks1[l1].item);
            delete_ks1(table, k1);
            delete_ks2(table, k2);
            return EXIT_SUCCESS;
        }
    }
    fprintf(stderr, "Error: there is no element in the table\n");
    return -1;
}

int delete_all(struct Table *table, int k, int ks)
{
    if (ks == 1)
    {
        if (search_ks1(table, k) > 0)
        {
            int l1 = k;
            int l2 = table->ks2[table->ks1[search_ks1(table, k)].item->key2].key;

            return delete(table, l1, l2);
        } else
        {
            fprintf(stderr, "Error: there is no element in the table\n");
            return EXIT_FAILURE;
        }
    } else
    {
        if (search_ks2(table, k) > 0){
        int l2 = k;
        int l1 = table->ks1[table->ks2[search_ks2(table, k)].item->key1].key;

        return delete(table, l1, l2);}else{
            fprintf(stderr, "Error: there is no element in the table\n");
            return EXIT_FAILURE;
        }
    }
}

void print_table(struct Table table)
{
    printf("first keyspace");
    printf("\nKey   |   Value\n");
    for (int i = 0; i < table.msize1; i++)
    {
        if (table.ks1[i].busy)
        {
            printf("%d %15s\n", table.ks1[i].key, table.ks1[i].item->info);
        }
    }
    printf("\nsecond keyspace");
    printf("\nKey   |   Value\n");
    for (int i = 0; i < table.msize2; i++)
    {
        if (table.ks2[i].busy)
        {
            printf("%d %15s\n", table.ks2[i].key, table.ks2[i].item->info);
        }
    }
}

struct Table find_range(struct Table *table, int l, int r)
{
    struct Table result = table_initializer(table->csize1, table->csize2);
    for (int i = 0; i < table->msize1; i++)
    {
        if (table->ks1[i].busy)
        {
            int fkey = table->ks1[i].key, lkey = table->ks2[table->ks1[i].item->key2].key;
            if ((fkey >= l) && (fkey <= r))
            {
                char *temp = malloc(sizeof(char) * (int) (strlen(table->ks1[i].item->info) + 1));
                strcpy(temp, table->ks1[i].item->info);
                insert(&result, temp, fkey, lkey);
            }
        }
    }
    return result;
}

int table_destroyer(struct Table *table)
{
    for (int i = 0; i < table->csize1; i++)
    {
        if (table->ks1[i].busy)
        {
            free(table->ks1[i].item->info);
            free(table->ks1[i].item);
        }
    }
    free(table->ks1);
    free(table->ks2);
    return EXIT_SUCCESS;
}