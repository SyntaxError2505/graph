#include <math.h>
#include <stdbool.h>
#include <limits.h>
#include <raylib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define STANDARD_CAPACITY 10
#define STANDARD_RADIUS 20.0

typedef struct _ball_system{
	int* x;
	int* y;
	int capacity;
	int size;
} ball_system;

void ball_system_init(ball_system* system){
	system->x = malloc(sizeof(int)*STANDARD_CAPACITY);
	system->y = malloc(sizeof(int)*STANDARD_CAPACITY);
	system->capacity = STANDARD_CAPACITY;
	system->size = 0;
	printf("Init\n");
}

void ball_system_free(ball_system* system){
	free(system->x);
	free(system->y);
	system->x = NULL;
	system->y = NULL;
	system->capacity = 0;
	system->size = 0;
	printf("Freed\n");
}

void ball_system_add_ball(ball_system* system, int x, int y){
	if(system->size >= system->capacity){
		system->capacity *= 2;
		printf("Multiplied\n");
		
		system->x = realloc(system->x, sizeof(int)*system->capacity);
		system->y = realloc(system->y, sizeof(int)*system->capacity);
	}

	system->x[system->size] = x;
	system->y[system->size] = y;
	system->size += 1;
}

// ONLY BETWEEN Begin and EndDrawing() functions by raylib
void ball_system_draw_balls(ball_system* system, int touched_index, bool touched){
	for(int i = 0; i < system->size; i++){
		DrawCircle(system->x[i], system->y[i], STANDARD_RADIUS, (touched && i == touched_index) ? PURPLE : RED);
	}
}

// Returns INDEX of touched Ball. Returns system.size + 1 if no ball is touched
int ball_system_check_mouse_touch(ball_system* system, bool* touched){
	int smallest_index = system->size + 1;
	float smallest_distance = INT_MAX;
	*touched = false;

	for(int i = 0; i < system->size; i++){
		float distance_x = abs(system->x[i] - GetMouseX());
		float distance_y = abs(system->y[i] - GetMouseY());
		float distance = sqrt(distance_x * distance_x + distance_y * distance_y);

		smallest_index = (distance < smallest_distance) ? i : smallest_index;
		smallest_distance = (distance < smallest_distance) ? distance : smallest_distance;
	}

	*touched = (smallest_distance < STANDARD_RADIUS);
	return smallest_index;
}

int main(void)
{
    InitWindow(800, 450, "raylib [core] example - basic window");
	ball_system system;
	ball_system_init(&system);

	for(int i = 0; i <= 10; i++){
		ball_system_add_ball(&system, i * 30, 40);
	}

    while (!WindowShouldClose())
    {
		bool touched;

        BeginDrawing();
		int index = ball_system_check_mouse_touch(&system, &touched);
		if(touched){
			printf("Touching index: %i\n", index);
		}
		ball_system_draw_balls(&system, index, touched);
        ClearBackground(BLACK);
        EndDrawing();
    }

	ball_system_free(&system);
    CloseWindow();

    return 0;
}
