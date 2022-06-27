#include "Table.h"

int search_ks1(Table *table, int k) {
    for (int i = table->ks1_offset; i < table->ks2_offset; i += sizeof(struct KeySpace1)) {
        fseek(table->file, i, SEEK_SET);
        KeySpace1 *temp = malloc(sizeof(KeySpace1));
        fread(temp, sizeof(KeySpace1), 1, table->file);
        if (temp->busy && temp->key == k) {
            free(temp);
            return i;
        }
        free(temp);
    }
    return -1; /* элемент не найден */
}

int garbage_collector_ks1(struct Table *table) {
    int cur = table->csize1;
    table->csize1 = 0;
    for (int i = table->ks1_offset; i < table->ks2_offset; i += sizeof(KeySpace1)) {
        fseek(table->file, i, SEEK_SET);
        KeySpace1 *temp = malloc(sizeof(KeySpace1));
        fread(temp, sizeof(KeySpace1), 1, table->file);
        if (temp->busy) /* элемент занят */
        {
            if (i != table->csize1 * sizeof(KeySpace1) + sizeof (int)) /* элемент должен быть перемещен */
            {
                fseek(table->file, temp->item_offset, SEEK_SET);
                Item *temp_item = malloc(sizeof(Item));
                fread(temp_item, sizeof(Item), 1, table->file);
                temp_item->key1_index = table->csize1;
                fseek(table->file, temp->item_offset, SEEK_SET);
                fwrite(temp_item, sizeof(Item), 1, table->file);
                free(temp_item);

                fseek(table->file, table->csize1 * (int) sizeof(KeySpace1) + sizeof (int), SEEK_SET);
                fwrite(temp, sizeof(KeySpace1), 1, table->file);
            }
            table->csize1++;
        }
        free(temp);
    }
    return table->csize1 == table->msize1 ? -1 : 0; /* в таблице нет свободного места */
}

int insert_ks1(struct Table *table, int k, struct Item *item) {

    if (search_ks1(table, k) >= 0) {
        return -1;
    }
    if (table->csize1 == table->msize1) {
        if (garbage_collector_ks1(table) < 0) {
            return -2;
        }
    }
    item->key1_index = table->csize1;
    KeySpace1 *temp_ks1 = malloc(sizeof(KeySpace1));
    temp_ks1->busy = 1;
    temp_ks1->key = k;
    temp_ks1->item_offset = file_size(table->file);
    fseek(table->file, table->csize1 * (int) sizeof(KeySpace1) +(int) sizeof(int), SEEK_SET);
    fwrite(temp_ks1, sizeof(KeySpace1), 1, table->file);
    table->csize1++;
    free(temp_ks1);
    return EXIT_SUCCESS;
}

int file_size(FILE *fp) {
    long int save_pos, size_of_file;
    save_pos = ftell(fp);
    fseek(fp, 0L, SEEK_END);
    size_of_file = ftell(fp);
    fseek(fp, save_pos, SEEK_SET);
    return (size_of_file);
}

int delete_ks1(struct Table *table, int k) {
    int l;
    if ((l = search_ks1(table, k)) < 0) {
        return -1; /* указанного элемента в таблице нет */
    }
    fseek(table->file, l, SEEK_SET);
    KeySpace1 *temp_ks1 = malloc(sizeof(KeySpace1));
    temp_ks1->item_offset = -1;
    temp_ks1->key = -1;
    temp_ks1->busy = 0;
    fwrite(temp_ks1, sizeof(KeySpace1), 1, table->file);
    free(temp_ks1);
    return EXIT_SUCCESS;
}


int era(int n) {
    int p;
    int *mass;
    char flag;
    if (n >= 2) {
        n = n - 1;
        mass = (int *) malloc(n * sizeof(int));
        for (int i = 0; i < n; i++) {
            mass[i] = i + 2;
        }
        for (int i = 0; i < n; i++) {
            p = mass[i];
            flag = 0;
            for (int j = i + 1; j < n; j++) {
                if (!(mass[j] % p)) {
                    for (int k = j; k < n - 1; k++) {
                        mass[k] = mass[k + 1];
                    }
                    flag = 1;
                    n--;
                    j--;
                }
            }
            if (flag == 0) break;
        }
        if (n > 2)
            n = mass[n - 2];
        else
            n = 1;
        free(mass);
        return n;
    } else {
        return 1;
    }
}

