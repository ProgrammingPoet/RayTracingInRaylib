#include <iostream>
#include <raylib.h>
#include <rlgl.h>
#include <cmath>
#include <vector>

constexpr int SCREENWIDTH = 1280;
constexpr int SCREENHIEGHT = 720;

struct LINE {
	float x1, y1, x2, y2;
};

std::vector<LINE> WALLS;
std::vector<LINE> RAYS;
std::vector<char> SHAPE;
float RAYALPHA = 1.0f;
float INNERALPHA = 0.5f;



struct RAYINFO
{
	bool HIT = false;
	char HITTYPE = 'W';
	float LEASTMULTIPLIER = 1;
	float GREATESTMULTIPLIER = 0;
	Vector2 WALLSTART;
	Vector2 WALLEND;
};

struct RE {
	float X = 0;
	float Y = 0;
	int VELOCITY = 600;
	const int SIDELENGTH = 1;
	char DIRECTION = 'R';
	int NUMBEROFRAYS = 100000;
	int ANGLE = 180;
	float RAYANGLE;
	float RAYLENGTH;
	void UpdateRayAngle() {
		RAYANGLE = (float)180 / NUMBEROFRAYS;
	}
	void SetRayLength() {
		RAYLENGTH = std::sqrt(SCREENHIEGHT * SCREENHIEGHT + SCREENWIDTH * SCREENWIDTH);
	}
};

RE RAYEMITTER;
void DrawRayEmitter() {
	static const int MAXY = SCREENHIEGHT - RAYEMITTER.SIDELENGTH;
	static const int MAXX = SCREENWIDTH - RAYEMITTER.SIDELENGTH;

	double DELTATIME = GetFrameTime();

	if (RAYEMITTER.Y <= 0 && RAYEMITTER.X >= 0 && RAYEMITTER.X < MAXX) {
		if (RAYEMITTER.Y <= 0 && RAYEMITTER.X == 0) {
			RAYEMITTER.Y = 0;
			RAYEMITTER.DIRECTION = 'D';
		}
		RAYEMITTER.X += RAYEMITTER.VELOCITY * DELTATIME;
	}
	else if (RAYEMITTER.Y >= 0 && RAYEMITTER.X >= MAXX && RAYEMITTER.Y < MAXY) {
		if (RAYEMITTER.X >= (SCREENWIDTH - RAYEMITTER.SIDELENGTH)) {
			RAYEMITTER.X = SCREENWIDTH - RAYEMITTER.SIDELENGTH;
			RAYEMITTER.DIRECTION = 'L';
		}
		RAYEMITTER.Y += RAYEMITTER.VELOCITY * DELTATIME;
	}
	else if (RAYEMITTER.Y >= MAXY && RAYEMITTER.X <= MAXX && RAYEMITTER.X > 0) {
		if (RAYEMITTER.Y >= MAXY && RAYEMITTER.X == MAXX) {
			RAYEMITTER.Y = MAXY;
			RAYEMITTER.DIRECTION = 'U';
		}
		RAYEMITTER.X -= RAYEMITTER.VELOCITY * DELTATIME;
	}
	else if (RAYEMITTER.Y <= MAXY && RAYEMITTER.X <= 0 && RAYEMITTER.Y > 0) {
		if (RAYEMITTER.Y == MAXY && RAYEMITTER.X <= 0) {
			RAYEMITTER.Y = MAXY;
			RAYEMITTER.X = 0;
			RAYEMITTER.DIRECTION = 'R';
		}
		RAYEMITTER.Y -= RAYEMITTER.VELOCITY * DELTATIME;
	}
	DrawRectangle(RAYEMITTER.X, RAYEMITTER.Y, RAYEMITTER.SIDELENGTH, RAYEMITTER.SIDELENGTH, BLUE);
}

void CreateRays() {
	RAYS.clear();
	int STARTANGLE = 0;
	float STARTX = RAYEMITTER.X + (RAYEMITTER.SIDELENGTH / 2.0f);
	float STARTY = RAYEMITTER.Y + (RAYEMITTER.SIDELENGTH / 2.0f);
	if (RAYEMITTER.DIRECTION == 'D') STARTANGLE = 0;
	else if (RAYEMITTER.DIRECTION == 'L') STARTANGLE = 90;
	else if (RAYEMITTER.DIRECTION == 'U') STARTANGLE = 180;
	else if (RAYEMITTER.DIRECTION == 'R') STARTANGLE = 270;
	for (int RAYNUMBER = 0; RAYNUMBER < RAYEMITTER.NUMBEROFRAYS; RAYNUMBER++) {
		float ANGLE = STARTANGLE + (RAYEMITTER.RAYANGLE * RAYNUMBER);
		ANGLE = ANGLE * (3.141592653589879323 / 180.0f); //deg to rad
		float ENDX = STARTX + std::cos(ANGLE) * RAYEMITTER.RAYLENGTH;
		float ENDY = STARTY + std::sin(ANGLE) * RAYEMITTER.RAYLENGTH;
		RAYS.push_back({ STARTX,STARTY,ENDX,ENDY });
	}
}




