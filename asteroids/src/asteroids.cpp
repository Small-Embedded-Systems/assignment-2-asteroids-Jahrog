/* 
    Jacob Roling
	w14038815
*/

/* C libraries */
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>
#include <display.h>
#include <mbed.h>
#include "model.h"
#include "view.h"
#include "controller.h"

/* Global Variables */
//elapsed time in the game
float e_time; 
//current currentscore which the user has
int   currentscore = 0; 
//current amount of lifesleft the user has
int   lifesleft = 5; 
//current level of shield the user has
int shield = 3; 
//
struct ship player; 
//if state of game is active or not
bool paused = true; 

/* Speed of game */
float Dt = 0.01f;

/* Ticker objects to manage physics, graphics and input */
Ticker model, view, controller;

/* Local Method Declarations*/
static void end();

/* jstick centre used to start game */
DigitalIn startbutton(P5_3,PullUp);

/* Main */
int main()
{
    init_DBuffer();
    
    view.attach( draw, 0.025);
    model.attach( physics, Dt);
    controller.attach( controls, 0.025);
    
    /* game start */
    while( startbutton.read() ){
        paused=true;
    }
		paused = false;
		
		/* Main Loop */
    while(true) {
			//If shields are less then 1 call end()
			if(shield < 1) {
				end(); //end method
			}
			//If the player runs out of lifesleft reset the game when play is resumed
			if(lifesleft < 0) {
				lifesleft = 5; //lifesleft back to default
				currentscore = 0; //currentscore reset to zero
				e_time = 0; //elapsed time reset
			}
    }
}

/* Called when the player crashes the ship. 
	 Pauses the game, resets shields and takes away a life.  */
static void end() {
	lifesleft --;
	while( startbutton.read() ){
        paused=true;
    }
	shield = 3;
	paused = false;
}
