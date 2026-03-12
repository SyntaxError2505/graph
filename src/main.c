#include <math.h>
#include <stdbool.h>
#include <limits.h>
#include <raylib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define STANDARD_CAPACITY 10
#define STANDARD_RADIUS 20.0

#define REPULSION_MULTIPLIER 50
#define GRAVITY_MULTIPLIER 1

typedef struct _ball_system{
	float* x;
	float* x_vel;
	float* y;
	float* y_vel;
	int capacity;
	int size;
} ball_system;

void ball_system_init(ball_system* system){
	system->x = malloc(sizeof(float)*STANDARD_CAPACITY);
	system->x_vel = malloc(sizeof(float)*STANDARD_CAPACITY);
	system->y = malloc(sizeof(float)*STANDARD_CAPACITY);
	system->y_vel = malloc(sizeof(float)*STANDARD_CAPACITY);
	system->capacity = STANDARD_CAPACITY;
	system->size = 0;
	printf("Init\n");
}

void ball_system_free(ball_system* system){
	free(system->x);
	free(system->y);
	free(system->y_vel);
	free(system->x_vel);
	system->x = NULL;
	system->y = NULL;
	system->x_vel = NULL;
	system->y_vel = NULL;
	system->capacity = 0;
	system->size = 0;
}

void ball_system_add_ball(ball_system* system, int x, int y){
	if(system->size >= system->capacity){
		system->capacity *= 2;
		
		system->x = realloc(system->x, sizeof(float)*system->capacity);
		system->x_vel = realloc(system->x_vel, sizeof(float)*system->capacity);
		system->y = realloc(system->y, sizeof(float)*system->capacity);
		system->y_vel = realloc(system->y_vel, sizeof(float)*system->capacity);
	}

	system->x[system->size] = x;
	system->y[system->size] = y;
	system->x_vel[system->size] = 0;
	system->y_vel[system->size] = 0;
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
		float distance_x = fabs(system->x[i] - GetMouseX());
		float distance_y = fabs(system->y[i] - GetMouseY());
		float distance = sqrt(distance_x * distance_x + distance_y * distance_y);

		smallest_index = (distance < smallest_distance) ? i : smallest_index;
		smallest_distance = (distance < smallest_distance) ? distance : smallest_distance;
	}

	*touched = (smallest_distance < STANDARD_RADIUS);
	return smallest_index;
}

void ball_system_update(ball_system* system){
	for(int i = 0; i < system->size; i++){
		for(int j = 0; j < system->size; j++){
			if(i != j){
				float distance_x = system->x[j] - system->x[i];
				float distance_y = system->y[j] - system->y[i];
				float distance = sqrt((distance_x * distance_x) + (distance_y * distance_y));

				if(distance == 0){
					distance = 1;
				}

				float force = GRAVITY_MULTIPLIER * ((STANDARD_RADIUS*STANDARD_RADIUS) / (distance * distance));
				float repulsion = REPULSION_MULTIPLIER * ((STANDARD_RADIUS * STANDARD_RADIUS) / (distance * distance * distance));

				
				// Initialize and normalize direction vector
				float direction_vector_x = distance_x / distance;
				float direction_vector_y = distance_y / distance;
				// Transliate to moement vector
				float movement_vector_x = force * direction_vector_x;
				float movement_vector_y = force * direction_vector_y;
				// Translate a repulsion vector
				float repulsion_vector_x = repulsion * direction_vector_x;
				float repulsion_vector_y = repulsion * direction_vector_y;

				system->x_vel[i] -= repulsion_vector_x;
				system->y_vel[i] -= repulsion_vector_y;
				system->x_vel[i] += movement_vector_x;
				system->y_vel[i] += movement_vector_y;
			}
		}
	}
	
	for(int i = 0; i < system->size; i++){
		system->x[i] += system->x_vel[i] * GetFrameTime();
		system->y[i] *= 0.99;
		system->y[i] += system->y_vel[i] * GetFrameTime();
		system->y[i] *= 0.99;
	}
}

int main(void)
{
    InitWindow(800, 450, "Graph");
	ball_system system;
	ball_system_init(&system);

	//srand(time(NULL));

	for(int i = 0; i <= 10; i++){
		ball_system_add_ball(&system, rand() % 800, rand() % 450);
	}


    while (!WindowShouldClose())
    {
		bool touched;
		bool grabbing;
		int touched_index = ball_system_check_mouse_touch(&system, &touched);
		ball_system_update(&system);

        BeginDrawing();
		ball_system_draw_balls(&system, touched_index, touched);
        ClearBackground(BLACK);
        EndDrawing();
    }

	ball_system_free(&system);
    CloseWindow();

    return 0;
}