RAYINFO GetRayIntersection(LINE RAY) {
	RAYINFO HIT;
	char MODE = 'W';
	for (int WALLINDEX = 0; WALLINDEX < WALLS.size(); WALLINDEX++) {
		LINE WALL = WALLS[WALLINDEX];
		if (std::isnan(WALLS[WALLINDEX].x1) && std::isnan(WALLS[WALLINDEX].y1) && std::isnan(WALLS[WALLINDEX].x2) && MODE == 'W' && std::isnan(WALLS[WALLINDEX].y2)) { MODE = 'E'; }
		else if (std::isnan(WALLS[WALLINDEX].x1) && std::isnan(WALLS[WALLINDEX].y1) && std::isnan(WALLS[WALLINDEX].x2) && std::isnan(WALLS[WALLINDEX].y2) && MODE == 'E') { MODE = 'W'; }
		float DENOMINATOR = (RAY.x1 - RAY.x2) * (WALL.y1 - WALL.y2) - (RAY.y1 - RAY.y2) * (WALL.x1 - WALL.x2);
		if (DENOMINATOR == 0) { continue; }
		float t = (((RAY.x1 - WALL.x1) * (WALL.y1 - WALL.y2)) - ((RAY.y1 - WALL.y1) * (WALL.x1 - WALL.x2))) / DENOMINATOR;
		float u = (((RAY.x1 - WALL.x1) * (RAY.y1 - RAY.y2)) - ((RAY.y1 - WALL.y1) * (RAY.x1 - RAY.x2))) / DENOMINATOR;
		if (t >= 0 && t <= 1 && u >= 0 && u <= 1) {
			HIT.HIT = true;
			if (MODE == 'E') {
				if (HIT.GREATESTMULTIPLIER < t) { HIT.GREATESTMULTIPLIER = t; HIT.HITTYPE = 'E'; }
				if (HIT.LEASTMULTIPLIER > t) { HIT.LEASTMULTIPLIER = t; HIT.HITTYPE = 'E'; HIT.WALLSTART = { WALL.x1, WALL.y1 }; HIT.WALLEND = { WALL.x2, WALL.y2 }; }
			}
			else if (MODE == 'W') {
				if (HIT.LEASTMULTIPLIER > t) { HIT.LEASTMULTIPLIER = t; HIT.HITTYPE = 'W'; HIT.WALLSTART = { WALL.x1, WALL.y1 }; HIT.WALLEND = { WALL.x2, WALL.y2 }; }
			}
		}
	}
	return HIT;
}


void BounceRay(LINE RAY, RAYINFO HITINFO);

void EvaluateRay(LINE RAY, bool PROCESSBOUNCE = true) {
	RAYINFO HITINFO = GetRayIntersection(RAY);
	if (HITINFO.HIT) {
		rlVertex2f(RAY.x1, RAY.y1);
		rlVertex2f(RAY.x1 + HITINFO.LEASTMULTIPLIER * (RAY.x2 - RAY.x1), RAY.y1 + HITINFO.LEASTMULTIPLIER * (RAY.y2 - RAY.y1));
		if (HITINFO.HITTYPE == 'E') {
			rlColor4f(1.0f, 0.0f, 0.0f, INNERALPHA);
			rlVertex2f(RAY.x1 + HITINFO.LEASTMULTIPLIER * (RAY.x2 - RAY.x1), RAY.y1 + HITINFO.LEASTMULTIPLIER * (RAY.y2 - RAY.y1));
			rlVertex2f(RAY.x1 + HITINFO.GREATESTMULTIPLIER * (RAY.x2 - RAY.x1), RAY.y1 + HITINFO.GREATESTMULTIPLIER * (RAY.y2 - RAY.y1));
			rlColor4f(0.0f, 0.0f, 1.0f, RAYALPHA);
			
		}else if (HITINFO.HITTYPE == 'W') {
			rlVertex2f(RAY.x1, RAY.y1);
			rlVertex2f(RAY.x1 + HITINFO.LEASTMULTIPLIER * (RAY.x2 - RAY.x1), RAY.y1 + HITINFO.LEASTMULTIPLIER * (RAY.y2 - RAY.y1));
			rlColor4f(0.0f, 0.0f, 1.0f, RAYALPHA);
		}
		if (PROCESSBOUNCE) {
			RAY.x2 = RAY.x1 + HITINFO.LEASTMULTIPLIER * (RAY.x2 - RAY.x1);
			RAY.y2 = RAY.y1 + HITINFO.LEASTMULTIPLIER * (RAY.y2 - RAY.y1);
			BounceRay(RAY, HITINFO);
		}
	}
}


