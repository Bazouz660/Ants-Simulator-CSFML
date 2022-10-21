/*
** EPITECH PROJECT, 2021
** ants (copie 1)
** File description:
** circle_bounds.c
*/

#include "ants.h"

sfBool circle_intersect(sfVector2f center, float radius, sfVector2f point)
{
    if (pow(point.x - center.x, 2) + pow(point.y - center.y,
        2) < pow(radius, 2))
        return sfTrue;
    return sfFalse;
}
