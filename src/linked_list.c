/*
** EPITECH PROJECT, 2021
** smoke test
** File description:
** linked_list.c
*/

#include "ants.h"

void add_ant(core_t *c, ant_t **head, colony_t colony)
{
	static int index = 0;
	ant_t *nnode;
	float angle = get_angle(colony.pos, colony.spwn_coords[index]);

	nnode = malloc(sizeof(ant_t));
    nnode->part = sfRectangleShape_create();
	nnode->index = index;
	nnode->dir_chg_clock = 0;
    nnode->clock = c->clock.seconds;
	nnode->status = 1;
	nnode->phe_clock = nnode->clock;
	nnode->last_home = 0;
	nnode->last_food = 0;
	nnode->following = True;
    sfRectangleShape_setFillColor(nnode->part, (sfColor){255, 255, 255, 255});
    sfRectangleShape_setSize(nnode->part, (sfVector2f){10, 10});
    sfRectangleShape_setPosition(nnode->part, colony.spwn_coords[index]);
    sfRectangleShape_setOrigin(nnode->part, get_rect_center(nnode->part));
    sfRectangleShape_setTexture(nnode->part, c->textures.ant, sfTrue);
	nnode->dir = (sfVector2f){rand_bool(), rand_bool()};
	nnode->dir_angle = angle;
	sfRectangleShape_setRotation(nnode->part, nnode->dir_angle);
	nnode->vect = (sfVector2f){cos(deg_to_rad(angle)), sin(deg_to_rad(angle))};
	nnode->next = (*head);
	nnode->prev = NULL;
	if ((*head) != NULL)
		(*head)->prev = nnode;
	(*head) = nnode;
	index++;
}

void del_ant(ant_t **head, ant_t *del_node)
{
	if (*head == NULL || del_node == NULL)
		return ;
	if (*head == del_node)
		*head = del_node->next;
	if (del_node->next != NULL)
		del_node->next->prev = del_node->prev;
	if (del_node->prev != NULL)
		del_node->prev->next = del_node->next;
	free(del_node);
}

void add_phe(core_t *c, pheromones_t **head, ant_t ant, float seconds)
{
	static int index = 0;
	pheromones_t *nnode;

	nnode = malloc(sizeof(pheromones_t));
	nnode->vertex.position = ant.pos;
	nnode->type = ant.status;
	nnode->pos = ant.pos;
	nnode->dist = ant.last_home;
	nnode->intensity = ant.last_food;
	if (nnode->type == 1)
	    nnode->vertex.color = sfBlue;
	if (nnode->type == 2)
	    nnode->vertex.color = sfMagenta;
	nnode->vertex.texCoords = (sfVector2f){0, 0};
	nnode->start = 0;
	nnode->alpha = 255;
	nnode->next = (*head);
	nnode->prev = NULL;
	if ((*head) != NULL)
		(*head)->prev = nnode;
	(*head) = nnode;
	index++;
}

void add_phe2(core_t *c, pheromones_t **head, sfVector2f pos, float seconds)
{
	static int index = 0;
	pheromones_t *nnode;

	nnode = malloc(sizeof(pheromones_t));
	nnode->vertex.position = pos;
	nnode->type = 1;
	nnode->pos = pos;
	nnode->dist = 0;
	nnode->intensity = 10;
	if (nnode->type == 1)
	    nnode->vertex.color = sfBlue;
	if (nnode->type == 2)
	    nnode->vertex.color = sfMagenta;
	nnode->vertex.texCoords = (sfVector2f){0, 0};
	nnode->start = 0;
	nnode->alpha = 255;
	nnode->next = (*head);
	nnode->prev = NULL;
	if ((*head) != NULL)
		(*head)->prev = nnode;
	(*head) = nnode;
	index++;
}

void del_phe(pheromones_t **head, pheromones_t *del_node)
{
	if (*head == NULL || del_node == NULL)
		return ;
	if (*head == del_node)
		*head = del_node->next;
	if (del_node->next != NULL)
		del_node->next->prev = del_node->prev;
	if (del_node->prev != NULL)
		del_node->prev->next = del_node->next;
	free(del_node);
}

void add_food(core_t *c, food_t **head, sfVector2f pos)
{
	static int index = 0;
	food_t *nnode;

	nnode = malloc(sizeof(ant_t));
	nnode->pos = pos;
	nnode->value = 2000;
	nnode->shape = sfCircleShape_create();
	sfCircleShape_setFillColor(nnode->shape, sfGreen);
	sfCircleShape_setRadius(nnode->shape, 20);
	sfCircleShape_setOrigin(nnode->shape, (sfVector2f){10, 10});
    sfCircleShape_setPosition(nnode->shape, nnode->pos);
	nnode->next = (*head);
	nnode->prev = NULL;
	if ((*head) != NULL)
		(*head)->prev = nnode;
	(*head) = nnode;
	index++;
}

void del_food(food_t **head, food_t *del_node)
{
	if (*head == NULL || del_node == NULL)
		return ;
	if (*head == del_node)
		*head = del_node->next;
	if (del_node->next != NULL)
		del_node->next->prev = del_node->prev;
	if (del_node->prev != NULL)
		del_node->prev->next = del_node->next;
	sfCircleShape_destroy(del_node->shape);
	free(del_node);
}
