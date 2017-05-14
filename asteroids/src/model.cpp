/* 
    Jacob Roling
	w14038815
*/

/* C libraries */
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include "model.h"
#include "utils.h"
#include "asteroids.h"

/* Local Variables */
static const int rockHeapSize = 8;
static const int missileHeapSize = 12;
static const int ufoHeapSize = 2;
static float missileCoolDown = 0;
static rock_t rockHeap[rockHeapSize];
static rock_t *rockFreeNodes;
static ufo_t ufoHeap[ufoHeapSize];
static ufo_t *ufoFreeNodes;
static missile_t missileHeap[missileHeapSize];
static missile_t *missileFreeNodes;
struct rock *rockCurrent = NULL;
struct ufo *ufoCurrent = NULL;
struct missile *missileCurrent = NULL;
static void rockFreeNode(rock_t *i);
static void initrockHeap(void);
static void killrocks(struct rock *a);
static void rockPhysics();
static void newrock(void);
static void rockUpdate(struct rock *a);
static void rockScreenWrap(struct rock *a);
static void rockPlayerCollision(struct rock *a);
static void missileFreeNode(missile_t *i);
static void initMissileHeap(void);
static void killMissiles(struct missile *m);
static void missilePhysics();
static void newMissile(void);
static void missileUpdate(struct missile *m);
static void missileOffScreen(struct missile *m);
static void ufoFreeNode(ufo_t *i);
static void initUfoHeap(void);
static void killUfos(struct ufo *u);
static void ufoPhysics();
static void newUfo(void);
static void ufoUpdate(struct ufo *u);
static void ufoScreenWrap(struct ufo *u);
static void ufoPlayerCollision(struct ufo *u);
static void missilerockCollision(struct missile *m, struct rock *a);
static void missileUfoCollision(struct missile *m, struct ufo *u);

static void resetShip();
static void shipMovement();
static void fullRotation();
static float directionx(int heading);
static float directiony(int heading);
static void screenWrap();
static float getTotalSpeed();
static void drag();
static void scoring();
void physics(void)
{
	//Initialise heaps on start screen
	if (paused && lifesleft == 5) {
		initrockHeap();
		initMissileHeap();
		initUfoHeap();
	}
	//Reset the players position and remove list objects when game is paused
	if (paused) {
		resetShip();
		killrocks(rockCurrent);
		killMissiles(missileCurrent);
		killUfos(ufoCurrent);
	}
	//While the game is in play
	else {
		shipMovement();
		rockPhysics();
		ufoPhysics();
		missilePhysics();
		missilerockCollision(missileCurrent, rockCurrent);
		missileUfoCollision(missileCurrent, ufoCurrent);
		scoring();
	}
}

/* Initiate the rock heap */
void initrockHeap(void) {
	int i;
	for (i = 0; i < (rockHeapSize - 1); i++) {
		rockHeap[i].next = &rockHeap[i + 1];
	}
	rockHeap[i].next = NULL;
	rockFreeNodes = &rockHeap[0];
}

/* Initiate the missile heap */
void initMissileHeap(void) {
	int i;
	for (i = 0; i < (missileHeapSize - 1); i++) {
		missileHeap[i].next = &missileHeap[i + 1];
	}
	missileHeap[i].next = NULL;
	missileFreeNodes = &missileHeap[0];
}

void initUfoHeap(void) {
	int i;
	for (i = 0; i < (ufoHeapSize - 1); i++) {
		ufoHeap[i].next = &ufoHeap[i + 1];
	}
	ufoHeap[i].next = NULL;
	ufoFreeNodes = &ufoHeap[0];
}

/* Allocate a rock node */
rock_t *allocateNoderock(void) {
	rock_t *rockNode = NULL;
	if (rockFreeNodes) {
		rockNode = rockFreeNodes;
		rockFreeNodes = rockFreeNodes->next;
	}
	return rockNode;
}

/* Allocate a missile node */
missile_t *allocateNodeMissile(void) {
	missile_t *missileNode = NULL;
	if (missileFreeNodes) {
		missileNode = missileFreeNodes;
		missileFreeNodes = missileFreeNodes->next;
	}
	return missileNode;
}

