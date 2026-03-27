#include <iostream>
#include <raylib.h>
#include <rlgl.h>
#include <string>
#include <cmath>
#include <vector>

const int SCREENWIDTH = 1280;
const int SCREENHEIGHT = 720;
const int RAYEMITTERSIDE = 1;
const int RAYEMITTERVELOCITY = 100;
float RAYEMITTERX = 0;
float RAYEMITTERY = 0;
const int MAXY = SCREENHEIGHT - RAYEMITTERSIDE;
const int MAXX = SCREENWIDTH - RAYEMITTERSIDE;
int NUMBEROFRAYS = 100000;
float RAYANGLE = 180.0f / NUMBEROFRAYS;
const float RAYLENGTH = std::sqrt(SCREENHEIGHT * SCREENHEIGHT + SCREENWIDTH * SCREENWIDTH);
std::string RAYDIRECTION;
std::vector<float> VERTEXESX;
std::vector<float> VERTEXESY;
std::vector<char> SHAPES;



void DrawRayEmitter() {
	double DELTATIME = GetFrameTime();
	if (RAYEMITTERY<=0 && RAYEMITTERX>=0 && RAYEMITTERX<MAXX) {
		if (RAYEMITTERY <= 0 && RAYEMITTERX == 0) {
			RAYEMITTERY = 0;
			RAYDIRECTION = "DOWN";
		}
		RAYEMITTERX += RAYEMITTERVELOCITY * DELTATIME;
	}
	else if (RAYEMITTERY >= 0 && RAYEMITTERX >= MAXX && RAYEMITTERY<MAXY) {
		if (RAYEMITTERX >= (SCREENWIDTH - RAYEMITTERSIDE)) {
			RAYEMITTERX = SCREENWIDTH - RAYEMITTERSIDE;
			RAYDIRECTION = "LEFT";
		}
		RAYEMITTERY += RAYEMITTERVELOCITY * DELTATIME;
	}
	else if (RAYEMITTERY >= MAXY && RAYEMITTERX <= MAXX && RAYEMITTERX>0) {
		if (RAYEMITTERY >= MAXY && RAYEMITTERX == MAXX) {
			RAYEMITTERY = MAXY;
			RAYDIRECTION = "UP";
		}
		RAYEMITTERX -= RAYEMITTERVELOCITY * DELTATIME;
	}
	else if (RAYEMITTERY <= MAXY && RAYEMITTERX <= 0 && RAYEMITTERY>0) {
		if (RAYEMITTERY == MAXY && RAYEMITTERX <= 0) {
			RAYEMITTERY = MAXY;
			RAYEMITTERX = 0;
			RAYDIRECTION = "RIGHT";
		}
		RAYEMITTERY -= RAYEMITTERVELOCITY * DELTATIME;
	}
	DrawRectangle(RAYEMITTERX, RAYEMITTERY, RAYEMITTERSIDE, RAYEMITTERSIDE, BLUE);
}

