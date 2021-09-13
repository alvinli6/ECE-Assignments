// Project includes
#include "globals.h"
#include "hardware.h"
#include "map.h"
#include "graphics.h"
#include "speech.h"

Map* firstMap = set_active_map(0);
Map* secondMap = set_active_map(1);
int currentMap = 0;
int talkedToBlue = 0;
int OMode = 0;
int seven1X = rand()%50;
int seven1Y = rand()%50;
int gotFake = 0;
int gotFake2 = 0;
int seven2Here = 0;
int ballCount = 0;
int gotTreasure = 0;
int picX = 25;
int picY = 25;
unsigned int health = 100;
// Functions in this file
int get_action (GameInputs inputs);
int update_game (int action);
void draw_game (int init);
void init_main_map ();
void init_maze_map();
void init_main_map2();
int main ();

/**
 * The main game state. Must include Player locations and previous locations for
 * drawing to work properly. Other items can be added as needed.
 */
struct {
    int x,y;    // Current locations
    int px, py; // Previous locations
    int has_key;
    // You can add other properties for the player here
} Player;


/**
 * Given the game inputs, determine what kind of update needs to happen.
 * Possbile return values are defined below.
 */
#define NO_ACTION 0
#define ACTION_BUTTON 1
#define MENU_BUTTON 2
#define GO_LEFT 3
#define GO_RIGHT 4
#define GO_UP 5
#define GO_DOWN 6

int get_action(GameInputs inputs)
{
    if (!inputs.b1)
    {
        return ACTION_BUTTON;    
    }
    else if (!inputs.b2)
    {
        OMode = !OMode;
    }
    else if (!inputs.b3)
    {
        return MENU_BUTTON;   
    }
    else if (inputs.ax > 0.2)
    {
        return GO_RIGHT;    
    }
    else if (inputs.ax < -0.2)
    {
        return GO_LEFT;   
    }
    else if (inputs.ay > 0.2)
    {
        return GO_UP;    
    }
    else if (inputs.ay < -0.2)
    {
        return GO_DOWN;    
    }
    return NO_ACTION;
}

/**
 * Update the game state based on the user action. For example, if the user
 * requests GO_UP, then this function should determine if that is possible by
 * consulting the map, and update the Player position accordingly.
 * 
 * Return values are defined below. FULL_DRAW indicates that for this frame,
 * draw_game should not optimize drawing and should draw every tile, even if
 * the player has not moved.
 */
