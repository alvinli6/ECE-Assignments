#ifndef MAP_H
#define MAP_H

#include "hash_table.h"

/**
 * A structure to represent the map. The implementation is private.
 */
struct Map;

// A function pointer type for drawing MapItems.
// All tiles are 11x11 blocks.
// u,v is the top left corner pixel of the block
typedef void (*DrawFunc)(int u, int v);

/**
 * The data for elements in the map. Each item in the map HashTable is a
 * MapItem.
 */
typedef struct {
    /**
     * Indicates the "type" of the MapItem: WALL, DOOR, PLANT, etc. This is
     * useful for determining how to interact with the object when updating the
     * game state.
     */
    int type;
    
    /**
     * A function pointer to the drawing function for this item. Used by draw_game.
     */
    DrawFunc draw;
    
    /**
     * If zero, this item should block character motion.
     */
    int walkable;
    
    /**
     * Arbitrary extra data for the MapItem. Could be useful for keeping up with
     * special information, like where a set of stairs should take the player.
     * 
     * Iterpretation of this can depend on the type of the MapItem. For example,
     * a WALL probably doesn't need to use this (it can be NULL), where an NPC
     * might use it to store game state (have I given the player the key yet?).
     */
    void* data;
} MapItem;

typedef struct {
    int tm;
    int tx, ty;
} StairsData;

// MapItem types
// Define more of these!
#define WALL    0
#define PLANT   1
#define NPC1    2
#define ladder  3
#define radar   4
#define One     5
#define Two     6
#define Three   7
#define Four    8
#define Five    9
#define Six     10
#define Seven1   11
#define lava    12
#define Frieza  13
#define Seven2  14
#define Beam    15
#define Sword   16
#define Exclamation 17
#define Sparkle 18
#define Treasure  19
#define Piccolo 20
/**
 * Initializes the internal structures for all maps. This does not populate
 * the map with items, but allocates space for them, initializes the hash tables, 
 * and sets the width and height.
 */
void maps_init();

/**
 * Returns a pointer to the active map.
 */
Map* get_active_map();

/**
 * Sets the active map to map m, where m is the index of the map to activate.
 * Returns a pointer to the new active map.
 */
Map* set_active_map(int m);

/**
 * Returns the map m, regardless of whether it is the active map. This function
 * does not change the active map.
 */
Map* get_map(int m);

/**
 * Print the active map to the serial console.
 */
void print_map();

// Access
/**
 * Returns the width of the active map.
 */
int map_width();

/**
 * Returns the heigh of the active map.
 */
int map_height();

/**
 * Returns the total number of cells in the active map.
 */
int map_area();

/**
 * Returns the MapItem immediately above the given location.
 */
MapItem* get_north(int x, int y);

/**
 * Returns the MapItem immediately below the given location.
 */
MapItem* get_south(int x, int y);

/**
 * Returns the MapItem immediately to the right of the given location.
 */
MapItem* get_east(int x, int y);

/**
 * Returns the MapItem immediately to the left of  the given location.
 */
MapItem* get_west(int x, int y);

/**
 * Returns the MapItem at the given location.
 */
MapItem* get_here(int x, int y);

// Directions, for using the modification functions
#define HORIZONTAL  0
#define VERTICAL    1

/**
 * If there is a MapItem at (x,y), remove it from the map.
 */
void map_erase(int x, int y);

/**
 * Add WALL items in a line of length len beginning at (x,y).
 * If dir == HORIZONTAL, the line is in the direction of increasing x.
 * If dir == VERTICAL, the line is in the direction of increasing y.
 *
 * If there are already items in the map that collide with this line, they are
 * erased.
 */
void add_wall(int x, int y, int dir, int len);

/**
 * Add a PLANT item at (x,y). If there is already a MapItem at (x,y), erase it
 * before adding the plant.
 */
void add_plant(int x, int y);

/**
 * Adds the NPC at (x,y).
 */
void add_NPC1(int x, int y);
 
/**
 * Adds ladder to next map at (x,y)
 */
 
void add_ladder(int x, int y);

/**
 * Adds radar to maze at (x,y)
 */
void add_radar(int x, int y);
/**
 *
 */
void add_One(int x, int y);
void add_Two(int x, int y);
void add_Three(int x, int y);
void add_Four(int x, int y);
void add_Five(int x, int y);
void add_Six(int x, int y);
void add_Seven1(int x, int y);
void add_Seven2(int x, int y);
void add_Beam(int x, int y);
void add_Sword(int x, int y);
void add_lava(int x, int y, int dir, int len);
void add_Frieza(int x, int y);
void add_Exclamation(int x, int y);
void add_Sparkle(int x, int y);
void add_Treasure(int x, int y);
void add_Piccolo(int x, int y);

void destroyMap(Map* map);
 
void createMap(Map* map);
void createMaze(Map* map);

void map1_init();
void maze_init();
 
#endif //MAP_H