float GetRayIntersection(float STARTX, float STARTY, float& ENDX, float& ENDY) {
	float LEASTMULTIPLIER = 1;
	int MAXINDEX;
	if (VERTEXESX.size() >= VERTEXESY.size()) MAXINDEX = VERTEXESX.size();
	else MAXINDEX = VERTEXESY.size();
	int INDEX = 0;
	while (INDEX < MAXINDEX) {
		char SHAPE = SHAPES[INDEX];
		if (SHAPE == 'R') {
			float X1 = VERTEXESX[INDEX];
			float X2 = VERTEXESX[INDEX + 1];
			float X3 = VERTEXESX[INDEX + 2];
			float X4 = VERTEXESX[INDEX + 3];
			float Y1 = VERTEXESY[INDEX];
			float Y2 = VERTEXESY[INDEX + 1];
			float Y3 = VERTEXESY[INDEX + 2];
			float Y4 = VERTEXESY[INDEX + 3];
			for (int VERTEX = 0; VERTEX < 4; VERTEX++){
				float STARTVERTEXX = VERTEXESX[INDEX + VERTEX];
				float ENDVERTEXX = VERTEXESX[INDEX + (VERTEX+1)%4];
				float STARTVERTEXY = VERTEXESY[INDEX + VERTEX];
				float ENDVERTEXY = VERTEXESY[INDEX + (VERTEX + 1) % 4];
				float DENOMINATOR = (STARTX - ENDX) * (STARTVERTEXY - ENDVERTEXY) - (STARTY - ENDY) * (STARTVERTEXX - ENDVERTEXX);
				if (DENOMINATOR == 0) continue;
				float t = (((STARTX - STARTVERTEXX) * (STARTVERTEXY - ENDVERTEXY)) - ((STARTY - STARTVERTEXY) * (STARTVERTEXX - ENDVERTEXX))) / DENOMINATOR;
				float u = (((STARTX - STARTVERTEXX) * (STARTY - ENDY)) - ((STARTY - STARTVERTEXY) * (STARTX - ENDX))) / DENOMINATOR;
				if (t >= 0 && t <= 1 && u>=0 && u<=1) {
					if (LEASTMULTIPLIER > t) LEASTMULTIPLIER = t;
				}
			}
			INDEX += 4;
		}
	}
	return LEASTMULTIPLIER;
}


void EmitRays() {
	rlBegin(RL_LINES);
	rlColor4f(0.0f, 0.0f, 1.0f, 0.4f);
	int STARTANGLE = 0;
	float STARTX = RAYEMITTERX + (RAYEMITTERSIDE / 2.0f);
	float STARTY = RAYEMITTERY + (RAYEMITTERSIDE / 2.0f);
	if (RAYDIRECTION == "DOWN") STARTANGLE = 0;
	else if (RAYDIRECTION == "LEFT") STARTANGLE = 90;
	else if (RAYDIRECTION == "UP") STARTANGLE = 180;
	else if (RAYDIRECTION == "RIGHT") STARTANGLE = 270;
	for (int RAYNUMBER = 0; RAYNUMBER < NUMBEROFRAYS; RAYNUMBER++) {
		float ANGLE = STARTANGLE + (RAYANGLE * RAYNUMBER);
		ANGLE = ANGLE * (3.141592653589879323 / 180.0f); //deg to rad
		float ENDX = STARTX + std::cos(ANGLE) * RAYLENGTH;
		float ENDY = STARTY + std::sin(ANGLE) * RAYLENGTH;
		float MULTIPLIER = GetRayIntersection(STARTX, STARTY, ENDX, ENDY);
		ENDX = STARTX + MULTIPLIER * (ENDX - STARTX);
		ENDY = STARTY + MULTIPLIER * (ENDY - STARTY);
		rlVertex2f(STARTX, STARTY);
		rlVertex2f(ENDX, ENDY);
	}
	rlEnd();
}


int main() {
	
	InitWindow(1280, 720, "RayTracing");
	SHAPES.push_back('R');
	VERTEXESX.push_back(100);
	VERTEXESX.push_back(100);
	VERTEXESX.push_back(300);
	VERTEXESX.push_back(300);
	VERTEXESY.push_back(100);
	VERTEXESY.push_back(300);
	VERTEXESY.push_back(300);
	VERTEXESY.push_back(100);
	while (!WindowShouldClose())
	{
		BeginDrawing();
		ClearBackground(BLACK);
		DrawRectangle(100, 100, 200, 200, RED);
		if (IsKeyPressed(KEY_DOWN)) {
			NUMBEROFRAYS /= 2;
			RAYANGLE = 180.0f / NUMBEROFRAYS;
		}
		else if (IsKeyPressed(KEY_UP)) {
			if (NUMBEROFRAYS == 0 ) NUMBEROFRAYS = 2;
			else NUMBEROFRAYS *= 2;
			RAYANGLE = 180.0f / NUMBEROFRAYS;
		}

		DrawRayEmitter();
		EmitRays();
		EndDrawing();
	}
	CloseWindow();
	return 0;
}