/*
** EPITECH PROJECT, 2021
** B-MUL-200-STG-2-1-myrpg-basile.trebus-hamann
** File description:
** main.c
*/

#include "ants.h"

void toggle_quadtree_view(core_t *c)
{
    static int key_pressed = False;

    if (sfKeyboard_isKeyPressed(sfKeyT) && key_pressed == False) {
        if (c->ren.quadtree_visible == True)
            c->ren.quadtree_visible = False;
        else
            c->ren.quadtree_visible = True;
        key_pressed = True;
    }
    if (!sfKeyboard_isKeyPressed(sfKeyT))
        key_pressed = False;
}

void analyse_events(core_t *c)
{
    while (sfRenderWindow_pollEvent(c->ren.w, &c->e)) {
        if (c->e.type == sfEvtClosed)
            sfRenderWindow_close(c->ren.w);
        if (sfKeyboard_isKeyPressed(sfKeyEscape))
            sfRenderWindow_close(c->ren.w);
        toggle_quadtree_view(c);
    }
}

void update_clock(core_t *c)
{
    c->clock.time = sfClock_getElapsedTime(c->clock.clock);
    c->clock.seconds = c->clock.time.microseconds/1000000.0;
}

void update_fps(core_t *c)
{
    float fps = get_fps(c->clock.clock, &c->clock.prev_time);

    if (c->clock.seconds > c->ren.fps_clock + 0.3) {
        c->ren.fps_clock = c->clock.seconds;
        sfText_setString(c->ren.fps_hint, my_int_to_str(fps));
    }
}

void move_ant(core_t *c, ant_t *ant)
{
    sfVector2f next_pos = {ant->pos.x + ant->vect.x, ant->pos.y + ant->vect.y};

    if (next_pos.x > c->map.bounds.width + c->map.bounds.left
        || next_pos.x < c->map.bounds.left) {
        ant->vect.x *= -1;
        ant->dir_angle = get_angle(ant->pos,
        (sfVector2f){ant->pos.x + ant->vect.x,
        ant->pos.y + ant->vect.y});
        sfRectangleShape_setRotation(ant->part, ant->dir_angle);
    }
    if (next_pos.y > c->map.bounds.height + c->map.bounds.top
        || next_pos.y < c->map.bounds.top) {
        ant->vect.y *= -1;
        ant->dir_angle = get_angle(ant->pos,
        (sfVector2f){ant->pos.x + ant->vect.x,
        ant->pos.y + ant->vect.y});
        sfRectangleShape_setRotation(ant->part, ant->dir_angle);
    }
    sfRectangleShape_move(ant->part, ant->vect);
}

void draw_ant(core_t *c, ant_t *ant)
{
    sfRenderWindow_drawRectangleShape(c->ren.w, ant->part, NULL);
}

float dist_from(sfVector2f p1, sfVector2f p2)
{
    float distance = sqrt(pow(p2.x - p1.x, 2) + pow(p2.y - p1.y, 2));

    return (distance);
}

void find_pheromone(core_t *c, ant_t *ant)
{
    sfFloatRect r_in = sfRectangleShape_getGlobalBounds(ant->part);
    sfFloatRect r_out = r_in;
    sfFloatRect r_cly = sfCircleShape_getGlobalBounds(c->cly.circ);
    phe_t *phe;
    float angle;
    float f_angle;
    float intensity = 9999;
    float dist = 10;
    float diff;
    float turn_value = 0;
    int found = 0;

    if (sfFloatRect_intersects(&r_in, &r_cly, NULL))
        return;
    r_cly.left -= 100;
    r_cly.top -= 100;
    r_cly.height += 200;
    r_cly.width += 200;

    r_out.left -= 20;
    r_out.top -= 20;
    r_out.height += 40;
    r_out.width += 40;
    ant->pos = sfRectangleShape_getPosition(ant->part);
    phe = get_points_in_range(c->quadtree, (r_out));
    if (phe != NULL) {
        for (int i = 0; phe[i].type != -1; i++) {
            if (sfFloatRect_contains(&r_out, phe[i].pos.x, phe[i].pos.y)) {
                if (phe[i].type == 2 && ant->status == 1 && phe[i].intensity <= intensity) {
                    found = 1;
                    angle = get_angle(ant->pos, phe[i].pos);
                    f_angle = sfRectangleShape_getRotation(ant->part);
                    diff = angle - f_angle;
                    if (diff < 140 && diff > -140) {
                        intensity = phe[i].intensity;
                        turn_value = diff / 10;
                    }
                }
                if (ant->status == 2 && phe[i].type == 1 && phe[i].dist <= dist) {
                    found = 1;
                    angle = get_angle(ant->pos, phe[i].pos);
                    f_angle = sfRectangleShape_getRotation(ant->part);
                    diff = angle - f_angle;
                    if (diff < 140 && diff > -140) {
                        dist = phe[i].dist;
                        turn_value = diff / 10;
                    }
                }
            }
        }
    }
    free(phe);
    if ((dist_from(c->cly.pos, ant->pos) + sfCircleShape_getRadius(c->cly.circ)) < 200 && ant->status == 2) {
        angle = get_angle(ant->pos, c->cly.pos);
        found = 1;
        f_angle = sfRectangleShape_getRotation(ant->part);
        diff = angle - f_angle;
        turn_value = diff;
    }
    if (found == 0)
        ant->dir_angle += ((double)random_number(1, 2) / 10) * ant->dir.x;
    ant->dir_angle += turn_value;
}

