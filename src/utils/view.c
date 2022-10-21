/*
** EPITECH PROJECT, 2021
** B-MUL-200-STG-2-1-myrpg-basile.trebus-hamann
** File description:
** view.c
*/

#include "ants.h"

sfFloatRect view_get_global_bounds(sfView *view)
{
    sfVector2f center = sfView_getCenter(view);
    sfVector2f size = sfView_getSize(view);
    sfFloatRect rect;

    rect.width = size.x;
    rect.height  = size.y;
    rect.left = center.x - (size.x / 2);
    rect.top = center.y - (size.y / 2);
    return rect;
}