#define NO_RESULT 0
#define GAME_OVER 1
#define FULL_DRAW 2
int update_game(int action)
{
    // Save player previous location before updating
    Player.px = Player.x;
    Player.py = Player.y;
    
    
    // Do different things based on the each action.
    // You can define functions like "go_up()" that get called for each case.
    switch(action)
    {
        case GO_UP:
             if ((get_north(Player.x, Player.y)->walkable == true || get_north(Player.x, Player.y) == NULL || OMode) && (Player.y != 0))
             {
                 Player.y--;
                 if (!currentMap && ballCount != 6)
                 {
                    map_erase(picX, picY);
                    draw_game(FULL_DRAW);
                    picY++;
                    add_Piccolo(picX, picY);
                 }
                 return FULL_DRAW;
             }
             break;
        case GO_LEFT:   
             if ((get_west(Player.x, Player.y)->walkable == true || get_west(Player.x, Player.y) == NULL || OMode) && (Player.x != 0))
             {
                 Player.x--;
                 if (!currentMap && ballCount != 6)
                 {
                    map_erase(picX, picY);
                    draw_game(FULL_DRAW);
                    picX++;
                    add_Piccolo(picX, picY);
                 }
                 return FULL_DRAW;
             }
             break;            
        case GO_DOWN:   
             if ((get_south(Player.x, Player.y)->walkable == true || get_south(Player.x, Player.y) == NULL || OMode) && (Player.y != 49))
             {
                 Player.y++;
                 if (!currentMap && ballCount != 6)
                 {
                    map_erase(picX, picY);
                    draw_game(FULL_DRAW);
                    picY--;
                    add_Piccolo(picX, picY);
                 }
                 return FULL_DRAW;
             }
             break;
        case GO_RIGHT:  
             if ((get_east(Player.x, Player.y)->walkable == true || get_east(Player.x, Player.y) == NULL || OMode) && (Player.x != 49))
             {
                 Player.x++;
                 if (!currentMap && ballCount != 6)
                 {
                    map_erase(picX, picY);
                    draw_game(FULL_DRAW);
                    picX--;
                    add_Piccolo(picX, picY);
                 }
                 return FULL_DRAW;
             }
             break;
        case NO_ACTION:
             break;
        case ACTION_BUTTON: 
             if (get_here(Player.x, Player.y)->type != Treasure && get_here(Player.x, Player.y)->type != PLANT && get_here(Player.x, Player.y)->type != Sword && (!Player.has_key) && get_here(Player.x, Player.y)->type != radar && get_here(Player.x, Player.y)->type != One && get_here(Player.x, Player.y)->type != Two && get_here(Player.x, Player.y)->type != Three && get_here(Player.x, Player.y)->type != Four && get_here(Player.x, Player.y)->type != Five && get_here(Player.x, Player.y)->type != Six && get_here(Player.x, Player.y)->type != Seven1 && get_here(Player.x, Player.y)->type != ladder && get_north(Player.x, Player.y)->type != NPC1 && get_south(Player.x, Player.y)->type != NPC1 && get_east(Player.x, Player.y)->type != NPC1 && get_west(Player.x, Player.y)->type != NPC1)
             {
                speech("You do not", "have radar!");   
                speech("Or journey", "very far away");
                speech("for the", "Z-SWORD");
             }
             if (get_north(Player.x, Player.y)->type == NPC1 || get_south(Player.x, Player.y)->type == NPC1 || get_east(Player.x, Player.y)->type == NPC1 || get_west(Player.x, Player.y)->type == NPC1)
             {
                speech("Bulma:", "Find the ladder");  
                talkedToBlue = 1;
             }
             if (get_here(Player.x, Player.y)->type == ladder)
             {
                 if (!currentMap)
                 {
                    for (int i = 0; i < 50; i++)
                    {
                        for (int j = 0; j < 50; j++)
                        {
                            map_erase(i, j);   
                        }   
                    }
                    init_maze_map();
                    set_active_map(1);
                    currentMap = 1;
                    return FULL_DRAW;
                 }
                 else if (currentMap == 1)
                 {
                    for (int i = 0; i < 30; i++)
                    {
                        for (int j = 0; j < 30; j++)
                        {
                            map_erase(i, j);   
                        } 
                    }
                    destroyMap(get_active_map());
                    init_main_map();
                    currentMap = 0;
                    return FULL_DRAW;
                 }
             }
             if (get_here(Player.x, Player.y)->type == One)
             {
                speaker.period(1.0/493.88);
                speaker = 0.05;
                wait(0.2);
                speaker.period(1.0/587.33);
                speaker = 0.05;
                wait(0.2);
                speaker.period(1.0/880);
                speaker = 0.05;
                wait(0.2);
                speaker = 0;
                speech("South for", "two-star ball"); 
                map_erase(8, 10); 
                ballCount++;
             }
             if (get_here(Player.x, Player.y)->type == Two)
             {
                speaker.period(1.0/493.88);
                speaker = 0.05;
                wait(0.2);
                speaker.period(1.0/587.33);
                speaker = 0.05;
                wait(0.2);
                speaker.period(1.0/880);
                speaker = 0.05;
                wait(0.2);
                speaker = 0;  
                speech("East for", "three-star ball"); 
                map_erase(8, 20);
                ballCount++;
             }
             if (get_here(Player.x, Player.y)->type == Three)
             {
                speaker.period(1.0/493.88);
                speaker = 0.05;
                wait(0.2);
                speaker.period(1.0/587.33);
                speaker = 0.05;
                wait(0.2);
                speaker.period(1.0/880);
                speaker = 0.05;
                wait(0.2);
                speaker = 0;
                speech("East for", "four-star ball"); 
                map_erase(20, 20);  
                ballCount++;
             }
             if (get_here(Player.x, Player.y)->type == Four)
             {
                speaker.period(1.0/493.88);
                speaker = 0.05;
                wait(0.2);
                speaker.period(1.0/587.33);
                speaker = 0.05;
                wait(0.2);
                speaker.period(1.0/880);
                speaker = 0.05;
                wait(0.2);
                speaker = 0;
                speech("East for", "five-star ball"); 
                map_erase(30, 20);  
                ballCount++;
             }
             if (get_here(Player.x, Player.y)->type == Five)
             {
                speaker.period(1.0/493.88);
                speaker = 0.05;
                wait(0.2);
                speaker.period(1.0/587.33);
                speaker = 0.05;
                wait(0.2);
                speaker.period(1.0/880);
                speaker = 0.05;
                wait(0.2);
                speaker = 0;
                speech("South for", "six-star ball"); 
                map_erase(40, 20);  
                ballCount++;
             }
             if (get_here(Player.x, Player.y)->type == Six)
             { 
                speaker.period(1.0/493.88);
                speaker = 0.05;
                wait(0.2);
                speaker.period(1.0/587.33);
                speaker = 0.05;
                wait(0.2);
                speaker.period(1.0/880);
                speaker = 0.05;
                wait(0.2);
                speaker = 0;
                speech("Find dragon radar", "for seven-star!");
                map_erase(40, 35);  
                ballCount++;
             }
             if (!gotFake2 && Player.has_key && Player.x == 31 && Player.y == 17 && !currentMap)
             {
                map_erase(31, 17);
                add_plant(31, 17);  
                gotFake2 = 1;
                draw_game(FULL_DRAW);
                speech("This one", "is fake!");  
             }
             if (get_here(Player.x, Player.y)->type == Seven1)
             {
                 map_erase(seven1X, seven1Y);
                 add_plant(seven1X, seven1Y);
                 draw_game(FULL_DRAW);
                 speaker.period(1.0/176.02);
                 speaker = 0.05;
                 wait(0.5);
                 speaker = 0;
                 speech("Frieza stole", "the seven-star!");
                 speech("Go defeat him!!!", "");
    
                 gotFake = 1;
                 seven2Here = 1;
             }
             if (get_here(Player.x, Player.y)->type == radar)
             {
                 Player.has_key = 1;
                 speech("You got the", "dragon radar!");
                 add_plant(25, 15);
             }
             if (!seven2Here && Player.has_key && get_here(Player.x, Player.y) == NULL && get_north(Player.x, Player.y)->type != NPC1 && get_south(Player.x, Player.y)->type != NPC1 && get_east(Player.x, Player.y)->type != NPC1 && get_west(Player.x, Player.y)->type != NPC1)
             {
                if ((Player.x - seven1X) > 0 && (Player.y == seven1Y))
                {
                    speech("West", "");  
                }   
                else if ((Player.x - seven1X < 0) && (Player.y == seven1Y))
                {
                    speech("East", "");   
                }
                else if ((Player.y - seven1Y > 0) && (Player.x == seven1X))
                {
                    speech("North", "");   
                }
                else if ((Player.y - seven1Y < 0) && (Player.x == seven1X))
                {
                    speech("South", "");   
                }
                else if ((Player.x - seven1X > 0) && (Player.y - seven1Y > 0))
                {
                    speech("North", "west");
                }
                else if ((Player.x - seven1X > 0) && (Player.y - seven1Y < 0))
                {
                    speech("South", "west");
                }
                else if ((Player.x - seven1X < 0) && (Player.y - seven1Y > 0))
                {
                    speech("North", "east");
                }
                else if ((Player.x - seven1X < 0) && (Player.y - seven1Y < 0))
                {
                    speech("South", "east");
                }
             }
             if (gotFake && Player.y == 21 && Player.x <= 5)
             {
                add_Exclamation(0, 20);
                draw_game(FULL_DRAW);
                for (int i = Player.x - 1; i >= 0; i--)
                {
                    add_Beam(i, 21);
                    wait(1); 
                    draw_game(FULL_DRAW);  
                }  
                map_erase(0, 20);
                draw_game(FULL_DRAW);
                for (int i = 0; i < Player.x; i++)
                {
                    map_erase(i, 21);
                    draw_game(FULL_DRAW);   
                }
                add_Seven2(0, 21);
             }
             if (get_here(Player.x, Player.y)->type == Seven2)
             {
                ballCount++;
             }
             if (get_here(Player.x, Player.y)->type == Sword)
             {
                uLCD.cls();
                uLCD.color(RED);
                uLCD.set_font_size(2, 2);
                uLCD.printf("YOU HAVE FOUND\nTHE STRONGEST\nWEAPON IN\nTHE UNIVERSE.");
                uLCD.cls();
                uLCD.printf("\n\n\n\n       GAME\n\n\n       OVER\n");   
                while(1)
                {
                    wait(1);   
                }
             }
             if (get_here(Player.x, Player.y)->type == PLANT)
             {
                add_Exclamation(Player.x, Player.y - 1);
                map_erase(Player.x, Player.y);
                draw_game(FULL_DRAW);
                wait(0.5);
                map_erase(Player.x, Player.y - 1);
             }
             if (get_here(Player.x, Player.y)->type == Treasure)
             {
                map_erase(Player.x, Player.y);
                draw_game(FULL_DRAW);
                speech("You found the", "treasure!!!");
                gotTreasure = 1;   
             }
             break;
        case MENU_BUTTON: 
             uLCD.rectangle(0, 0, 127, 127, BLUE);
             uLCD.filled_rectangle(1, 1, 126, 126, BLACK);
             uLCD.locate(1, 1);
             uLCD.color(BLUE);
             uLCD.printf("TopButton: Action\n LeftButton: Menu\n Bottom: Omni\n\n     Find the\n   dragon radar!\n   Tilt to play!\n");
             wait(7);
             uLCD.cls();
             uLCD.rectangle(0, 0, 127, 127, BLUE);
             uLCD.filled_rectangle(1, 1, 126, 126, BLACK);
             uLCD.locate(1, 1);
             uLCD.color(RED);
             uLCD.printf("Accelerometer:\n\n\n\n\n X: RIGHT/LEFT\n Y: UP/DOWN");
             wait(4);
             uLCD.filled_rectangle(0, 0, 127, 127, BLACK);
             break;
             
        default:
        
             break;
    }
    return NO_RESULT;
}


