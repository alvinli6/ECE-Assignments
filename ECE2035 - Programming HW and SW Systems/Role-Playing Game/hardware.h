#ifndef HARDWARE_H
#define HARDWARE_H

/**
 * Structure that represents all the inputs to the game.
 * If additional hardware is added, new elements should be added to this struct.
 */
struct GameInputs {
    int b1, b2, b3;     // Button presses
    double ax, ay, az;  // Accelerometer readings
};

/**
 * Initialize all the hardware.
 */
int hardware_init();

/**
 * Read all the user inputs. 
 * This is all input hardware interaction should happen.
 * Returns a GameInputs struct that has all the inputs recorded.
 * This GameInputs is used elsewhere to compute the game update.
 */
GameInputs read_inputs();

#endif // HARDWARE_H