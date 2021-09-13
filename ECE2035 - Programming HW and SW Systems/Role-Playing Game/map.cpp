#include "map.h"

#include "globals.h"
#include "graphics.h"

/**
 * The Map structure. This holds a HashTable for all the MapItems, along with
 * values for the width and height of the Map.
 */
struct Map {
    HashTable* items;
    int w, h;
};

unsigned int numBuckets = 2500;
unsigned int numBuckets2 = 200;

/**
 * Storage area for the maps.
 * This is a global variable, but can only be access from this file because it
 * is static.
 */
static Map map[2];
static int active_map;

/**
 * The first step in HashTable access for the map is turning the two-dimensional
 * key information (x, y) into a one-dimensional unsigned integer.
 * This function should uniquely map (x,y) onto the space of unsigned integers.
 */
static unsigned XY_KEY(int X, int Y) {
    return (Y*map[active_map].w+X);
}

/**
 * This is the hash function actually passed into createHashTable. It takes an
 * unsigned key (the output of XY_KEY) and turns it into a hash value (some
 * small non-negative integer).
 */
unsigned map_hash(unsigned key)
{
    if (!active_map)
    {
        return key%(numBuckets);
    }
    else if (active_map == 1)
    {
        return key%(numBuckets2);   
    }
}

void maps_init()
{
        map[0].items = createHashTable(map_hash, numBuckets);
        map[0].w = 50;
        map[0].h = 50;
        map[1].items = createHashTable(map_hash, numBuckets2);
        map[1].w = 30;
        map[1].h = 30;
}

void map1_init()
{
        map[0].items = createHashTable(map_hash, numBuckets);
        map[0].w = 50;
        map[0].h = 50;
}

void maze_init()
{
        map[1].items = createHashTable(map_hash, numBuckets2);
        map[1].w = 30;
        map[1].h = 30;
}


Map* get_active_map()
{
    return &map[active_map];
}   

Map* set_active_map(int m)
{
    active_map = m;
    return &map[active_map];
}

void print_map()
{
    // As you add more types, you'll need to add more items to this array.
    char lookup[] = {'W', 'P'};
    for(int y = 0; y < map_height(); y++)
    {
        for (int x = 0; x < map_width(); x++)
        {
            MapItem* item = get_here(x,y);
            if (item) pc.printf("%c", lookup[item->type]);
            else pc.printf(" ");
        }
        pc.printf("\r\n");
    }
}

int map_width()
{
    return map[active_map].w;
}

int map_height()
{
    return map[active_map].w;
}

int map_area()
{
    return map[active_map].w*map[active_map].h;
}

MapItem* get_north(int x, int y)
{
    Map* current = get_active_map();
    int hashedKey = XY_KEY(x, y-1);
    return (MapItem*)getItem(current->items, hashedKey);
}

MapItem* get_south(int x, int y)
{
    Map* current = get_active_map();
    int hashedKey = XY_KEY(x, y+1);
    return (MapItem*)getItem(current->items, hashedKey);
}

MapItem* get_east(int x, int y)
{
    Map* current = get_active_map();
    int hashedKey = XY_KEY(x+1, y);
    return (MapItem*)getItem(current->items, hashedKey);
}

MapItem* get_west(int x, int y)
{
    Map* current = get_active_map();
    int hashedKey = XY_KEY(x-1, y);
    return (MapItem*)getItem(current->items, hashedKey);
}

MapItem* get_here(int x, int y)
{
    Map* current = get_active_map();
    int hashedKey = XY_KEY(x, y);
    return (MapItem*)getItem(current->items, hashedKey);
}


void map_erase(int x, int y)
{
    removeItem(map[active_map].items, map_hash(XY_KEY(x, y)));
}

void destroyMap(Map* map)
{
    destroyHashTable(map->items);   
}

void createMap(Map* map)
{
    map->items = createHashTable(map_hash, numBuckets); 
}

void createMaze(Map* map)
{
    map->items = createHashTable(map_hash, numBuckets2); 
}

