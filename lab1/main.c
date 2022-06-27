#include "Table.h"
#include "Utils.h"


int size1 = 2, size2 = 2;




int main() {
    print_info();
    struct Table table = table_initializer(size1, size2);
    int cmd;
    while (1) {
        if (cli_read_int(&cmd, 1)) {
            continue;
        }
        if (cmd == 1) {
            insert_in(&table);
        } else if (cmd == 2) {
            find_in(&table);
        } else if (cmd == 3) {
            delete_in(&table);
        } else if (cmd == 4) {
            find_all_in(&table);
        } else if (cmd == 5) {
            delete_all_in(&table);
        } else if (cmd == 6) {
            print_table(table);
        } else if (cmd == 7) {
            range_in(&table);
        } else if (cmd == 8) {
            print_info();
        } else if (cmd == 0) {
            break;
        } else {
            fprintf(stderr, "Error: Unknown command.\n");
            fprintf(stderr, "\t%i\n", cmd);
            fprintf(stderr, "\t%*c\n", 1, '^');
        }
    }

    table_destroyer(&table);
    return EXIT_SUCCESS;

}