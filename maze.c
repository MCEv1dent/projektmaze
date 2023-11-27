// Autor: Miroslav Šich
// Program ma za ukol vyresit trojuhelnikove bludiste zpusobem drzeni se prave nebo leve steny. Neumi najit nejkratsi cestu.
// Prekladaci argumenty: gcc -std=c11 -Wall -Wextra -Werror maze.c -o maze

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

typedef struct // Struct pro praci s mapou
{
    int rows;
    int cols;
    unsigned char *cells;
} Map;

// Deklarace vsech funkci pro pouziti
Map loadMap(bool *error, const char *filename);
void freeMap(Map *map);
bool isborder(Map *map, int r, int c, int border);
bool find_way(Map *map, int r, int c, int entry, int leftright);
int start_border(Map *map, int r, int c);

Map loadMap(bool *error, const char *filename) // Funkce Loadmap
{
    Map map;

    FILE *file = fopen(filename, "r"); // Otevreni souboru
    if (file == NULL)                  // Pokud neexistuje/nepodarilo se
    {
        *error = true;
        return map;
    }

    if (fscanf(file, "%d %d", &map.rows, &map.cols) != 2) // Pokud se nepodarilo nacteni rows a cols
    {
        *error = true;
        fclose(file);
        return map;
    }

    map.cells = (unsigned char *)malloc(map.rows * map.cols * sizeof(unsigned char)); // Vytvoreni malloc pro cells
    if (map.cells == NULL)                                                            // Pokud se nepodaril malloc do cells
    {
        *error = true;
        fclose(file);
        return map;
    }

    for (int i = 0; i < map.rows; ++i)
    {
        for (int j = 0; j < map.cols; ++j)
        {
            if (fscanf(file, "%hhu", &map.cells[i * map.cols + j]) == -1) // Nacteni hodnot do cells (%hhu je ciselny output z unsigned charu)
            {
                *error = true;
                fclose(file);
                freeMap(&map);
                return map;
            }
        }
    }

    for (int row = 0; row < map.rows; row++)
    {
        for (int col = 0; col < map.cols; col++)
        {
            if (map.cells[row * map.cols + col] > 7) // Kontroluje, jestli je hodnota policka pouze od 0 do 7
            {
                *error = true;
                fclose(file);
                freeMap(&map);
                return map;
            }
            if (col + 1 < map.cols) // Kontroluje, jestli sedi left right border
            {
                if (isborder(&map, row, col, 2) != isborder(&map, row, col + 1, 1))
                {
                    *error = true;
                    fclose(file);
                    freeMap(&map);
                    return map;
                }
            }
            if ((row + col) % 2 && row + 1 < map.rows) // Kontroluje top/bottom border
            {
                if (isborder(&map, row, col, 3) != isborder(&map, row + 1, col, 3))
                {
                    *error = true;
                    fclose(file);
                    freeMap(&map);
                    return map;
                }
            }
        }
    }

    fclose(file);

    return map;
}

void freeMap(Map *map) // Uvolneni pameti
{
    free(map->cells);
    map->cells = NULL;
}

bool isborder(Map *map, int r, int c, int border)
{
    int value = map->cells[r * map->cols + c]; // Nacte hodnotu policka (pr. pole 6,1 = 5 * 6 + 1 = 31)

    if (border == 1)                 // left border
        return (value & 0b001) != 0; // Vraci kdyz je border, jinak ne
    else if (border == 2)            // right border
        return (value & 0b010) != 0; // Vraci kdyz je border, jinak ne
    else                             // top/bottom border
        return (value & 0b100) != 0; // Vraci kdyz je border, jinak ne
}