int hash1(struct Table *table, int k) {
    return k % table->msize2;
}

int hash2(struct Table *table, int value) {
    int n = era(table->msize2);
    return n - (value % n);
}

int insert_ks2(struct Table *table, int k, struct Item *item) {
    int start, i = hash1(table, k), h = hash2(table, k);
    start = i;
    fseek(table->file, i * (int) sizeof(KeySpace2) + table->ks2_offset, SEEK_SET);
    KeySpace2 *temp_ks2 = malloc(sizeof(KeySpace2));
    fread(temp_ks2, sizeof(KeySpace2), 1, table->file);

    while (temp_ks2->busy > 0) /* позиция занята */
    {
        if (temp_ks2->key == k) /* элемент с заданным ключом есть в таблице */
        {
            return -1;
        }
        i = (i + h) % (table->msize2); /* следующая позиция */
        fseek(table->file, i * (int) sizeof(KeySpace2) + table->ks2_offset, SEEK_SET);
        fread(temp_ks2, sizeof(KeySpace2), 1, table->file);
        if (i == start) /* вернулись в исходную позицию - таблица полна */
        {
            return -2;
        }
    }

    item->key2_index = i;
    temp_ks2->busy = 1;
    temp_ks2->key = k;
    temp_ks2->item_offset = file_size(table->file);
    fseek(table->file, i * (int) sizeof(KeySpace2) + table->ks2_offset, SEEK_SET);
    fwrite(temp_ks2, sizeof(KeySpace1), 1, table->file);
    table->csize2++;
    free(temp_ks2);
    fseek(table->file, 0, SEEK_SET);
    fwrite(&table->csize2, sizeof(int), 1, table->file);
    return EXIT_SUCCESS;
}

int search_ks2(struct Table *table, int k) {
    int start, i = hash1(table, k), h = hash2(table, k); /* шаг перемешивания */
    start = i;
    fseek(table->file, i * (int) sizeof(KeySpace2) + table->ks2_offset, SEEK_SET);
    KeySpace2 *temp_ks2 = malloc(sizeof(KeySpace2));
    fread(temp_ks2, sizeof(KeySpace2), 1, table->file);

    while (temp_ks2->busy > 0) /* позиция не свободна */
    {
        if (temp_ks2->busy > 0 && temp_ks2->key == k) /* элемент найден */
        {
            free(temp_ks2);
            return i * (int) sizeof(KeySpace2) + table->ks2_offset;
        }
        i = (i + h) % table->msize2; /* следующая позиция */
        fseek(table->file, i * (int) sizeof(KeySpace2) + table->ks2_offset, SEEK_SET);
        fread(temp_ks2, sizeof(KeySpace2), 1, table->file);
        if (i == start) {
            break;
        }
    }
    free(temp_ks2);
    return -1; /* вернулись в исходную позицию – элемента в таблице нет */
}


int delete_ks2(struct Table *table, int k) {
    int l;
    if ((l = search_ks2(table, k)) < 0) {
        return -1; /* указанного элемента в таблице нет */
    }
    fseek(table->file, l, SEEK_SET);
    KeySpace2 *temp_ks2 = malloc(sizeof(KeySpace2));
    temp_ks2->busy = 0;
    temp_ks2->key =-1;
    temp_ks2->item_offset =-1;
    fwrite(temp_ks2, sizeof(KeySpace2), 1, table->file);
    table->csize2--;
    fseek(table->file, 0, SEEK_SET);
    fwrite(&table->csize2, sizeof(int), 1, table->file);
    free(temp_ks2);
    return EXIT_SUCCESS;
}