/**
 * Entry point for frame drawing. This should be called once per iteration of
 * the game loop. This draws all tiles on the screen, followed by the status 
 * bars. Unless init is nonzero, this function will optimize drawing by only 
 * drawing tiles that have changed from the previous frame.
 */
void draw_game(int init)
{
    // Draw game border first
    if(init) draw_border();
    
    
    if (talkedToBlue)
    {
        add_ladder(14,11);
    }
    if (gotFake)
    {
        map_erase(seven1X, seven1Y);
        add_plant(seven1X, seven1Y);   
    }
    if (!gotFake && Player.has_key && currentMap == 0)
    {
        add_Seven1(seven1X, seven1Y);   
        add_Seven1(31, 17);
    }
    if(gotFake2)
    {
        map_erase(31, 17);
        add_plant(31, 17);    
    }
    // Iterate over all visible map tiles
    for (int i = -5; i <= 5; i++) // Iterate over columns of tiles
    {
        for (int j = -4; j <= 4; j++) // Iterate over one column of tiles
        {
            // Here, we have a given (i,j)
            
            // Compute the current map (x,y) of this tile
            int x = i + Player.x;
            int y = j + Player.y;
            
            // Compute the previous map (px, py) of this tile
            int px = i + Player.px;
            int py = j + Player.py;
                        
            // Compute u,v coordinates for drawing
            int u = (i+5)*11 + 3;
            int v = (j+4)*11 + 15;
            
            // Figure out what to draw
            DrawFunc draw = NULL;
            if (init && i == 0 && j == 0) // Only draw the player on init
            {
                if (OMode)
                {
                    draw_GodMode(u, v, Player.has_key);
                    continue;
                }
                else{
                    draw_player(u, v, Player.has_key);
                    continue;
                }
            }
            else if (x >= 0 && y >= 0 && x < map_width() && y < map_height()) // Current (i,j) in the map
            {
                MapItem* curr_item = get_here(x, y);
                MapItem* prev_item = get_here(px, py);
                if (init || curr_item != prev_item) // Only draw if they're different
                {
                    if (curr_item) // There's something here! Draw it
                    {
                        draw = curr_item->draw;
                    }
                    else // There used to be something, but now there isn't
                    {
                        draw = draw_nothing;
                    }
                }
            }
            else if (init) // If doing a full draw, but we're out of bounds, draw the walls.
            {
                draw = draw_wall;
            }

            // Actually draw the tile
            if (draw) draw(u, v);
        }
    }

    // Draw status bars    
    draw_upper_status();
    draw_lower_status();
}


