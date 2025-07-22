#pragma once

#include "gameguru.h"

struct entity_thread_data
{
	int e;
};

struct entity_init_thread_data
{
	int e;
};

void entity_loop_thread(entity_thread_data* pData);
void entity_init_thread_part1(entity_init_thread_data* pData);
void entity_init_thread_part2(entity_init_thread_data* pData);
void entity_init_thread(entity_init_thread_data* pData);