void change_direction(core_t *c, ant_t *ant)
{
    double angle;
    sfVector2f vect = {cos(deg_to_rad(ant->dir_angle)),
    sin(deg_to_rad(ant->dir_angle))};

    ant->last_home += 0.01;
    ant->last_food += 0.01;
    vect.x *= c->cly.speed;
    vect.y *= c->cly.speed;
    ant->pos = sfRectangleShape_getPosition(ant->part);
    if (c->clock.seconds > ant->dir_chg_clock + 0.3) {
        ant->dir_chg_clock = c->clock.seconds;
        ant->dir.x *= rand_bool();
    }
    ant->vect = vect;
    ant->dir_angle = get_angle(ant->pos, (sfVector2f){ant->pos.x + vect.x,
    ant->pos.y + vect.y});
    sfRectangleShape_setRotation(ant->part, ant->dir_angle);
}

void put_pheromone(core_t *c, ant_t *ant)
{
    phe_t *found;
    sfFloatRect ant_rect = sfRectangleShape_getGlobalBounds(ant->part);

    if (c->clock.seconds > ant->phe_clock + 0.4) {
        ant->phe_clock = c->clock.seconds;
        found = get_points_in_range(c->quadtree, ant_rect);
        
        //if ()


        if ((ant->status == 1 && ant->last_home < 10)
            || (ant->status == 2 && ant->last_food < 10))
            add_phe(c, &c->cly.phe, *ant, c->clock.seconds);
    }
}

void pickup_food(core_t *c, ant_t *ant, food_t *node, food_t **head)
{
    sfFloatRect r1 = sfRectangleShape_getGlobalBounds(ant->part);
    sfFloatRect r2;
    float dist = 61;
    float ndist;
    int found = 0;
    float turn_value = 0;

    while (node != NULL) {
        r2 = sfCircleShape_getGlobalBounds(node->shape);
        ndist = dist_from(ant->pos, node->pos);
        if (ndist < dist && ant->status == 1) {
            dist = ndist;
            found = 1;
            float angle = get_angle(ant->pos, node->pos);
            float f_angle = sfRectangleShape_getRotation(ant->part);
            float diff = angle - f_angle;
            turn_value = diff / 20;
        }
        if (sfFloatRect_intersects(&r1, &r2, NULL) && ant->status == 1) {
            node->value -= 1;
            if (node->value <= 0)
                del_food(head, node);
            ant->status = 2;
            ant->last_food = 0;
            ant->vect.y *= -1;
            ant->vect.x *= -1;
            ant->dir_angle = get_angle(ant->pos,
            (sfVector2f){ant->pos.x + ant->vect.x,
            ant->pos.y + ant->vect.y});
            sfRectangleShape_setRotation(ant->part, ant->dir_angle);
        }
        node = node->next;
    }
    ant->dir_angle += turn_value;
    r2 = sfCircleShape_getGlobalBounds(c->cly.circ);
    if (sfFloatRect_intersects(&r1, &r2, NULL)) {
        ant->last_home = 0;
        ant->vect.y *= -1;
        ant->vect.x *= -1;
        ant->dir_angle = get_angle(ant->pos,
        (sfVector2f){ant->pos.x + ant->vect.x,
        ant->pos.y + ant->vect.y});
        sfRectangleShape_setRotation(ant->part, ant->dir_angle);
        ant->status = 1;
    }
}

void update_ants_path(core_t *c, ant_t **head, ant_t *node)
{
    while (node != NULL) {
        find_pheromone(c, node);
        move_ant(c, node);
        node = node->next;
    }
}

void update_ants_food(core_t *c)
{
    ant_t *node = c->cly.ants;

    while (c->clock.seconds < c->clock.foodpickup_clock + 0.016)
        0;
    c->clock.foodpickup_clock = c->clock.seconds;
    while (node != NULL) {
        pickup_food(c, node, c->map.food, &c->map.food);
        node = node->next;
    }
    update_ants_food(c);
}