void add_wall(int x, int y, int dir, int len)
{
    for(int i = 0; i < len; i++)
    {
        MapItem* w1 = (MapItem*) malloc(sizeof(MapItem));
        w1->type = WALL;
        w1->draw = draw_wall;
        w1->walkable = false;
        w1->data = NULL;
        unsigned key = (dir == HORIZONTAL) ? XY_KEY(x+i, y) : XY_KEY(x, y+i);
        void* val = insertItem(get_active_map()->items, key, w1);
        if (val) free(val); // If something is already there, free it
    }
}

void add_plant(int x, int y)
{
    MapItem* w1 = (MapItem*) malloc(sizeof(MapItem));
    w1->type = PLANT;
    w1->draw = draw_plant;
    w1->walkable = true;
    w1->data = NULL;
    void* val = insertItem(get_active_map()->items, XY_KEY(x, y), w1);
    if (val) free(val); // If something is already there, free it
}

void add_NPC1(int x, int y)
{
    MapItem* w1 = (MapItem*) malloc(sizeof(MapItem));
    w1->type = NPC1;
    w1->draw = draw_NPC1;
    w1->walkable = false;
    w1->data = NULL;
    void* val = insertItem(get_active_map()->items, XY_KEY(x, y), w1);
    if (val) free(val); // If something is already there, free it
}

void add_ladder(int x, int y)
{
    MapItem* w1 = (MapItem*) malloc(sizeof(MapItem));
    w1->type = ladder;
    w1->draw = draw_ladder;
    w1->walkable = true;
    w1->data = NULL;
    void* val = insertItem(get_active_map()->items, XY_KEY(x, y), w1);
    if (val) free(val); // If something is already there, free it
}

void add_radar(int x, int y)
{
    MapItem* w1 = (MapItem*) malloc(sizeof(MapItem));
    w1->type = radar;
    w1->draw = draw_radar;
    w1->walkable = true;
    w1->data = NULL;
    void* val = insertItem(get_active_map()->items, XY_KEY(x, y), w1);
    if (val) free(val); // If something is already there, free it
}

void add_One(int x, int y)
{
    MapItem* w1 = (MapItem*) malloc(sizeof(MapItem));
    w1->type = One;
    w1->draw = draw_One;
    w1->walkable = true;
    w1->data = NULL;
    void* val = insertItem(get_active_map()->items, XY_KEY(x, y), w1);
    if (val) free(val); // If something is already there, free it
}

void add_Two(int x, int y)
{
    MapItem* w1 = (MapItem*) malloc(sizeof(MapItem));
    w1->type = Two;
    w1->draw = draw_Two;
    w1->walkable = true;
    w1->data = NULL;
    void* val = insertItem(get_active_map()->items, XY_KEY(x, y), w1);
    if (val) free(val); // If something is already there, free it
}

void add_Three(int x, int y)
{
    MapItem* w1 = (MapItem*) malloc(sizeof(MapItem));
    w1->type = Three;
    w1->draw = draw_Three;
    w1->walkable = true;
    w1->data = NULL;
    void* val = insertItem(get_active_map()->items, XY_KEY(x, y), w1);
    if (val) free(val); // If something is already there, free it
}

void add_Four(int x, int y)
{
    MapItem* w1 = (MapItem*) malloc(sizeof(MapItem));
    w1->type = Four;
    w1->draw = draw_Four;
    w1->walkable = true;
    w1->data = NULL;
    void* val = insertItem(get_active_map()->items, XY_KEY(x, y), w1);
    if (val) free(val); // If something is already there, free it
}

void add_Five(int x, int y)
{
    MapItem* w1 = (MapItem*) malloc(sizeof(MapItem));
    w1->type = Five;
    w1->draw = draw_Five;
    w1->walkable = true;
    w1->data = NULL;
    void* val = insertItem(get_active_map()->items, XY_KEY(x, y), w1);
    if (val) free(val); // If something is already there, free it
}

void add_Six(int x, int y)
{
    MapItem* w1 = (MapItem*) malloc(sizeof(MapItem));
    w1->type = Six;
    w1->draw = draw_Six;
    w1->walkable = true;
    w1->data = NULL;
    void* val = insertItem(get_active_map()->items, XY_KEY(x, y), w1);
    if (val) free(val); // If something is already there, free it
}