/* Allocate a Ufo node */
ufo_t *allocateNodeUfo(void) {
	ufo_t *ufoNode = NULL;
	if (ufoFreeNodes) {
		ufoNode = ufoFreeNodes;
		ufoFreeNodes = ufoFreeNodes->next;
	}
	return ufoNode;
}

/* Free a rock node */
void rockFreeNode(rock_t *i){
	i->next = rockFreeNodes;
	rockFreeNodes = i;
}

/* Free a missile node */
void missileFreeNode(missile_t *i){
	i->next = missileFreeNodes;
	missileFreeNodes = i;
}

/* Free a UFO node */
void ufoFreeNode(ufo_t *i){
	i->next = ufoFreeNodes;
	ufoFreeNodes = i;
}

/* Reset the ships values */
void resetShip() {
	player.heading = 90;
	player.fire = false;
	player.position.x = 220;
	player.position.y = 120;
	player.velocity.x = 0;
	player.velocity.y = 0;
}

/* Calculate the ships velocity and update its position */
static void shipMovement() {
	fullRotation();
	screenWrap();
	float xperc = directionx(player.heading);
	float yperc = directiony(player.heading);
	//If the ship is below its terminal velocity
	if (getTotalSpeed() < 3)
	{
		//If the player is braking reduce velocity drastically
		if (player.engines < 0) {
			player.velocity.x -= (player.velocity.x / 10);
			player.velocity.y -= (player.velocity.y / 10);
		}
		//If the player is accelerating increase velocity in the direction of the heading
		else if (player.engines > 0) {
			player.velocity.x += player.engines * xperc * 0.0005;
			player.velocity.y -= player.engines * yperc * 0.0005;
		}
	}
	drag();
	//Update the players position using their velocity
	player.position.x += player.velocity.x;
	player.position.y += player.velocity.y;
}

/* If the player has turned 360 degrees reset the heading */
void fullRotation() {
	if (player.heading > 360) {
		player.heading = 1;
	}
	else if (player.heading < 1) {
		player.heading = 360;
	}
}

/* If the player leaves the screen place them on the other side of the display */
void screenWrap() {
	if (player.position.x > 485) {
		player.position.x = 5;
	}
	else if (player.position.x < -5) {
		player.position.x = 475;
	}
	else if (player.position.y > 270) {
		player.position.y = 5;
	}
	else if (player.position.y < -5) {
		player.position.y = 265;
	}
}

/* Calculate the players x velocity with the current heading */
float directionx(int heading) {
	float xperc;
	if (heading <= 90) {
		xperc = heading;
	}
	else if (heading <= 180) {
		xperc = 90 - (heading - 90);
	}
	else if (heading <= 270) {
		xperc = 90 - (heading - 90);
	}
	else if (heading > 270) {
		xperc = - (360 - heading);
	}
	return xperc;
}

/* Calculate the players y velocity with the current heading */
float directiony(int heading) {
	float yperc;
	if (heading <= 90) {
		yperc = 90 - heading;
	}
	else if (heading <= 180) {
		yperc = - (heading - 90);
	}
	else if (heading <= 270) {
		yperc = - (270 - heading);
	}
	else if (heading > 270) {
		yperc = heading - 270;
	}
	return yperc;
}

/* Get the players current total speed */
float getTotalSpeed() {
	float speedx;
	float speedy;
	if (player.velocity.x < 0) {
		speedx = -player.velocity.x;
	}
	else {
		speedx = player.velocity.x;
	}
	if (player.velocity.y < 0) {
		speedy = -player.velocity.y;
	}
	else {
		speedy = player.velocity.y;
	}
	return speedx + speedy;
}

/* Reduce the players velocity by 1% */
void drag() {
	player.velocity.x -= (player.velocity.x / 100);
	player.velocity.y -= (player.velocity.y / 100);
}

/* Calculate the rocks position, velocity and find any collisions with the player */
void rockPhysics() {
	newrock();
	rockUpdate(rockCurrent);
	rockScreenWrap(rockCurrent);
	rockPlayerCollision(rockCurrent);
}