void update_ants(core_t *c, ant_t **head, ant_t *node)
{
    sfFloatRect r1 = sfCircleShape_getGlobalBounds(c->cly.circ);

    sfRenderWindow_drawCircleShape(c->ren.w, c->cly.circ, NULL);
    while (node != NULL) {
        change_direction(c, node);
        find_pheromone(c, node);
        move_ant(c, node);
        put_pheromone(c, node);
        pickup_food(c, node, c->map.food, &c->map.food);
        draw_ant(c, node);
        node->pos = sfRectangleShape_getPosition(node->part);
        if (sfFloatRect_contains(&r1, node->pos.x,  node->pos.y))
            del_ant(head, node);
        node = node->next;
    }
}

void draw_phe(core_t *c, sfVertexArray *varray)
{
    sfRenderWindow_drawVertexArray(c->ren.w, varray, NULL);
}

void update_pheromones(core_t *c, pheromones_t **head, pheromones_t *node)
{
    sfVertexArray *varray = sfVertexArray_create();

    while (node != NULL) {
        if (node->start == 0) {
            node->fade_clock = c->clock.seconds;
            node->start = 1;
        }
        node->vertex.color.a = node->alpha;
        node->alpha -= (float)0.6;
        if (node->alpha <= 0)
            node->alpha = 0;
        sfVertexArray_append(varray, node->vertex);
        if (node->alpha == 0)
            del_phe(head, node);
        node = node->next;
    }
    sfVertexArray_setPrimitiveType(varray, sfPoints);
    draw_phe(c, varray);
}

void place_food(core_t *c)
{
    if (c->clock.seconds > c->map.place_clock + 0.2
        && sfMouse_isButtonPressed(sfMouseLeft)) {
        add_food(c, &c->map.food, c->mouse.pos);
    }
}

void draw_food(core_t *c, food_t *node)
{
    while (node != NULL) {
        sfRenderWindow_drawCircleShape(c->ren.w, node->shape, NULL);
        node = node->next;
    }
}

void update_food(core_t *c)
{
    place_food(c);
    draw_food(c, c->map.food);
}

//void compute_quadtree(core_t *c)
//{
//    while (c->clock.seconds < c->clock.quadtree_clock + 0.016)
//        0;
//    c->clock.quadtree_clock = c->clock.seconds;
//    generate_quadtree(c, c->quadtree, &c->cly.phe);
//}
//
//void compute_path(core_t *c)
//{
//    while (c->clock.seconds < c->clock.pathfinding_clock + 0.016)
//        0;
//    c->clock.pathfinding_clock = c->clock.seconds;
//    generate_quadtree(c, c->quadtree, &c->cly.phe);
//    update_ants_path(c, &c->cly.ants, c->cly.ants);
//    compute_path(c);
//}

void render_game(core_t *c)
{
    static float cooldown = 0;

    sfRenderWindow_display(c->ren.w);
    sfRenderWindow_clear(c->ren.w, sfBlack);
    sfRenderWindow_drawRectangleShape(c->ren.w, c->ren.background, NULL);
    update_clock(c);
    update_fps(c);
    //if (sfMouse_isButtonPressed(sfMouseLeft) && c->clock.seconds > cooldown + 0.3) {
    //    cooldown = c->clock.seconds;
    //    c->mouse.pos.x += ((double)random_number(-5, 5) / 10);
    //    c->mouse.pos.y += ((double)random_number(-5, 5) / 10);
    //    add_phe2(c, &c->cly.phe, c->mouse.pos, c->clock.seconds);
    //    c->mouse.pos.x += ((double)random_number(-5, 5) / 10);
    //    c->mouse.pos.y += ((double)random_number(-5, 5) / 10);
    //    add_phe2(c, &c->cly.phe, c->mouse.pos, c->clock.seconds);
    //    c->mouse.pos.x += ((double)random_number(-5, 5) / 10);
    //    c->mouse.pos.y += ((double)random_number(-5, 5) / 10);
    //    add_phe2(c, &c->cly.phe, c->mouse.pos, c->clock.seconds);
    //    c->mouse.pos.x += ((double)random_number(-5, 5) / 10);
    //    c->mouse.pos.y += ((double)random_number(-5, 5) / 10);
    //    add_phe2(c, &c->cly.phe, c->mouse.pos, c->clock.seconds);
    //}
    generate_quadtree(c, c->quadtree, &c->cly.phe);
    if (c->ren.quadtree_visible == True)
        draw_quadtree(c, c->quadtree);
    update_ants(c, &c->cly.ants, c->cly.ants);
    update_pheromones(c, &c->cly.phe, c->cly.phe);
    update_food(c);
    sfRenderWindow_drawText(c->ren.w, c->ren.fps_hint, NULL);
    analyse_events(c);
}

