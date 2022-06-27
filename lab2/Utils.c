#include "Utils.h"
#include "errno.h"
const static int str_length = 100;

void print_info()
{
    printf("Enter the command number: \n"
           "1. Inserting an element by the composite key \n"
           "2. Searching the table for an element by the composite key \n"
           "3. Deleting an element from the table by the composite key \n"
           "4. Searching the element table by any given key \n"
           "5. Deleting from the table all the elements specified by the key in one of the key spaces \n"
           "6. Displaying the contents of the table on the screen \n"
           "7. Searching for elements specified by the range of keys\n"
           "8. Info\n"
           "To exit, write - 0\n");
}

int cli_read_str(char *str, int size)
{

    size_t length;

    if (!fgets(str, size + 2, stdin))
    {
        return EXIT_FAILURE;
    }

    length = strlen(str);
    if (str[length - 1] == '\n')
    {
        str[--length] = '\0';
        errno = 0;

        if (length == 0)
        {
            fprintf(stderr, "Error: An empty string was entered.\n");
            return EXIT_FAILURE;
        }

    } else
    {
        scanf("%*[^\n]");
        scanf("%*c");
        fprintf(stderr, "Error: do not enter more than %d character(s).\n", size);
        return EXIT_FAILURE;
    }

    return 0;
}

int cli_read_int(int *value, int size)
{
    size_t length;
    char *end = NULL;
    char *buf = malloc(sizeof(char) * (size + 2));

    if (!fgets(buf, size + 2, stdin))
    {
        free(buf);
        return EXIT_FAILURE;
    }

    length = strlen(buf);

    if (buf[length - 1] == '\n')
    {
        buf[--length] = '\0';
        errno = 0;
        *value = strtol(buf, &end, 10);
        if (length == 0)
        {
            fprintf(stderr, "Error: An empty string was entered.\n");
            free(buf);
            return EXIT_FAILURE;
        }
        if (errno != 0 || *end != '\0')
        {
            fprintf(stderr, "Error: Invalid character.\n");
            fprintf(stderr, "\t%s\n", buf);
            fprintf(stderr, "\t%*c\n", (int) (end - buf) + 1, '^');
            free(buf);
            return EXIT_FAILURE;
        }
    } else
    {
        scanf("%*[^\n]");
        scanf("%*c");
        fprintf(stderr, "Error: do not enter more than %d character(s).\n", size);
        free(buf);
        return EXIT_FAILURE;
    }
    free(buf);
    return EXIT_SUCCESS;
}

int insert_in(struct Table *table)
{
    int k1, k2;
    char *str = malloc(str_length);
    printf("Enter the key1\n");
    if (cli_read_int(&k1, 5) == EXIT_FAILURE)
    {
        free(str);
        return EXIT_FAILURE;
    }
    printf("Enter the key2\n");
    if (cli_read_int(&k2, 5) == EXIT_FAILURE)
    {
        free(str);
        return EXIT_FAILURE;
    }
    printf("Enter the value(str)\n");
    if (cli_read_str(str, str_length) == EXIT_FAILURE){
        free(str);
        return EXIT_FAILURE;
    }
    if (insert(table, str, k1, k2) == EXIT_SUCCESS)
    {
        printf("Successful!\n");
    }
    return EXIT_SUCCESS;
}

int find_in(struct Table *table)
{
    int k1, k2;
    printf("Enter the key1\n");
    if (cli_read_int(&k1, 5) == EXIT_FAILURE)
    {
        return EXIT_FAILURE;
    }
    printf("Enter the key2\n");
    if (cli_read_int(&k2, 5) == EXIT_FAILURE)
    {
        return EXIT_FAILURE;
    }
    char *temp = find(table, k1, k2);
    if (temp != NULL)
    {
        printf("%s\n", temp);
        free(temp);

        return EXIT_SUCCESS;
    } else
    {
        return EXIT_FAILURE;
    }
}

int delete_in(struct Table *table)
{
    int k1, k2;
    printf("Enter the key1\n");
    if (cli_read_int(&k1, 5) == EXIT_FAILURE)
    {
        return EXIT_FAILURE;
    }
    printf("Enter the key2\n");
    if (cli_read_int(&k2, 5) == EXIT_FAILURE)
    {
        return EXIT_FAILURE;
    }
    if (delete(table, k1, k2) != EXIT_SUCCESS)
    {
        return EXIT_FAILURE;
    } else
    {
        printf("Successful!\n");
        return EXIT_SUCCESS;
    }
}

int find_all_in(struct Table *table)
{
    int k1;
    printf("Enter the key1\n");
    if (cli_read_int(&k1, 5) == EXIT_FAILURE)
    {
        return EXIT_FAILURE;
    }
    char *temp = find_all(table, k1);
    if (temp == NULL)
    {
        return EXIT_FAILURE;
    }
    printf("%s\n", temp);
    free(temp);
    return EXIT_SUCCESS;
}

int delete_all_in(struct Table *table)
{
    int k1, ks;
    printf("Enter the keyspace number\n");
    if (cli_read_int(&ks, 1) == EXIT_FAILURE)
    {
        return EXIT_FAILURE;
    }
    if (ks != 1 && ks != 2){
        fprintf(stderr, "Error: keyspace not found\n");
        return EXIT_FAILURE;
    }

    printf("Enter the key1\n");
    if (cli_read_int(&k1, 5) == EXIT_FAILURE)
    {
        return EXIT_FAILURE;
    }
    if (delete_all(table, k1, ks) != EXIT_SUCCESS)
    {
        return EXIT_FAILURE;
    }
    printf("Successful!\n");
    return EXIT_SUCCESS;
}

int range_in(struct Table *table)
{
    int l, r;
    printf("Enter the left border\n");
    if (cli_read_int(&l, 6) == EXIT_FAILURE)
    {
        return EXIT_FAILURE;
    }
    printf("Enter the right border\n");
    if (cli_read_int(&r, 6) == EXIT_FAILURE)
    {
        return EXIT_FAILURE;
    }
    if (l > r){
        fprintf(stderr, "Error: Wrong borders\n");
        return EXIT_FAILURE;
    }
    struct Table t = find_range(table, l, r);
    print_table(t);
    table_destroyer(&t);
    return EXIT_SUCCESS;
}