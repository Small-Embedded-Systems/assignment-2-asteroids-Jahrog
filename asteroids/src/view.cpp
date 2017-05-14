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
#include "asteroids.h"
#include "model.h"
#include "utils.h"

Display *graphics = Display::theDisplay();

const colour_t background = rgb(0,0,0);

/* Local Method Declarations*/
static void drawStart();

static void drawGameOver();
static void drawBanner();

static void drawShip();
static void drawShield();
static void drawrocks(struct rock *a);
static void drawMissiles(struct missile *m);
static void drawUfos(struct ufo *u);
static coordinate_t getPoint(coordinate_t origin, int basex, int basey, float heading);

/* double buffering functions */
void init_DBuffer(void)
{   /* initialise the LCD driver to use second frame in buffer */
  uint16_t *bufferbase = graphics->getFb();
  uint16_t *nextbuffer = bufferbase+ (480*272);
  LPC_LCD->UPBASE = (uint32_t)nextbuffer;
}

void swap_DBuffer(void)
{   /* swaps frames used by the LCD driver and the graphics object */
  uint16_t *buffer = graphics->getFb();
  graphics->setFb( (uint16_t*) LPC_LCD->UPBASE);
  LPC_LCD->UPBASE = (uint32_t)buffer;
}

/* Main Draw Method */
void draw(void) {
  //Draw background
	graphics->fillScreen(background);
	//If the game has not started draw the start screen
	if (paused && lifesleft == 5) {
		drawStart();
	}
	//If the player has died and has remaining lifesleft draw the respawn screen
	else if (paused && lifesleft < 5 && lifesleft >= 0) {
		drawStart();
	}	
	//If the player is out of lifesleft draw the game over screen
	else if (paused && lifesleft < 0) {
		drawGameOver();
	}	
	//If the game is in play draw the game elements
	else {
		drawrocks(rockCurrent);
		drawMissiles(missileCurrent);
		drawUfos(ufoCurrent);
		drawBanner();
		drawShip();
	}
	//Display the elements drawn on the LCD
  swap_DBuffer();
}

/* Draws title screen */
static void drawStart() {
	graphics->setTextSize(2);
	graphics->setCursor(33, 100);
	graphics->setTextColor(GREEN);
	graphics->setTextSize(4);
	graphics->printf("Asteroids");
	graphics->setTextSize(2);
	graphics->setCursor(33, 140);
	graphics->printf("joystick center to start");
	graphics->setTextSize(1);
}


/* Draws game over screen*/
static void drawGameOver() {
	graphics->setCursor(5, 5);
	graphics->setTextColor(GREEN);
	graphics->setTextSize(3);
	graphics->printf("OUT OF LIFES");
	graphics->setCursor(5, 50);
	graphics->setTextSize(2);
	graphics->printf("joystick centre to restart");
	graphics->setTextSize(2);
	graphics->setCursor(5,75);
	graphics->printf("currentscore: %d", currentscore);
	graphics->setCursor(5,150);
	graphics->printf("JACOB ROLING");
	graphics->setCursor(5,175);
	graphics->printf("W14038815");
}

/* Draw banner information containing the players lifesleft currentscore and shield status */
static void drawBanner() {
	graphics->setCursor(5,4);
	graphics->setTextSize(1);
	graphics->setTextColor(GREEN);
	graphics->printf("Current score: %d", currentscore);
	graphics->setCursor(5,12);
	graphics->printf("lifes Left: %d", lifesleft);
}


/* Draw the players ship */
static void drawShip() {
	float heading = radians(player.heading);
	coordinate_t front, backR, backL;
	//Calculate the three points of the triangle based on the players heading
	front = getPoint(player.position, 0, -15, heading);
	backL = getPoint(player.position, 12, 12, heading);
	backR = getPoint(player.position, -12, 12, heading);
	graphics->fillTriangle(front.x, front.y, backL.x, backL.y, backR.x, backR.y, GREEN);
	//Draw the ships shields
	drawShield();
}

/* Draw shields around the ship and in the information banner */
static void drawShield() {
	//As the shield is damaged it turns red and the shield bar in the info banner reduces
	if (shield == 3) {
		graphics->drawCircle(player.position.x, player.position.y, 19, BLUE);
		graphics->drawCircle(player.position.x, player.position.y, 21, BLUE);
		graphics->drawCircle(player.position.x, player.position.y, 23, BLUE);
	}
	else if (shield == 2) {
		graphics->drawCircle(player.position.x, player.position.y, 19, BLUE);
		graphics->drawCircle(player.position.x, player.position.y, 21, BLUE);
	}
	else {
		graphics->drawCircle(player.position.x, player.position.y, 19, BLUE);
		
	}
}

/* Calculate a point based on its origin and displacement*/
static coordinate_t getPoint(coordinate_t origin, int basex, int basey, float heading) {
	coordinate_t result;
	result.x = (basex * cos(heading)) - (basey * sin(heading));
	result.y = (basex * sin(heading)) + (basey * cos(heading));
	result.x += origin.x;
	
	result.y += origin.y;
	
	return result;
}

/* Iterate through the list of rocks and draw each one if it is still alive */
void drawrocks(struct rock *a) {
	while (a) {
		if (a->live) {
			graphics->fillCircle(a->position.x, a->position.y, a->size, LIGHTYELLOW);
		}
		a = a->next;
	}
}

/* Iterate through the list of missiles and draw each one if it is still alive */
void drawMissiles(struct missile *m) {
	while (m) {
		if(m->live) {
			graphics->fillCircle(m->position.x, m->position.y, 1, WHITE);
		}
		m = m->next;
	}
}

void drawUfos(struct ufo *u) {
	while (u) {
		if (u->live) {
			graphics->fillCircle(u->position.x, u->position.y, u->size, GREEN);
			
			graphics->fillCircle(u->position.x, u->position.y, u->size, RED);
		}
		u = u->next;
	}
}