void init_fps_text(core_t *c)
{
    c->ren.fps_hint = sfText_create();
    sfText_setFont(c->ren.fps_hint, sfFont_createFromFile("misc/font.ttf"));
    sfText_setColor(c->ren.fps_hint, sfWhite);
    sfText_setPosition(c->ren.fps_hint, (sfVector2f){30, 30});
    sfText_setString(c->ren.fps_hint, NULL);
    c->ren.fps_clock = 0;
}

sfVector2f *divide_spawn(core_t *c, colony_t colony)
{
    int number_of_chunks = colony.nb_ants;
    double degrees = 0;
    sfVector2f *coords = malloc(sizeof(sfVector2f) * number_of_chunks);
    double radius = sfCircleShape_getRadius(colony.circ) * 2;

    for (int i = 0; i < number_of_chunks; i++) {
        degrees = i * ((double)360 / number_of_chunks);
        float radian = (degrees * (PI / 180));
        coords[i].x = colony.pos.x + radius * cos(radian);
        coords[i].y = colony.pos.y + radius * sin(radian);
    }
    return coords;
}

void init_colony(core_t *c)
{
    c->cly.circ = sfCircleShape_create();
    sfCircleShape_setFillColor(c->cly.circ, sfBlue);
    sfCircleShape_setRadius(c->cly.circ, 60);
    sfCircleShape_setOrigin(c->cly.circ, (sfVector2f){60, 60});
    sfCircleShape_setPosition(c->cly.circ, c->cly.pos);
    c->cly.nb_ants = 500;
    c->cly.speed = 1.3;
    c->cly.spwn_coords = divide_spawn(c, c->cly);
    for (int i = 0; i < c->cly.nb_ants; i++)
        add_ant(c, &c->cly.ants, c->cly);
}

void init_map(core_t *c)
{
    c->map.bounds.height = 1080;
    c->map.bounds.width = 1920;
    c->map.bounds.top = 0;
    c->map.bounds.left = 0;
    c->map.place_clock = 0;
    c->map.food = NULL;
}

void init_game(core_t *c)
{
    c->ren.w = create_window("Ants");
    c->ren.background = sfRectangleShape_create();
    sfRectangleShape_setSize(c->ren.background, (sfVector2f){1920, 1080});
    sfRectangleShape_setFillColor(c->ren.background, sfBlack);
    sfRenderWindow_setPosition(c->ren.w, (sfVector2i){0, 0});
    c->cly.ants = NULL;
    c->cly.phe = NULL;
    c->cly.em_dir = (sfVector2f){0, 0};
    c->cly.pos = (sfVector2f){960, 500};
    c->cly.clock = 0;
    c->clock.pathfinding_clock = 0;
    c->clock.foodpickup_clock = 0;
    c->clock.quadtree_clock = 0;
    c->clock.clock = sfClock_create();
    sfTime prev_time = sfClock_getElapsedTime(c->clock.clock);
    c->textures.ant = sfTexture_createFromFile("sprites/ant.png", NULL);
    c->mouse.last_pos.x = sfMouse_getPositionRenderWindow(c->ren.w).x;
    c->mouse.last_pos.y = sfMouse_getPositionRenderWindow(c->ren.w).y;
    c->mouse.diff = (sfVector2f){0, 0};
    //c->quadtree_thread = sfThread_create(compute_quadtree, c);
    //c->pathfinding_thread = sfThread_create(compute_path, c);
    c->foodpickup_thread = sfThread_create(update_ants_food, c);
    c->quadtree = NULL;
    c->ren.quadtree_visible = False;
    init_map(c);
    init_fps_text(c);
    init_colony(c);
}

int game_loop(void)
{
    core_t *c = malloc(sizeof(core_t));

    init_game(c);
    sfRenderWindow_setFramerateLimit(c->ren.w, 60);
    //sfThread_launch(c->pathfinding_thread);
    //sfThread_launch(c->foodpickup_thread);
    //sfThread_launch(c->quadtree_thread);
    while (sfRenderWindow_isOpen(c->ren.w)) {
        c->mouse.pos.x = sfMouse_getPositionRenderWindow(c->ren.w).x;
        c->mouse.pos.y = sfMouse_getPositionRenderWindow(c->ren.w).y;
        render_game(c);
    }
}

int main(int ac, char **av)
{
    return game_loop();
}