/**
 * Initialize the main world map. Add walls around the edges, interior chambers,
 * and plants in the background so you can see motion. Note: using the similar
 * procedure you can init the secondary map(s).
 */
void init_main_map()
{
    // "Random" plants
    Map* map = set_active_map(0);
    
    
    for(int i = map_width() + 3; i < map_area(); i += 39)
    {
        add_plant(i % map_width(), i / map_width());
    }
    pc.printf("plants\r\n");
    
    add_NPC1(10, 9);
    add_One(8, 10);
    add_Two(8, 20);
    add_Three(20, 20);
    add_Four(30, 20);
    add_Five(40, 20);
    add_Six(40, 35);
    add_Frieza(0, 21);
    add_lava(3, 8, HORIZONTAL, 2);
    add_lava(13, 8, HORIZONTAL, 3);
    add_Sword(49, 49);
    add_Treasure(14, 3);
    add_Piccolo(picX, picY);
    print_map();
}


/**
 * Generates layout of second map
 */
void init_maze_map()
{
    // "Random" plants
    Map* map = set_active_map(1);
    
    pc.printf("Adding walls!\r\n");
    add_wall(0,              0,              HORIZONTAL, 30);
    add_wall(0,              29,             HORIZONTAL, 30);
    add_wall(0,              0,              VERTICAL,   30);
    add_wall(29,             0,              VERTICAL,   30);
    add_wall(10, 14, HORIZONTAL, 20);
    add_wall(0, 16, HORIZONTAL, 29);
    
    speech("Find radar and go", "up other stairs");
    pc.printf("Walls done!\r\n");
    
    add_ladder(28, 15);
    
    add_radar(25, 15);
    print_map();
}