void BounceRay(LINE RAY, RAYINFO HITINFO) {
	float OFFSET = 1.0f;
	float WALLdx = HITINFO.WALLEND.x - HITINFO.WALLSTART.x;
	float WALLdy = HITINFO.WALLEND.y - HITINFO.WALLSTART.y;
	float NORMALdx = -WALLdy;
	float NORMALdy = WALLdx;
	float NORMALLENGTH = std::sqrt(NORMALdx * NORMALdx + NORMALdy * NORMALdy);
	NORMALdx /= NORMALLENGTH;
	NORMALdy /= NORMALLENGTH;
	float RAYdx = RAY.x2 - RAY.x1;
	float RAYdy = RAY.y2 - RAY.y1;
	float DOTPRODUCT = RAYdx * NORMALdx + RAYdy * NORMALdy;
	if (DOTPRODUCT > 0) {
		NORMALdx = -NORMALdx;
		NORMALdy = -NORMALdy;
		DOTPRODUCT = RAYdx * NORMALdx + RAYdy * NORMALdy;
	}
	float BOUNCEdx = RAYdx - 2 * DOTPRODUCT * NORMALdx;
	float BOUNCEdy = RAYdy - 2 * DOTPRODUCT * NORMALdy;
	float BOUNCELENGTH = std::sqrt(BOUNCEdx * BOUNCEdx + BOUNCEdy * BOUNCEdy);
	BOUNCEdx /= BOUNCELENGTH;
	BOUNCEdy /= BOUNCELENGTH;
	LINE BOUNCERAY;
	BOUNCERAY.x1 = RAY.x2;
	BOUNCERAY.y1 = RAY.y2;
	BOUNCERAY.x1 = BOUNCERAY.x1 + (BOUNCEdx * OFFSET);
	BOUNCERAY.y1 = BOUNCERAY.y1 + (BOUNCEdy * OFFSET);
	BOUNCERAY.y2 = BOUNCEdy * RAYEMITTER.RAYLENGTH + BOUNCERAY.y1;
	BOUNCERAY.x2 = BOUNCEdx * RAYEMITTER.RAYLENGTH + BOUNCERAY.x1;
	EvaluateRay(BOUNCERAY, false);
}



void DoRayTracing() {
	rlBegin(RL_LINES);
	rlColor4f(0.0f, 0.0f, 1.0f, RAYALPHA);
	CreateRays();
	for (int RAYINDEX = 0; RAYINDEX < RAYEMITTER.NUMBEROFRAYS; RAYINDEX++) {
		LINE RAY = RAYS[RAYINDEX];
		EvaluateRay(RAY);
	}
	rlEnd();
}





int main() {
	WALLS.insert(WALLS.end(), { {0,0,1280,0},{1280,0,1280,720},{1280,720,0,720},{0,720,0,0} });
	SHAPE.push_back('R');
	WALLS.insert(WALLS.end(), { { NAN, NAN,NAN,NAN }, {100,100,300,100}, { 300,100,300,300 },{300,300,100,300},{100,300,100,100}, {NAN, NAN,NAN,NAN} });
	RAYEMITTER.SetRayLength();
	InitWindow(SCREENWIDTH, SCREENHIEGHT, "RayTracing");


	while (!WindowShouldClose()) {
		BeginDrawing();
		DrawFPS(10, 10);
		ClearBackground(BLACK);

		if (IsKeyPressed(KEY_DOWN)) {
			RAYEMITTER.NUMBEROFRAYS /= 2;
			RAYEMITTER.UpdateRayAngle();
		}
		else if (IsKeyPressed(KEY_UP)) {
			if (RAYEMITTER.NUMBEROFRAYS == 0) RAYEMITTER.NUMBEROFRAYS = 2;
			else RAYEMITTER.NUMBEROFRAYS *= 2;
			RAYEMITTER.UpdateRayAngle();
		}

		DrawRayEmitter();
		DoRayTracing();

		EndDrawing();
	}


	return 0;
}

