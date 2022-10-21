/*
** EPITECH PROJECT, 2021
** ants
** File description:
** quadtree.c
*/

#include "ants.h"

quadtree_t *new_quadtree(sfFloatRect bounds, int capacity, sfColor color)
{
    quadtree_t *quadtree = malloc(sizeof(quadtree_t));

    quadtree->bounds = bounds;
    quadtree->capacity = capacity;
    quadtree->divided = False;
    quadtree->ne = NULL;
    quadtree->nw = NULL;
    quadtree->se = NULL;
    quadtree->sw = NULL;
    quadtree->fullness = 0;
    quadtree->quad = sfRectangleShape_create();
    sfRectangleShape_setFillColor(quadtree->quad, sfTransparent);
    sfRectangleShape_setOutlineColor(quadtree->quad, color);
    sfRectangleShape_setOutlineThickness(quadtree->quad, 1);
    sfRectangleShape_setSize(quadtree->quad, (sfVector2f){quadtree->bounds.width,
    quadtree->bounds.height});
    sfRectangleShape_setPosition(quadtree->quad, (sfVector2f){quadtree->bounds.left,
    quadtree->bounds.top});
    quadtree->phe = malloc(sizeof(phe_t) * capacity);
    return quadtree;
}

static void copy_content(pheromones_t *p1, phe_t *p2)
{
    p2->alpha = p1->alpha;
    p2->color = p1->color;
    p2->dist = p1->dist;
    p2->fade_clock = p1->fade_clock;
    p2->intensity = p1->intensity;
    p2->pos = p1->pos;
    p2->start = p1->start;
    p2->type = p1->type;
    p2->vertex = p1->vertex;
}

void subdivide(quadtree_t *quadtree, int capacity)
{
    sfFloatRect bounds = quadtree->bounds;

    quadtree->nw = new_quadtree((sfFloatRect){bounds.left, bounds.top,
    bounds.width / 2, bounds.height / 2}, capacity, sfBlue);
    quadtree->ne = new_quadtree((sfFloatRect){bounds.left + (bounds.width / 2),
    bounds.top, bounds.width / 2, bounds.height / 2}, capacity, sfRed);
    quadtree->sw = new_quadtree((sfFloatRect){bounds.left,
    bounds.top + (bounds.height / 2), bounds.width / 2, bounds.height / 2},
    capacity, sfYellow);
    quadtree->se = new_quadtree((sfFloatRect){bounds.left + (bounds.width / 2),
    bounds.top + (bounds.height / 2), bounds.width / 2, bounds.height / 2},
    capacity, sfGreen);
}

void insert_point(core_t *c, quadtree_t *quadtree, phe_t phe)
{
    if (!sfFloatRect_contains(&quadtree->bounds, phe.pos.x, phe.pos.y))
        return;
    if (quadtree->capacity > quadtree->fullness) {
        quadtree->phe[quadtree->fullness] = phe;
        if (quadtree->fullness + 1 <= quadtree->capacity)
            quadtree->fullness++;
    } else {
        if (quadtree->divided == False) {
            subdivide(quadtree, quadtree->capacity);
            quadtree->divided = True;
        }
        insert_point(c, quadtree->nw, phe);
        insert_point(c, quadtree->ne, phe);
        insert_point(c, quadtree->sw, phe);
        insert_point(c, quadtree->se, phe);
    }
}

void draw_quadtree(core_t *c, quadtree_t *quadtree)
{
    if (quadtree == NULL)
        return;
    sfRenderWindow_drawRectangleShape(c->ren.w, quadtree->quad, NULL);
    if (quadtree->divided == True) {
        draw_quadtree(c, quadtree->nw);
        draw_quadtree(c, quadtree->ne);
        draw_quadtree(c, quadtree->sw);
        draw_quadtree(c, quadtree->se);
    }
}

void concat_array(phe_t **dest, phe_t *src)
{
    int dest_len = 0;
    int src_len = 0;
    int x = 0;
    phe_t *tmp = *dest;
    phe_t *res;

    if (src == NULL)
        return;
    while (src[src_len].type != -1)
        src_len++;
    if (tmp == NULL) {
        res = malloc(sizeof(phe_t) * (dest_len + src_len + 1));
        for (int i = 0; i < src_len; i++)
            res[i] = src[i];
    } else {
        while (tmp[dest_len].type != -1)
            dest_len++;
        res = malloc(sizeof(phe_t) * (dest_len + src_len + 1));
        for (int i = 0; i < dest_len; i++)
            res[i] = tmp[i];
        for (int i = dest_len; i < (src_len + dest_len); i++, x++) {
            res[i] = src[x];
        }
    }
    res[src_len + dest_len].type = -1;
    *dest = res;
    free(tmp);
    free(src);
}

phe_t *check_region(quadtree_t *quadtree, sfFloatRect range)
{
    sfFloatRect r1 = quadtree->bounds;
    sfFloatRect r2 = range;
    phe_t *found = NULL;
    int len = 0;

    if (sfFloatRect_intersects(&r1, &r2, NULL) == sfFalse) {
        return found;
    } else {
        for (int i = 0; i < quadtree->capacity; ++i) {
            if (sfFloatRect_contains(&r2, quadtree->phe[i].pos.x,
                quadtree->phe[i].pos.y)) {
                found = realloc(found, sizeof(phe_t) * (len + 1));
                found[len] = quadtree->phe[i];
                len++;
            }
        }
        found = realloc(found, sizeof(phe_t) * (len + 1));
        found[len].type = -1;
        if (quadtree->divided == True) {
            concat_array(&found, check_region(quadtree->nw, range));
            concat_array(&found, check_region(quadtree->ne, range));
            concat_array(&found, check_region(quadtree->sw, range));
            concat_array(&found, check_region(quadtree->se, range));
        }
        return found;
    }

}

phe_t *get_points_in_range(quadtree_t *quadtree, sfFloatRect range)
{
    phe_t *found = NULL;

    if (quadtree != NULL) {
        found = check_region(quadtree, range);
        return found;
    }
    return NULL;
}

void free_quadtree(quadtree_t *quadtree)
{
    if (quadtree == NULL)
        return;
    if (quadtree->divided == True) {
        free_quadtree(quadtree->ne);
        free_quadtree(quadtree->nw);
        free_quadtree(quadtree->se);
        free_quadtree(quadtree->sw);
    }
    free(quadtree->phe);
    sfRectangleShape_destroy(quadtree->quad);
    free(quadtree);
}

void generate_quadtree(core_t *c, quadtree_t *quadtree, pheromones_t **head)
{
    pheromones_t *tmp = *head;
    phe_t phe;

    c->cly.nb_phe = 0;
    if (quadtree != NULL)
        free_quadtree(quadtree);
    quadtree = new_quadtree((sfFloatRect){0, 0,
    sfRenderWindow_getSize(c->ren.w).x,
    sfRenderWindow_getSize(c->ren.w).y}, 70, sfBlue);
    while (tmp != NULL) {
        copy_content(tmp, &phe);
        insert_point(c, quadtree, phe);
        tmp = tmp->next;
        c->cly.nb_phe++;
    }
    free(tmp);
    c->quadtree = quadtree;
}