void add_Seven1(int x, int y)
{
    MapItem* w1 = (MapItem*) malloc(sizeof(MapItem));
    w1->type = Seven1;
    w1->draw = draw_Seven1;
    w1->walkable = true;
    w1->data = NULL;
    void* val = insertItem(get_active_map()->items, XY_KEY(x, y), w1);
    if (val) free(val); // If something is already there, free it
}

void add_lava(int x, int y, int dir, int len)
{
    for(int i = 0; i < len; i++)
    {
        MapItem* w1 = (MapItem*) malloc(sizeof(MapItem));
        w1->type = lava;
        w1->draw = draw_lava;
        w1->walkable = true;
        w1->data = NULL;
        unsigned key = (dir == HORIZONTAL) ? XY_KEY(x+i, y) : XY_KEY(x, y+i);
        void* val = insertItem(get_active_map()->items, key, w1);
        if (val) free(val); // If something is already there, free it
    }
}

void add_Frieza(int x, int y)
{
    MapItem* w1 = (MapItem*) malloc(sizeof(MapItem));
    w1->type = Frieza;
    w1->draw = draw_Frieza;
    w1->walkable = false;
    w1->data = NULL;
    void* val = insertItem(get_active_map()->items, XY_KEY(x, y), w1);
    if (val) free(val); // If something is already there, free it
}

void add_Seven2(int x, int y)
{
    MapItem* w1 = (MapItem*) malloc(sizeof(MapItem));
    w1->type = Seven2;
    w1->draw = draw_Seven2;
    w1->walkable = true;
    w1->data = NULL;
    void* val = insertItem(get_active_map()->items, XY_KEY(x, y), w1);
    if (val) free(val); // If something is already there, free it
}

void add_Beam(int x, int y)
{
    MapItem* w1 = (MapItem*) malloc(sizeof(MapItem));
    w1->type = Beam;
    w1->draw = draw_Beam;
    w1->walkable = false;
    w1->data = NULL;
    void* val = insertItem(get_active_map()->items, XY_KEY(x, y), w1);
    if (val) free(val); // If something is already there, free it
}

void add_Sword(int x, int y)
{
    MapItem* w1 = (MapItem*) malloc(sizeof(MapItem));
    w1->type = Sword;
    w1->draw = draw_Sword;
    w1->walkable = true;
    w1->data = NULL;
    void* val = insertItem(get_active_map()->items, XY_KEY(x, y), w1);
    if (val) free(val); // If something is already there, free it
}

void add_Exclamation(int x, int y)
{
    MapItem* w1 = (MapItem*) malloc(sizeof(MapItem));
    w1->type = Exclamation;
    w1->draw = draw_Exclamation;
    w1->walkable = true;
    w1->data = NULL;
    void* val = insertItem(get_active_map()->items, XY_KEY(x, y), w1);
    if (val) free(val); // If something is already there, free it
}

void add_Sparkle(int x, int y)
{
    MapItem* w1 = (MapItem*) malloc(sizeof(MapItem));
    w1->type = Sparkle;
    w1->draw = draw_Sparkle;
    w1->walkable = true;
    w1->data = NULL;
    void* val = insertItem(get_active_map()->items, XY_KEY(x, y), w1);
    if (val) free(val); // If something is already there, free it
}

void add_Treasure(int x, int y)
{
    MapItem* w1 = (MapItem*) malloc(sizeof(MapItem));
    w1->type = Treasure;
    w1->draw = draw_Treasure;
    w1->walkable = true;
    w1->data = NULL;
    void* val = insertItem(get_active_map()->items, XY_KEY(x, y), w1);
    if (val) free(val); // If something is already there, free it
}

void add_Piccolo(int x, int y)
{
    MapItem* w1 = (MapItem*)malloc(sizeof(MapItem));
    w1->type = Piccolo;
    w1->draw = draw_Piccolo;
    w1->walkable = false;
    w1->data = NULL;
    void* val = insertItem(get_active_map()->items, XY_KEY(x, y), w1);
    if (val) free(val); // If something is already there, free it
}