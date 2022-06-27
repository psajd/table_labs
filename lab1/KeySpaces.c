#include "Table.h"

int search_ks1(struct Table *table, int k) {
    int i;
    for (i = 0; i < table->csize1; i++) {
        if (table->ks1[i].busy && table->ks1[i].key == k) {
            return i; /* элемент найден – он занят и имеет искомый ключ */
        }
    }
    return -1; /* элемент не найден */
}


int garbage_collector_ks1(struct Table *table) {
    int i, cur = table->msize1;
    table->csize1 = 0;
    for (i = 0; i < cur; i++) {
        if (table->ks1[i].busy) /* элемент занят */
        {
            if (i != table->csize1) /* элемент должен быть перемещен */
            {
                table->ks1[i].item->key1 = table->csize1;
                table->ks1[table->csize1] = table->ks1[i]; /* перемещение элемента */
            }
            table->csize1++;
        }
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
    item->key1 = table->csize1;
    table->ks1[table->csize1].busy = 1;
    table->ks1[table->csize1].key = k;
    table->ks1[table->csize1].item = item;
    table->csize1++;
    return EXIT_SUCCESS;
}

int delete_ks1(struct Table *table, int k) {
    int l;
    if ((l = search_ks1(table, k)) < 0) {
        return -1; /* указанного элемента в таблице нет */
    }
    table->ks1[l].busy = 0;
    //free(table->ks1[l].item);
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

int  hash1(struct Table *table, int k) {
    return k % table->msize2;
}

int  hash2(struct Table *table, int value) {
    int n = era(table->msize2);
    return n - (value % n);
}


int insert_ks2(struct Table *table, int k, struct Item *item) {
    int start, i, h = hash2(table, k); /* шаг перемешивания */
    /* вычисление исходной позиции таблицы */
    i = hash1(table, k);
    start = i;
    while (table->ks2[i].busy > 0) /* позиция занята */
    {
        if (table->ks2[i].key == k) /* элемент с заданным ключом есть в таблице */
        {
            return -1;
        }
        i = (i + h) % table->msize2; /* следующая позиция */
        if (i == start) /* вернулись в исходную позицию - таблица полна */
        {
            return -2;
        }
    }
    item->key2 = i;
    table->ks2[i].busy = 1;
    table->ks2[i].key = k;
    table->ks2[i].item = item;
    table->csize2++;
    return EXIT_SUCCESS;
}

int search_ks2(struct Table *table, int k) {
    int start, i, h = hash2(table, k); /* шаг перемешивания */
    i = hash1(table, k); /* вычисление исходной позиции таблицы */
    start = i;

    while (table->ks2[i].busy >= 0) /* позиция не свободна */
    {
        if (table->ks2[i].busy > 0 && table->ks2[i].key == k) /* элемент найден */
        {
            return i;
        }
        i = (i + h) % table->msize2; /* следующая позиция */
        if (i == start) {
            break;
        }
    }
    return -1; /* вернулись в исходную позицию – элемента в таблице нет */
}

int delete_ks2(struct Table *table, int k) {
    int l;
    if ((l = search_ks2(table, k)) < 0) {
        return -1; /* указанного элемента в таблице нет */
    }
    table->ks2[l].busy = 0;
    table->csize2--;
    return EXIT_SUCCESS;
}