bool find_way(Map *map, int r, int c, int entry, int leftright)
{
    if (r > map->rows - 1 || r < 0 || c > map->cols - 1 || c < 0)
    {
        return 1;
    }

    if (leftright == 0) // Pokud pravidlo prave ruky
    {
        for (int i = 0; i < 3; i++) // Meni steny, v nejhorsim pripade se proste vraci tou stenou, kterou prisel
        {
            int new_entry = (r + c) % 2 == 0 ? (entry - 1 + i) % 3 + 1 : (entry + 2 - i) % 3 + 1;
            // Trojuhelnik a entry 1 - new_entry = 1, 3, 2
            // Trojuhelnik a entry 2 - new_entry = 2, 1, 3
            // Trojuhelnik a entry 3 - new_entry = 3, 2, 1
            // Obraceny trojuhelnik a entry 1 - new_entry = 1, 2, 3
            // Obraceny trojuhelnik a entry 2 - new_entry = 2, 3, 1
            // Obraceny trojuhelnik a entry 3 - new_entry = 3, 1, 2
            if (new_entry == 1)
            {
                if (!isborder(map, r, c, (r + c) % 2 == 0 ? 2 : 3)) // pokud neni hrana a policko trojuhelnik jde do bottom, pro obraceny jde do right
                {
                    int new_r = (r + c) % 2 == 0 ? r : r + 1;
                    int new_c = (r + c) % 2 == 0 ? c + 1 : c;
                    printf("%d,%d\n", r + 1, c + 1);
                    find_way(map, new_r, new_c, (r + c) % 2 == 0 ? 1 : 3, 0); // pokud policko trojuhelnik je entry left, pro obraceny je top
                    break;
                }
            }
            else if (new_entry == 2)
            {
                if (!isborder(map, r, c, (r + c) % 2 == 0 ? 3 : 1)) // pokud neni hrana a policko trojuhelnik jde do left, pro obraceny jde do top
                {
                    int new_r = (r + c) % 2 == 0 ? r - 1 : r;
                    int new_c = (r + c) % 2 == 0 ? c : c - 1;
                    printf("%d,%d\n", r + 1, c + 1);
                    find_way(map, new_r, new_c, (r + c) % 2 == 0 ? 3 : 2, 0); // pokud policko trojuhelnik je entry bottom, pro obraceny je right
                    break;
                }
            }
            else if (new_entry == 3)
            {
                if (!isborder(map, r, c, (r + c) % 2 == 0 ? 1 : 2)) // pokud neni hrana a policko trojuhelnik jde do right, pro obraceny jde do left
                {
                    int new_r = r;
                    int new_c = (r + c) % 2 == 0 ? c - 1 : c + 1;
                    printf("%d,%d\n", r + 1, c + 1);
                    find_way(map, new_r, new_c, (r + c) % 2 == 0 ? 2 : 1, 0); // pokud policko trojuhelnik je entry right, pro obraceny je left
                    break;
                }
            }
        }
    }
    else if (leftright == 1) // Pokud pravidlo leve ruky
    {
        for (int i = 0; i < 3; i++) // Meni steny, v nejhorsim pripade se proste vraci tou stenou, kterou prisel
        {
            int new_entry = (r + c) % 2 != 0 ? (entry - 1 + i) % 3 + 1 : (entry + 2 - i) % 3 + 1;
            // Trojuhelnik a entry 1 - new_entry = 1, 2, 3
            // Trojuhelnik a entry 2 - new_entry = 2, 3, 1
            // Trojuhelnik a entry 3 - new_entry = 3, 1, 2
            // Obraceny trojuhelnik a entry 1 - new_entry = 1, 3, 2
            // Obraceny trojuhelnik a entry 2 - new_entry = 2, 1, 3
            // Obraceny trojuhelnik a entry 3 - new_entry = 3, 2, 1
            if (new_entry == 1)
            {
                if (!isborder(map, r, c, (r + c) % 2 != 0 ? 2 : 3)) // pokud neni hrana a policko trojuhelnik jde do right, pro obraceny jde do top
                {
                    int new_r = (r + c) % 2 == 0 ? r - 1 : r;
                    int new_c = (r + c) % 2 == 0 ? c : c + 1;
                    printf("%d,%d\n", r + 1, c + 1);
                    find_way(map, new_r, new_c, (r + c) % 2 != 0 ? 1 : 3, 1); // pokud policko trojuhelnik je entry bottom, pro obraceny je left
                    break;
                }
            }
            else if (new_entry == 2)
            {
                if (!isborder(map, r, c, (r + c) % 2 != 0 ? 3 : 1)) // pokud neni hrana a policko trojuhelnik jde do bottom, pro obraceny jde do left
                {
                    int new_r = (r + c) % 2 == 0 ? r : r + 1;
                    int new_c = (r + c) % 2 == 0 ? c - 1 : c;
                    printf("%d,%d\n", r + 1, c + 1);
                    find_way(map, new_r, new_c, (r + c) % 2 != 0 ? 3 : 2, 1); // pokud policko trojuhelnik je entry right, pro obraceny je top
                    break;
                }
            }
            else if (new_entry == 3)
            {
                if (!isborder(map, r, c, (r + c) % 2 != 0 ? 1 : 2)) // pokud neni hrana a policko trojuhelnik jde do left, pro obraceny jde do right
                {
                    int new_r = r;
                    int new_c = (r + c) % 2 == 0 ? c + 1 : c - 1;
                    printf("%d,%d\n", r + 1, c + 1);
                    find_way(map, new_r, new_c, (r + c) % 2 != 0 ? 2 : 1, 1); // pokud policko trojuhelnik je entry left, pro obraceny je right
                    break;
                }
            }
        }
    }
    return true;
}

