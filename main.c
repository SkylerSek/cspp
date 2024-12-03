#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

const int FILE_BUFFER = 1024;
const int OPERATION_BUFFER = 1024;
const int INPUT_BUFFER = 1024;
char **change_log;
int number_of_change = 0;
int max_change_log_length = 0;

void create_file();
void copy_file();
void delete_file();
void show_file();
void append_line();
void delete_line();
void insert_line();
void show_line();
void show_number_of_line();
void show_change_log();
void rename_file();
void search_text();
int count_number_of_line(char *filename);
void add_change_log(char operation[OPERATION_BUFFER]);
void free_change_log();

int main()
{
    int choice;
    char input[INPUT_BUFFER];
    int loop = 1;
    while (loop)
    {
        for (int i = 0; i < 24; i++)
        {
            printf("-");
        }
        printf("\n0. Exit program\n");
        printf("1. Create file\n");
        printf("2. Copy file\n");
        printf("3. Delete file\n");
        printf("4. Show file\n");
        printf("5. Append line\n");
        printf("6. Delete line\n");
        printf("7. Insert line\n");
        printf("8. Show line\n");
        printf("9. Show change log\n");
        printf("10. Show number of lines\n");
        printf("11. Rename file\n");
        printf("12. Search text\n");
        for (int i = 0; i < 24; i++)
        {
            printf("-");
        }
        printf("\n");
        // read user's choice
        while (1)
        {
            printf("Enter your choice: ");
            if (fgets(input, sizeof(input), stdin) == NULL)
            {
                printf("\033[31mError: Failed to read input\033[0m\n");
                choice = 0;
                break;
            }
            if (sscanf(input, "%d", &choice) == 1 && 0 <= choice && choice <= 12)
            {
                break;
            }
            else
            {
                printf("Please enter an integer between 0 and 12\n");
            }
        }

        switch (choice)
        {
        case 0:
            loop = 0;
            break;
        case 1:
            create_file();
            break;
        case 2:
            copy_file();
            break;
        case 3:
            delete_file();
            break;
        case 4:
            show_file();
            break;
        case 5:
            append_line();
            break;
        case 6:
            delete_line();
            break;
        case 7:
            insert_line();
            break;
        case 8:
            show_line();
            break;
        case 9:
            show_change_log();
            break;
        case 10:
            show_number_of_line();
            break;
        case 11:
            rename_file();
            break;
        case 12:
            search_text();
            break;
        }
    }

    // free the memory of change log to prevent memory leak
    free_change_log();

    return 0;
}
/*
Ask for permission to overwrite existing file
parameter: string prompt
return: 1 for agree, 0 for deny
*/
int get_permission(char *prompt)
{
    char input[4];
    do
    {
        printf("\033[31m%s\033[0m", prompt);
        if (fgets(input, sizeof(input), stdin) == NULL)
        {
            printf("\033[31mError: Failed to read input\033[0m\n");
            return 0;
        }
        input[strcspn(input, "\n")] = '\0';
        if ('a' <= input[0] && input[0] <= 'z')
        {
            input[0] -= 32;
        }
        if (input[0] == 'N' && strlen(input) == 1)
        {
            return 0;
        }
    } while (input[0] != 'Y' || strlen(input) != 1);
    return 1;
}
void create_file()
{
    char filename[FILE_BUFFER];
    FILE *file;
    // get file name
    printf("Enter the name of the file to create: ");
    if (fgets(filename, sizeof(filename), stdin) == NULL)
    {
        printf("\033[31mError: Failed to read input\033[0m\n");
        return;
    }
    // remove trailing \n
    filename[strcspn(filename, "\n")] = '\0';
    // check existence for file
    if (access(filename, F_OK) == 0)
    {
        printf("%s already exists\n", filename);
        if (get_permission("Do you wish to overwrite (Y/N)? ") == 0)
        {
            return;
        }
    }
    // create new file
    file = fopen(filename, "w");
    if (file == NULL)
    {
        printf("\033[31mError: Failed to create file\033[0m\n");
        return;
    }
    printf("\033[32mFile created\033[0m\n");
    fclose(file);
    char operation[OPERATION_BUFFER];
    snprintf(operation, sizeof(operation), "%d. %s: Created file (Lines: 0)", number_of_change + 1, filename);
    add_change_log(operation);
}
void copy_file()
{
    char original_file[FILE_BUFFER], new_file[FILE_BUFFER];
    FILE *original, *new;

    // get original file name
    printf("Enter the name of the original file: ");
    if (fgets(original_file, sizeof(original_file), stdin) == NULL)
    {
        printf("\033[31mError: Failed to read input\033[0m\n");
        return;
    }
    // remove trailing \n
    original_file[strcspn(original_file, "\n")] = '\0';
    // check existence for file
    if (access(original_file, F_OK) != 0)
    {
        printf("%s doesn't exist\n", original_file);
        return;
    }

    // get new file name
    printf("Enter the name of the new file: ");
    if (fgets(new_file, sizeof(new_file), stdin) == NULL)
    {
        printf("\033[31mError: Failed to read input\033[0m\n");
        return;
    }
    // remove trailing \n
    new_file[strcspn(new_file, "\n")] = '\0';
    // prevent copying to itself
    if (strcmp(original_file, new_file) == 0)
    {
        printf("You are not allowed to copy a file to itself\n");
        return;
    }
    // check existence for file
    if (access(new_file, F_OK) == 0)
    {
        printf("%s already exists\n", new_file);
        if (get_permission("Do you wish to overwrite(Y/N)? ") == 0)
        {
            return;
        }
    }

    original = fopen(original_file, "r");
    if (original == NULL)
    {
        printf("\033[31mError: Failed to open original file\033[0m\n");
        return;
    }
    new = fopen(new_file, "w");
    if (new == NULL)
    {
        printf("\033[31mError: Failed to create new file\033[0m\n"); 
        fclose(original);
        return;
    }
    char c;
    while ((c = fgetc(original)) != EOF)
    {
        fputc(c, new);
    }
    printf("\033[32mFile copied\033[0m\n");
    fclose(original);
    fclose(new);
    char operation[OPERATION_BUFFER];
    snprintf(operation, sizeof(operation), "%d. %s: Copied from %s (Lines: %d)", number_of_change + 1, new_file, original_file, count_number_of_line(new_file));
    add_change_log(operation);
}
void delete_file()
{
    char filename[FILE_BUFFER];
    FILE *file;
    // get file name
    printf("Enter the name of the file to delete: ");
    if (fgets(filename, sizeof(filename), stdin) == NULL)
    {
        printf("\033[31mError: Failed to read input\033[0m\n");
        return;
    }
    // remove trailing \n
    filename[strcspn(filename, "\n")] = '\0';
    // check existence for file
    if (access(filename, F_OK) != 0)
    {
        printf("%s doesn't exist\n", filename);
        return;
    }
    if (get_permission("Do you wish to delete (Y/N)? ") == 0)
    {
        return;
    }
    // delete file
    if (remove(filename) != 0)
    {
        printf("\033[31mError: Failed to delete file\033[0m\n");
        return;
    }
    printf("\033[32mFile deleted\033[0m\n");
    char operation[OPERATION_BUFFER];
    snprintf(operation, sizeof(operation), "%d. %s: Deleted file (Lines: N/A)", number_of_change + 1, filename);
    add_change_log(operation);
}
void show_file()
{
    char filename[FILE_BUFFER];
    FILE *file;
    // get file name
    printf("Enter the name of the file to show: ");
    if (fgets(filename, sizeof(filename), stdin) == NULL)
    {
        printf("\033[31mError: Failed to read input\033[0m\n");
        return;
    }
    // remove trailing \n
    filename[strcspn(filename, "\n")] = '\0';
    // check existence for file
    if (access(filename, F_OK) != 0)
    {
        printf("%s doesn't exist\n", filename);
        return;
    }
    // open file in read mode
    file = fopen(filename, "r");
    if (file == NULL)
    {
        printf("\033[31mError: Failed to open file\033[0m\n");
        return;
    }
    // show file
    char c;
    while ((c = fgetc(file)) != EOF)
    {
        printf("%c", c);
    }
    printf("\n");
    fclose(file);
}
void append_line()
{
    char filename[FILE_BUFFER];
    FILE *file;
    // get file name
    printf("Enter the name of the file: ");
    if (fgets(filename, sizeof(filename), stdin) == NULL)
    {
        printf("\033[31mError: Failed to read input\033[0m\n");
        return;
    }
    // remove trailing \n
    filename[strcspn(filename, "\n")] = '\0';
    // check existence for file
    if (access(filename, F_OK) != 0)
    {
        printf("%s doesn't exist\n", filename);
        return;
    }
    char input[INPUT_BUFFER];
    printf("Enter content to append: ");
    if (fgets(input, sizeof(input), stdin) == NULL)
    {
        printf("\033[31mError: Failed to read input\033[0m\n");
        return;
    }
    file = fopen(filename, "a");
    if (file == NULL)
    {
        printf("\033[31mError: Failed to open file\033[0m\n");
        return;
    }
    if (fputs(input, file) == EOF)
    {
        printf("\033[31mError: Failed to append line\033[0m\n");
        fclose(file);
        return;
    }
    printf("\033[32mAppended line\033[0m\n");
    fclose(file);
    char operation[OPERATION_BUFFER];
    snprintf(operation, sizeof(operation), "%d. %s: Appended line (Lines: %d)", number_of_change + 1, filename, count_number_of_line(filename));
    add_change_log(operation);
}
void delete_line()
{
    char filename[FILE_BUFFER];
    FILE *file;
    // get file name
    printf("Enter the name of the file: ");
    if (fgets(filename, sizeof(filename), stdin) == NULL)
    {
        printf("\033[31mError: Failed to read input\033[0m\n");
        return;
    }
    // remove trailing \n
    filename[strcspn(filename, "\n")] = '\0';
    // check existence for file
    if (access(filename, F_OK) != 0)
    {
        printf("%s doesn't exist\n", filename);
        return;
    }
    int number_of_line = count_number_of_line(filename);
    if (number_of_line == -1)
    {
        printf("\033[31mError: Failed to open file\033[0m\n");
        return;
    }
    if (number_of_line == 0)
    {
        printf("%s is empty\n", filename);
        return;
    }
    char input[INPUT_BUFFER];
    int choice;
    while (1)
    {
        printf("Enter line number: ");
        if (fgets(input, sizeof(input), stdin) == NULL)
        {
            printf("\033[31mError: Failed to read input\033[0m\n");
            return;
        }
        if (sscanf(input, "%d", &choice) == 1 && 1 <= choice && choice <= number_of_line)
        {
            break;
        }
        else
        {
            printf("Please enter an integer between 1 and %d\n", number_of_line);
        }
    }
    FILE *temp_file;
    char temp_file_name[FILE_BUFFER];
    for (int i = 0;; i++)
    {
        snprintf(temp_file_name, sizeof(temp_file_name), "TEMP(%d)-%s", i, filename);
        if (access(temp_file_name, F_OK) != 0)
        {
            break;
        }
    }
    file = fopen(filename, "r");
    if (file == NULL)
    {
        printf("\033[31mError: Failed to open file\033[0m\n");
        return;
    }
    temp_file = fopen(temp_file_name, "w");
    if (temp_file == NULL)
    {
        printf("\033[31mError: Failed to create temporary file\033[0m\n");
        fclose(file);
        return;
    }
    number_of_line = 0;
    char line[1000];
    while (fgets(line, sizeof(line), file))
    {
        number_of_line++;
        if (number_of_line == choice)
        {
            continue;
        }
        fputs(line, temp_file);
    }
    fclose(file);
    fclose(temp_file);
    file = fopen(filename, "w");
    if (file == NULL)
    {
        printf("\033[31mError: Failed to write to file\033[0m\n");
        return;
    }
    temp_file = fopen(temp_file_name, "r");
    if (temp_file == NULL)
    {
        printf("\033[31mError: Failed to read temporary file\033[0m\n");
        fclose(file);
        return;
    }
    while (fgets(line, sizeof(line), temp_file))
    {
        fputs(line, file);
    }
    printf("\033[32mDeleted line %d\033[0m\n", choice);
    fclose(file);
    fclose(temp_file);
    // add to change log
    char operation[OPERATION_BUFFER];
    snprintf(operation, sizeof(operation), "%d. %s: Deleted line %d (Lines: %d)", number_of_change + 1, filename, choice, count_number_of_line(filename));
    add_change_log(operation);
    // remove temporary file
    if (remove(temp_file_name) != 0)
    {
        printf("\033[31mError: Failed to delete temporary file\033[0m\n");
        return;
    }
}
void insert_line()
{
    char filename[FILE_BUFFER];
    FILE *file;
    // get file name
    printf("Enter the name of the file: ");
    if (fgets(filename, sizeof(filename), stdin) == NULL)
    {
        printf("\033[31mError: Failed to read input\033[0m\n");
        return;
    }
    // remove trailing \n
    filename[strcspn(filename, "\n")] = '\0';
    // check existence for file
    if (access(filename, F_OK) != 0)
    {
        printf("%s doesn't exist\n", filename);
        return;
    }
    int number_of_line = count_number_of_line(filename);
    if (number_of_line == -1)
    {
        printf("\033[31mError: Failed to open file\033[0m\n");
        return;
    }
    char input[INPUT_BUFFER];
    int choice;
    while (1)
    {
        printf("Enter line number to insert after: ");
        if (fgets(input, sizeof(input), stdin) == NULL)
        {
            printf("\033[31mError: Failed to read input\033[0m\n");
            return;
        }
        if (sscanf(input, "%d", &choice) == 1 && 0 <= choice && choice <= number_of_line)
        {
            break;
        }
        else
        {
            printf("Please enter an integer between 0 and %d\n", number_of_line);
        }
    }
    printf("Enter content to insert: ");
    if (fgets(input, sizeof(input), stdin) == NULL)
    {
        printf("\033[31mError: Failed to read input\033[0m\n");
        return;
    }
    FILE *temp_file;
    char temp_file_name[FILE_BUFFER];
    for (int i = 0;; i++)
    {
        snprintf(temp_file_name, sizeof(temp_file_name), "TEMP(%d)-%s", i, filename);
        if (access(temp_file_name, F_OK) != 0)
        {
            break;
        }
    }
    file = fopen(filename, "r");
    if (file == NULL)
    {
        printf("\033[31mError: Failed to open file\033[0m\n");
        return;
    }
    temp_file = fopen(temp_file_name, "w");
    if (temp_file == NULL)
    {
        printf("\033[31mError: Failed to create temporary file\033[0m\n");
        fclose(file);
        return;
    }
    number_of_line = 0;
    char line[INPUT_BUFFER];
    if (choice == 0)
    {
        fputs(input, temp_file);
    }
    while (fgets(line, sizeof(line), file))
    {
        number_of_line++;
        fputs(line, temp_file);
        if (number_of_line == choice)
        {
            fputs(input, temp_file);
        }
    }
    fclose(file);
    fclose(temp_file);
    file = fopen(filename, "w");
    if (file == NULL)
    {
        printf("\033[31mError: Failed to write to file\033[0m\n");
        return;
    }
    temp_file = fopen(temp_file_name, "r");
    if (temp_file == NULL)
    {
        printf("\033[31mError: Failed to read temporary file\033[0m\n");
        fclose(file);
        return;
    }
    while (fgets(line, sizeof(line), temp_file))
    {
        fputs(line, file);
    }
    printf("\033[32mInserted after line %d\033[0m\n", choice);
    fclose(file);
    fclose(temp_file);
    // add to change log
    char operation[OPERATION_BUFFER];
    snprintf(operation, sizeof(operation), "%d. %s: Inserted after line %d (Lines: %d)", number_of_change + 1, filename, choice, count_number_of_line(filename));
    add_change_log(operation);
    // remove temporary file
    if (remove(temp_file_name) != 0)
    {
        printf("\033[31mError: Failed to delete temporary file\033[0m\n");
        return;
    }
}
void show_line()
{
    char filename[FILE_BUFFER];
    FILE *file;
    // get file name
    printf("Enter the name of the file: ");
    if (fgets(filename, sizeof(filename), stdin) == NULL)
    {
        printf("\033[31mError: Failed to read input\033[0m\n");
        return;
    }
    // remove trailing \n
    filename[strcspn(filename, "\n")] = '\0';
    // check existence for file
    if (access(filename, F_OK) != 0)
    {
        printf("%s doesn't exist\n", filename);
        return;
    }
    int number_of_line = count_number_of_line(filename);
    if (number_of_line == -1)
    {
        printf("\033[31mError: Failed to open file\033[0m\n");
        return;
    }
    if (number_of_line == 0)
    {
        printf("%s is empty\n", filename);
        return;
    }
    char input[INPUT_BUFFER];
    int choice;
    while (1)
    {
        printf("Enter line number: ");
        if (fgets(input, sizeof(input), stdin) == NULL)
        {
            printf("\033[31mError: Failed to read input\033[0m\n");
            return;
        }
        if (sscanf(input, "%d", &choice) == 1 && 1 <= choice && choice <= number_of_line)
        {
            break;
        }
        else
        {
            printf("Please enter an integer between 1 and %d\n", number_of_line);
        }
    }
    file = fopen(filename, "r");
    if (file == NULL)
    {
        printf("\033[31mError: Failed to open file\033[0m\n");
        return;
    }
    char line[1000];
    number_of_line = 0;
    while (number_of_line < choice && fgets(line, sizeof(line), file))
    {
        number_of_line++;
    }
    printf("Content of line %d: %s\n", choice, line);
    fclose(file);
}
void show_number_of_line()
{
    char filename[FILE_BUFFER];
    FILE *file;
    // get file name
    printf("Enter the name of the file to show: ");
    if (fgets(filename, sizeof(filename), stdin) == NULL)
    {
        printf("\033[31mError: Failed to read input\033[0m\n");
        return;
    }
    // remove trailing \n
    filename[strcspn(filename, "\n")] = '\0';
    // check existence for file
    if (access(filename, F_OK) != 0)
    {
        printf("%s doesn't exist\n", filename);
        return;
    }
    int num_of_line = count_number_of_line(filename);
    if (num_of_line == -1)
    {
        printf("\033[31mError: Failed to open file\033[0m\n");
        return;
    }
    printf("%s has %d lines\n", filename, num_of_line);
}
int count_number_of_line(char *filename)
{
    FILE *file;
    int number_of_line = 0;
    char line[INPUT_BUFFER];
    file = fopen(filename, "r");
    if (file == NULL)
    {
        return -1;
    }
    while (fgets(line, sizeof(line), file))
    {
        number_of_line++;
    }
    fclose(file);
    return number_of_line;
}
void show_change_log()
{
    if (number_of_change == 0)
    {
        printf("No operations have been performed\n");
        return;
    }
    for (int i = 0; i < max_change_log_length; i++)
    {
        printf("-");
    }
    printf("\n");
    for (int i = 0; i < number_of_change; i++)
    {
        if (change_log[i] == NULL)
        {
            continue;
        }
        printf("%s\n", change_log[i]);
    }
    for (int i = 0; i < max_change_log_length; i++)
    {
        printf("-");
    }
    printf("\n");
}
void add_change_log(char operation[OPERATION_BUFFER])
{
    // create change log dynamic array
    if (number_of_change == 0)
    {
        change_log = malloc(sizeof(char *));
        if (change_log == NULL)
        {
            printf("\033[31mError: Failed to allocate memory for change log\033[0m\n");
            return;
        }
    }
    // reallocate memory for change log
    else
    {
        char **temp = realloc(change_log, (number_of_change + 1) * sizeof(char *));
        // return when failed to reallocate memory
        if (temp == NULL)
        {
            printf("\033[31mError: Failed to allocate memory for change log\033[0m\n");
            return;
        }
        // change pointer of change log to new array
        else
        {
            change_log = temp;
        }
    }
    change_log[number_of_change] = malloc(OPERATION_BUFFER * sizeof(char));
    if (change_log[number_of_change] == NULL)
    {
        printf("\033[31mError: Failed to allocate memory for change log\033[0m\n");
        return;
    }
    if (strlen(operation) > max_change_log_length)
    {
        max_change_log_length = strlen(operation);
    }
    snprintf(change_log[number_of_change], OPERATION_BUFFER * sizeof(char), "%s",  operation);
    number_of_change++;
}
void free_change_log()
{
    if (number_of_change == 0)
    {
        return;
    }
    for (int i = 0; i < number_of_change; i++)
    {
        if (change_log[i] != NULL)
        {
            free(change_log[i]);
        }
    }
    free(change_log);
}
void rename_file()
{
    char original_file[FILE_BUFFER], new_file[FILE_BUFFER];

    // get original file name
    printf("Enter the name of the original file: ");
    if (fgets(original_file, sizeof(original_file), stdin) == NULL)
    {
        printf("\033[31mError: Failed to read input\033[0m\n");
        return;
    }
    // remove trailing \n
    original_file[strcspn(original_file, "\n")] = '\0';
    // check existence for file
    if (access(original_file, F_OK) != 0)
    {
        printf("%s doesn't exist\n", original_file);
        return;
    }

    // get new file name
    printf("Enter the name of the new file: ");
    if (fgets(new_file, sizeof(new_file), stdin) == NULL)
    {
        printf("\033[31mError: Failed to read input\033[0m\n");
        return;
    }
    // remove trailing \n
    new_file[strcspn(new_file, "\n")] = '\0';
    // prevent copying to itself
    if (strcmp(original_file, new_file) == 0)
    {
        printf("You are not allowed to rename a file to itself\n");
        return;
    }
    // check existence for file
    if (access(new_file, F_OK) == 0)
    {
        printf("%s already exists\n", new_file);
        if (get_permission("Do you wish to overwrite (Y/N)? ") == 0)
        {
            return;
        }
        if (remove(new_file) != 0)
        {
            printf("\033[31mError: Failed to delete old version of %s\033[0m\n", new_file);
            return;
        }
    }

    if (rename(original_file, new_file) != 0)
    {
        printf("\033[31mError: Failed to rename file\033[0m\n");
        return;
    }
    printf("\033[32mRenamed file\033[0m\n");
    // add to change log
    char operation[OPERATION_BUFFER];
    snprintf(operation, sizeof(operation), "%d. %s: Renamed from %s (Lines: %d)", number_of_change + 1, new_file, original_file, count_number_of_line(new_file));
    add_change_log(operation);
}
void search_text()
{
    char filename[FILE_BUFFER];
    FILE *file;
    // get file name
    printf("Enter the name of the file: ");
    if (fgets(filename, sizeof(filename), stdin) == NULL)
    {
        printf("\033[31mError: Failed to read input\033[0m\n");
        return;
    }
    // remove trailing \n
    filename[strcspn(filename, "\n")] = '\0';
    // check existence for file
    if (access(filename, F_OK) != 0)
    {
        printf("%s doesn't exist\n", filename);
        return;
    }
    printf("Enter text to search: ");
    char input[INPUT_BUFFER];
    if (fgets(input, sizeof(input), stdin) == NULL)
    {
        printf("\033[31mError: Failed to read input\033[0m\n");
        return;
    }
    input[strcspn(input, "\n")] = '\0';
    file = fopen(filename, "r");
    if (file == NULL)
    {
        printf("\033[31mError: Failed to open file\033[0m\n");
        return;
    }
    char line[INPUT_BUFFER];
    int number_of_line = 0;
    int found = 0;
    while (fgets(line, sizeof(line), file))
    {
        number_of_line++;
        line[strcspn(line, "\n")] = '\0';
        if (strstr(line, input) != NULL)
        {
            printf("Line %d: %s\n", number_of_line, line);
            found = 1;
        }
    }
    fclose(file);
    // text not found
    if (found == 0)
    {
        printf("%s not found in %s\n", input, filename);
    }
}