/**
 * Program entry point! This is where it all begins.
 * This function orchestrates all the parts of the game. Most of your
 * implementation should be elsewhere - this holds the game loop, and should
 * read like a road map for the rest of the code.
 */
int main()
{
    uLCD.printf("\n\n    DRAGONBALL:\n");
    uLCD.printf("\n\n   PLANET NAMEK");
    uLCD.printf("\n\n\n\n     ECE 2035\n");
    speaker.period(1.0/739.99);
    speaker = 0.05;
    wait(0.25);
    speaker.period(1.0/739.99);
    speaker = 0.05;
    wait(0.25);
    speaker.period(1.0/739.99);
    speaker = 0.05;
    wait(0.25);
    speaker.period(1.0/739.99);
    speaker = 0.05;
    wait(0.5);
    speaker.period(1.0/493.88);
    speaker = 0.05;
    wait(0.5);
    speaker.period(1.0/587.33);
    speaker = 0.05;
    wait(0.25);
    speaker.period(1.0/659.25);
    speaker = 0.05;
    wait(0.5);
    speaker.period(1.0/739.99);
    speaker = 0.05;
    wait(1);
    speaker = 0;
    wait(1);
    
    uLCD.cls();

    // First things first: initialize hardware
    ASSERT_P(hardware_init() == ERROR_NONE, "Hardware init failed!");

    // Initialize the maps
    maps_init();
    init_main_map();
    
    // Initialize game state
    set_active_map(0);
    Player.x = Player.y = 5;

    // Initial drawing
    draw_game(true);

    // Main game loop
    while(1)
    {
        // Timer to measure game update speed
        Timer t; 
        t.start();
        
        uLCD.locate(0,0);
        uLCD.color(BLUE);
        uLCD.set_font_size(0.2, 0.2);
        float x = Player.x;
        float y = Player.y;
        uLCD.printf("X: %2.0f, Y: %2.0f", x, y);
        
        GameInputs in = read_inputs();
        int action = get_action(in);
        int update = update_game(action);
        draw_game(update);
        
        if (Player.x > 10 && Player.x < 18 && Player.y > 0 && Player.y < 6 && !gotTreasure)
        {
            add_Sparkle(13, 2);
            add_Sparkle(15, 2);
            add_Sparkle(13, 4);
            add_Sparkle(15, 4);
            draw_game(FULL_DRAW);
            wait(0.15);
            draw_game(FULL_DRAW);
            map_erase(13, 2);
            map_erase(15, 2);
            map_erase(13, 4);
            map_erase(15, 4);
            draw_game(FULL_DRAW);
            wait(0.15);
        }
        Timer t1; 
        t1.start();
        while (get_here(Player.x, Player.y)->type == lava && !OMode)
        {  
            if (t1.read() > 1)
            {
                health -= 20;   
                uLCD.locate(1, 14);
                uLCD.printf("%s%d%s", "Health: ", health, "%");
                wait(2);
                break;
            }
        }
        t1.stop();
        if (health == 0)
        {
            uLCD.cls();
            uLCD.color(RED);
            uLCD.set_font_size(2, 2);
            uLCD.printf("\n\n\n\n       GAME\n\n\n       OVER\n");  
            break;
        }
        if (ballCount == 7)
        {
            uLCD.cls();
            uLCD.color(GREEN);
            uLCD.set_font_size(2, 2);
            uLCD.printf("\n\n\n\n   SHENRON HAS\n\n     APPEARED\n\n\n  MAKE YOUR WISH");
            wait(3);
            uLCD.cls();
            uLCD.color(RED);
            uLCD.set_font_size(2, 2);
            uLCD.printf("\n\n\n\n       GAME\n\n\n       OVER\n");  
                while(1)
                {
                    wait(1);
                }   
        }
        // Actuall do the game update:
        // 1. Read inputs        
        // 2. Determine action (get_action)        
        // 3. Update game (update_game)
        // 3b. Check for game over
        // 4. Draw frame (draw_game)
        
        // 5. Frame delay
        t.stop();
        int dt = t.read_ms();
        if (dt < 100) wait_ms(100 - dt);
    }
}
