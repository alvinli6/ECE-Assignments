#ifndef GRAPHICS_H
#define GRAPHICS_H


/**
 * Draws the player. This depends on the player state, so it is not a DrawFunc.
 */
void draw_player(int u, int v, int key);

void draw_GodMode(int u, int v, int key);

/**
 * Draws the NPC. 
 */
void draw_NPC1(int u, int v);

/**
 * Draws the ladder to next map
 */
void draw_ladder(int u, int v);

/**
 * Draws the radar in maze
 */
void draw_radar(int u, int v);

void draw_One(int u, int v);
void draw_Two(int u, int v);
void draw_Three(int u, int v);
void draw_Four(int u, int v);
void draw_Five(int u, int v);
void draw_Six(int u, int v);
void draw_Seven1(int u, int v);
void draw_Seven2(int u, int v);
void draw_lava(int u, int v);
void draw_Frieza(int u, int v);
void draw_Beam(int u, int v);
void draw_Sword(int u, int v);
void draw_Exclamation(int u, int v);
void draw_Sparkle(int u, int v);
void draw_Treasure(int u, int v);
void draw_Piccolo(int u, int v);

/**
 * Takes a string image and draws it to the screen. The string is 121 characters
 * long, and represents an 11x11 tile in row-major ordering (across, then down,
 * like a regular multi-dimensional array). The available colors are:
 *      R = Red
 *      Y = Yellow
 *      G = Green
 *      D = Brown ("dirt")
 *      5 = Light grey (50%)
 *      3 = Dark grey (30%)
 *      Any other character is black
 * More colors can be easily added by following the pattern already given.
 */
void draw_img(int u, int v, const char* img);

/**
 * DrawFunc functions. 
 * These can be used as the MapItem draw functions.
 */
void draw_nothing(int u, int v);
void draw_wall(int u, int v);
void draw_plant(int u, int v);

/**
 * Draw the upper status bar.
 */
void draw_upper_status();

/**
 * Draw the lower status bar.
 */ 
void draw_lower_status();

/**
 * Draw the border for the map.
 */
void draw_border();

#endif // GRAPHICS_H