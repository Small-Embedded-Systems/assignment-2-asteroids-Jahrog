/* 
    Jacob Roling
	w14038815
*/


/* rocks Model */
struct point {
    float x,y;
};
typedef struct point coordinate_t;
typedef struct point vector_t;

/* Some initial struct types if you want to usd them */
struct ship {
		int heading; 
		int engines;
		bool fire;
    coordinate_t position;
    vector_t     velocity;
};

/* rock structure for use in the rock linked list */
typedef struct rock {
    coordinate_t position;
		vector_t 	velocity;
		int size;
		bool live;
    struct rock *next;
} rock_t;

/* Missile structure for use in the missile linked list */
typedef struct missile {
    coordinate_t position;
		vector_t     velocity;
		bool live;
    struct missile *next;
} missile_t;

/* rock structure for use in the rock linked list */
typedef struct ufo {
    coordinate_t position;
		vector_t 	velocity;
		int size;
		bool live;
    struct ufo *next;
} ufo_t;


/* Main physics method */
void physics(void);

/* rock and missile list pointers */
rock_t *allocateNoderock(void);
missile_t *allocateNodeMissile(void);
ufo_t *allocateNodeUfo(void);

/* Externalised variables */
extern struct	rock *rockCurrent;
extern const int rockHeapSize;
extern struct missile *missileCurrent;
extern const int missileHeapSize;
extern struct	ufo *ufoCurrent;
extern const int ufoHeapSize;
