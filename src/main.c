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

#define DRAG_COEFFICIENT 5

typedef struct _ball_system{
	float* x;
	float* x_vel;
	float* y;
	float* y_vel;
	int capacity;
	int size;
} ball_system;

void hookes_spring(float ax, float ay, float bx, float by, float *x_vel, float *y_vel, float prefered_distance, float stiffness){
	float distance_x = bx - ax;
	float distance_y = by - ay;
	float distance = sqrt((distance_x * distance_x) + (distance_y * distance_y));
	float force = stiffness * (prefered_distance - distance);
	float normalized_vector_x = distance_x / distance;
	float normalized_vector_y = distance_y / distance;
	float movement_vector_x = force * normalized_vector_x;
	float movement_vector_y = force * normalized_vector_y;
	*x_vel -= movement_vector_x * GetFrameTime();
	*y_vel -= movement_vector_y * GetFrameTime();
}

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

void ball_system_update(ball_system* system, bool grabbing, int grabbed_index){
	for(int i = 0; i < system->size; i++){
		if(grabbing && i == grabbed_index){
			hookes_spring(system->x[i], system->y[i], GetMouseX(), GetMouseY(), &system->x_vel[i], &system->y_vel[i], 0, 1000);
		}

		for(int j = 0; j < system->size; j++){
			if(i != j){
				hookes_spring(system->x[i], system->y[i], system->x[j], system->y[j], &system->x_vel[i], &system->y_vel[i], 250, 100);
			}
		}

		hookes_spring(system->x[i], system->y[i], GetScreenWidth()/2.0, GetScreenHeight()/2.0, &system->x_vel[i], &system->y_vel[i], 0, 50);

		system->x_vel[i] *= 1 - DRAG_COEFFICIENT * GetFrameTime();
		system->y_vel[i] *= 1 - DRAG_COEFFICIENT * GetFrameTime();
	}
	
	for(int i = 0; i < system->size; i++){
		system->x[i] += system->x_vel[i] * GetFrameTime();
		system->y[i] += system->y_vel[i] * GetFrameTime();
	}
}

int main(void){
	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(800, 450, "Graph");
	ball_system system;
	ball_system_init(&system);
	SetTargetFPS(60);

	srand(time(NULL));

	for(int i = 0; i <= 4; i++){
		ball_system_add_ball(&system, rand() % 800, rand() % 450);
	}


    while (!WindowShouldClose())
    {
		bool touched;
		int touched_index = ball_system_check_mouse_touch(&system, &touched);
		ball_system_update(&system, touched && IsMouseButtonDown(MOUSE_BUTTON_LEFT), touched_index);

        BeginDrawing();
		ball_system_draw_balls(&system, touched_index, touched);
        ClearBackground(BLACK);
        EndDrawing();
    }

	ball_system_free(&system);
    CloseWindow();

    return 0;
}