int start_border(Map *map, int r, int c) // Funkce pro hledani startovni hrany
{
    if (c == 1)
    {
        if (!isborder(map, r - 1, c - 1, 1)) // jestli je hrana left
        {
            return 1;
        }
        else if (r == map->rows && !isborder(map, r - 1, c - 1, 3)) // dolni roh
        {
            return 3;
        }
        else if (r == 1 && !isborder(map, r - 1, c - 1, 3)) // horni roh
        {
            return 3;
        }
        else
        {
            return 0;
        }
    }
    else if (c == map->cols)
    {
        if (!isborder(map, r - 1, c - 1, 2)) // jestli je hrana right
        {
            return 2;
        }
        else if (r == map->rows && !isborder(map, r - 1, c - 1, 3)) // dolni roh
        {
            return 3;
        }
        else if (r == 1 && !isborder(map, r - 1, c - 1, 3)) // horni roh
        {
            return 3;
        }
        else
        {
            return 0;
        }
    }
    else
    {
        if ((r == 1 && (r + c) % 2 == 0) || (r == map->rows && (r + c) % 2 == 1))
        {
            if (!isborder(map, r - 1, c - 1, 3)) // jestli je hrana top/bottom
            {
                return 3;
            }
        }
        return 0;
    }
}

int main(int argc, char *argv[])
{
    if (argc == 2)
    {
        const char *option = argv[1];
        if (strcmp(option, "--help") == 0)
        {
            printf("Pouziti programu:\n'--help' - zobrazi dostupne prikazy\n'--test soubor.txt' - vyhodnoti platnost bludiste\n'--rpath R C soubor.txt' - najde cestu ze zadanych souradnic ven drzenim se prave steny\n'--lpath R C soubor.txt' - najde cestu ze zadanych souradnic ven drzenim se leve steny\n");
            return 0;
        }
        else
        {
            printf("ERROR unknown command.\n");
            return 1;
        }
    }
    if (argc == 3)
    {
        const char *option = argv[1];
        const char *filename = argv[2];
        bool error;
        if (strcmp(option, "--test") == 0)
        {
            Map map = loadMap(&error, filename);
            if (error == true)
            {
                printf("Invalid\n");
                return 1;
            }
            printf("Valid\n");
            freeMap(&map);
            return 0;
        }
        else
        {
            printf("ERROR unknown command.\n");
            return 1;
        }
    }
    if (argc == 5)
    {
        const char *option = argv[1];
        const char *filename = argv[4];
        int rows = atoi(argv[2]);
        int cols = atoi(argv[3]);
        bool error;

        Map map = loadMap(&error, filename);
        if (error == true)
        {
            printf("Invalid file or maze.\n");
            return 1;
        }
        if (strcmp(option, "--rpath") == 0)
        {
            int a = start_border(&map, rows, cols);
            if (a == 0)
            {
                printf("Invalid entry point.\n");
                freeMap(&map);
                return 1;
            }
            find_way(&map, rows - 1, cols - 1, a, 0);
        }
        else if (strcmp(option, "--lpath") == 0)
        {
            int a = start_border(&map, rows, cols);
            if (a == 0)
            {
                printf("Invalid entry point.\n");
                freeMap(&map);
                return 1;
            }
            find_way(&map, rows - 1, cols - 1, a, 1);
        }
        else
        {
            printf("ERROR unknown command, but 5 arguments so enjoy the hidden message.\n");
            freeMap(&map);
            return 1;
        }
        freeMap(&map);
    }
    else
    {
        printf("ERROR unknown command.\n");
        return 1;
    }
    return 0;
}