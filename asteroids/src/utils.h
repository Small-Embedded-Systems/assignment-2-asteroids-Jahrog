/* 
    Jacob Roling
	w14038815
*/

/* some numerical helper functions */
float norm(float value, float min, float max);
float lerp(float min, float max, float value);
float map(float value, float lower, float upper, float min, float max);

/* make a 16bit colour */
typedef uint16_t colour_t;
/* macro to make colour value from r g b values in the range 0...255 
    see https://en.wikipedia.org/wiki/List_of_colors_(compact)
        https://en.wikipedia.org/wiki/List_of_Crayola_colored_pencil_colors
        https://en.wikipedia.org/wiki/List_of_Crayola_crayon_colors
        For lists of R G B values you might want to use
    I'm using a macro here as the compiler might well convert it into a constant at compile time
*/
#define rgb(r,g,b) ((uint16_t)(31 * r /255)<<11 | (uint16_t)(63 * g /255)<<5 | (uint16_t)(31 * b /255))

int randrange(int from, int to);

float dist(coordinate_t p1, coordinate_t p2);

extern const float pi;
float radians(float degrees);
float degrees(float radians);