/* Randomize a new rocks values */
void rockValues(struct rock *a) {
	a->position.x = randrange(40,461);
	a->position.y = randrange(40,241);
	a->size = randrange(5, 45);
	a->velocity.x = randrange(-10, 10);
	a->velocity.y = randrange(-10, 10);
			//If the rock will spawn on the player recalculate its values
			while (player.position.x > (a->position.x - (a->size) - 15) 
				&& player.position.x < (a->position.x + (a->size) + 15) 
				&& player.position.y > (a->position.y - (a->size) - 15)
				&& player.position.y < (a->position.y + (a->size) + 15)) {
					a->position.x = randrange(20,461);
					a->position.y = randrange(20,241);
				}
	a->live = true;
}

/* Create a new rock in the list */
void newrock() {
	struct rock *newrock = allocateNoderock();
	if (newrock) {
		newrock->next = rockCurrent;
		rockCurrent = newrock;
		rockValues(newrock);
	}
}

/* Destroy all rocks in the list */
void killrocks(struct rock *a) {
	for (; a; a = a->next) {
			a->live = false;
	}
}

/* Calculate the rocks position and velocity */
void missilePhysics() {
	// If the player has pressed fire and half a second has passed 
	// since the last missile was fired then fire a missile
	if (player.fire && missileCoolDown < 0) {
		newMissile();
		missileCoolDown = 0.5;
	}
	player.fire = false;
	missileCoolDown -= Dt;
	
	missileUpdate(missileCurrent);
	missileOffScreen(missileCurrent);
}

/* Set a missiles velocity and position based on the ships current heading */
void missileValues(struct missile *m) {
	m->velocity.x = directionx(player.heading) * 0.01;
	m->velocity.y = -directiony(player.heading) * 0.01;
	m->position.x = player.position.x + m->velocity.x * 10;
	m->position.y = player.position.y + m->velocity.y * 10;
	m->live = true;;
}

/* Create a new missile in the list */
void newMissile() {
	struct missile *newMissile = allocateNodeMissile();
	if (newMissile) {
		newMissile->next = missileCurrent;
		missileCurrent = newMissile;
		missileValues(newMissile);
	}
}

/* Destroy all missiles in the list*/
void killMissiles(struct missile *m) {
	for (; m; m = m->next) {
			m->live = false;
	}
}

void ufoPhysics() {
	newUfo();
	ufoUpdate(ufoCurrent);
	ufoScreenWrap(ufoCurrent);
	ufoPlayerCollision(ufoCurrent);
}

void ufoValues(struct ufo *u) {
	u->position.x = randrange(40,461);
	u->position.y = randrange(40,241);
	u->size = randrange(29, 30);
	u->velocity.x = randrange(-10, 30);
	u->velocity.y = randrange(-10, 30);
			//If the ufo will spawn on the player recalculate its values
			while (player.position.x > (u->position.x - (u->size) - 15) 
				&& player.position.x < (u->position.x + (u->size) + 15) 
				&& player.position.y > (u->position.y - (u->size) - 15)
				&& player.position.y < (u->position.y + (u->size) + 15)) {
					u->position.x = randrange(20,461);
					u->position.y = randrange(20,241);
				}
	u->live = true;
}


void newUfo() {
	struct ufo *newUfo = allocateNodeUfo();
	if (newUfo) {
		newUfo->next = ufoCurrent;
		ufoCurrent = newUfo;
		ufoValues(newUfo);
	}
}

/* Destroy all missiles in the list*/
void killUfos(struct ufo *u) {
	for (; u; u = u->next) {
			u->live = false;
	}
}

/* Update the rocks positions using their velocity 
		and remove an rock if it has been destroyed */
void rockUpdate(struct rock *a) {
	for (; a; a = a->next) {
		a->position.x += a->velocity.x/25;
		a->position.y += a->velocity.y/25;
		if (a->next->live == false) {
			struct rock *dead = a->next;
			a->next = a->next->next;
			rockFreeNode(dead);
		}
	}
}

