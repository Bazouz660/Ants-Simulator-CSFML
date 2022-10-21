/*
** EPITECH PROJECT, 2021
** B-MUL-200-STG-2-1-myrpg-basile.trebus-hamann
** File description:
** new_sound.c
*/

#include "ants.h"

sfSound *set_up_new_sound(sfSound *new_sound, const char *buffer_location)
{
    sfSoundBuffer *new_sound_buffer = sfSoundBuffer_createFromFile(buffer_location);

    new_sound = sfSound_create();
    sfSound_setBuffer(new_sound, new_sound_buffer);
    return (new_sound);
}
