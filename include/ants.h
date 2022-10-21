/*
** EPITECH PROJECT, 2022
** ants
** File description:
** ants.h
*/

#include "SFML/Graphics.h"
#include <SFML/Config.h>
#include <SFML/Audio.h>
#include <SFML/System.h>
#include <SFML/Window.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <math.h>

#define True 1
#define False 0

#define PI 3.14159265359

#define bool int

typedef struct ant_s {
    sfRectangleShape *part;
    int index;
    int status;
    float last_home;
    float last_food;
    float clock;
    float dir_angle;
    float dir_chg_clock;
    float phe_clock;
    bool following;
    sfVector2f dir;
    sfVector2f vect;
    sfVector2f pos;
    struct ant_s *next;
    struct ant_s *prev;
} ant_t;

typedef struct pheromones_s {
    int type;
    int start;
    sfVector2f pos;
    sfColor color;
    sfVertex vertex;
    float intensity;
    float dist;
    float fade_clock;
    float alpha;
    struct pheromones_s *next;
    struct pheromones_s *prev;
} pheromones_t;

typedef struct phe_s {
    int type;
    int start;
    sfVector2f pos;
    sfColor color;
    sfVertex vertex;
    float intensity;
    float dist;
    float fade_clock;
    float alpha;
} phe_t;

typedef struct colony_s {
    sfVector2f pos;
    sfVector2f em_dir;
    sfCircleShape *circ;
    ant_t *ants;
    pheromones_t *phe;
    sfVector2f *spwn_coords;
    int nb_phe;
    int nb_ants;
    float speed;
    float clock;
} colony_t;

typedef struct render_s {
    sfRenderWindow *w;
    sfView *v;
    sfRectangleShape *background;
    sfText *fps_hint;
    float fps_clock;
    bool quadtree_visible;
} render_t;

typedef struct clock_s {
    sfClock *clock;
    sfTime time;
    sfTime prev_time;
    float seconds;
    float pathfinding_clock;
    float foodpickup_clock;
    float quadtree_clock;
} clock_st;

typedef struct textures_s {
    sfTexture *ant;
} textures_t;

typedef struct mouse_s {
    sfVector2f pos;
    sfVector2f last_pos;
    sfVector2f diff;
} mouse_t;

typedef struct food_s {
    float value;
    sfCircleShape *shape;
    sfVector2f pos;
    struct food_s *next;
    struct food_s *prev;
} food_t;

typedef struct map_s {
    food_t *food;
    float place_clock;
    sfFloatRect bounds;
} map_t;

typedef struct quadtree_s {
    sfFloatRect bounds;
    bool divided;
    int capacity;
    int fullness;
    phe_t *phe;
    sfRectangleShape *quad;
    struct quadtree_s *nw;
    struct quadtree_s *ne;
    struct quadtree_s *sw;
    struct quadtree_s *se;
} quadtree_t;

typedef struct core_s {
    sfThread *pathfinding_thread;
    sfThread *foodpickup_thread;
    sfThread *quadtree_thread;
    render_t ren;
    clock_st clock;
    sfEvent e;
    colony_t cly;
    map_t map;
    textures_t textures;
    mouse_t mouse;
    quadtree_t *quadtree;
} core_t;

// Utils
sfRenderWindow *create_window(char const *title);
sfSprite *new_sprite(char *texture_name, float x, float y);
sfSound *set_up_new_sound(sfSound *new_sound, const char *buffer_location);
void init_animated_sprite(sfSprite *sprite, int columns, int rows);
sfSprite *setSprite_pos(sfSprite *sprite, float posX, float posY);
sfVector2f get_sprite_center(sfSprite *sprite);
float random_float(float scale);
int random_number(int min_num, int max_num);
sfVertex add_vertex_circular(core_t *c, float *radius,
float i, sfVector2f center);
int positive(int nb);
sfVector2f get_rect_center(sfRectangleShape *rect);
sfFloatRect init_frect(float left, float top, float width, float height);
int rand_range(int lower, int upper);
sfVector2f init_vector2f(float x, float y);
sfIntRect init_irect(int left, int top, int width, int height);
char *my_int_to_str(int nb);
double get_angle(sfVector2f origin, sfVector2f direction);
double rad_to_deg(double angle);
double deg_to_rad(double angle);
bool rand_bool(void);
sfBool circle_intersect(sfVector2f center, float radius, sfVector2f point);

// Ants
void add_ant(core_t *c, ant_t **head, colony_t emiter);
void del_ant(ant_t **head, ant_t *del_node);

// Fps
float get_fps(sfClock *clock, sfTime *prev_time);
float time_as_sec(sfTime time);

// Pheromones
void add_phe(core_t *c, pheromones_t **head, ant_t ant, float seconds);
void del_phe(pheromones_t **head, pheromones_t *del_node);
void add_phe2(core_t *c, pheromones_t **head, sfVector2f pos, float seconds);
phe_t *get_points_in_range(quadtree_t *quadtree, sfFloatRect range);

// Map
void add_food(core_t *c, food_t **head, sfVector2f pos);
void del_food(food_t **head, food_t *del_node);

// Quadtree
quadtree_t *new_quadtree(sfFloatRect bounds, int capacity, sfColor color);
void generate_quadtree(core_t *c, quadtree_t *quadtree, pheromones_t **head);
void draw_quadtree(core_t *c, quadtree_t *quadtree);