/* Update the missiles positions using their velocity 
		and remove an missile if it has been destroyed */
void missileUpdate(struct missile *m) {
		for (; m; m = m->next) {
		m->position.x += m->velocity.x;
		m->position.y += m->velocity.y;
		if (m->next->live == false) {
			struct missile *dead = m->next;
			m->next = m->next->next;
			missileFreeNode(dead);
		}
	}
}


/* Update the rocks positions using their velocity 
		and remove an rock if it has been destroyed */
void ufoUpdate(struct ufo *u) {
	for (; u; u = u->next) {
		u->position.x += u->velocity.x/25;
		u->position.y += u->velocity.y/25;
		if (u->next->live == false) {
			struct ufo *dead = u->next;
			u->next = u->next->next;
			ufoFreeNode(dead);
		}
	}
}
/* If an rock has passed the boundaries of the 
		screen move it to the other side of the screen */
void rockScreenWrap(struct rock *a) {
	for (; a; a = a->next) {
		if (a->position.x > 480) {
			a->position.x = 0;
		}
		else if (a->position.x < 0) {
			a->position.x = 480;
		}
		else if (a->position.y > 270) {
			a->position.y = 0;
		}
		else if (a->position.y < 0) {
			a->position.y = 270;
		}
	}
}

void ufoScreenWrap(struct ufo *u) {
	for (; u; u = u->next) {
		if (u->position.x > 480) {
			u->position.x = 0;
		}
		else if (u->position.x < 0) {
			u->position.x = 480;
		}
		else if (u->position.y > 270) {
			u->position.y = 0;
		}
		else if (u->position.y < 0) {
			u->position.y = 270;
		}
	}
}
/* If a missile has passed the boundaries of the screen destroy it */
void missileOffScreen(struct missile *m) {
	for (; m; m = m->next) {
		if (m->position.x > 480 || m->position.x < 0 ||
						m->position.y > 270 || m->position.y < 0) {
			m->live = false;
		}
	}
}

/* If the player touches an rock destroy the rock and damage the ships shield */
void rockPlayerCollision(struct rock *a) {
	for (; a; a = a->next) {
		if (player.position.x > (a->position.x - (a->size) - 8) 
				&& player.position.x < (a->position.x + (a->size) + 8) 
				&& player.position.y > (a->position.y - (a->size) - 8)
				&& player.position.y < (a->position.y + (a->size) + 8)
				&& a->live == true)
		{
			a->live = false;
			shield --;
		}
	}
}

void ufoPlayerCollision(struct ufo *u) {
	for (; u; u = u->next) {
		if (player.position.x > (u->position.x - (u->size) - 8) 
				&& player.position.x < (u->position.x + (u->size) + 8) 
				&& player.position.y > (u->position.y - (u->size) - 8)
				&& player.position.y < (u->position.y + (u->size) + 8)
				&& u->live == true)
		{
			u->live = false;
			shield --;
		}
	}
}

/* If a missile touches an rock destroy them both*/
void missilerockCollision(struct missile *m, struct rock *a) {
	for (; m; m = m->next) {
		for (; a; a = a->next) {
			if (m->position.x > (a->position.x - (a->size)) 
					&& m->position.x < (a->position.x + (a->size)) 
					&& m->position.y > (a->position.y - (a->size))
					&& m->position.y < (a->position.y + (a->size))
					&& a->live == true && m->live == true)
			{
				a->live = false;
				m->live = false;
			}
		}
	}
}

void missileUfoCollision(struct missile *m, struct ufo *u) {
	for (; m; m = m->next) {
		for (; u; u = u->next) {
			if (m->position.x > (u->position.x - (u->size)) 
					&& m->position.x < (u->position.x + (u->size)) 
					&& m->position.y > (u->position.y - (u->size))
					&& m->position.y < (u->position.y + (u->size))
					&& u->live == true && m->live == true)
			{
				u->live = false;
				m->live = false;
			}
		}
	}
}

/* For every second the player survives increment the currentscore */
void scoring() {
	e_time += Dt;
	if (e_time > currentscore) {
		currentscore ++;
	}
}
