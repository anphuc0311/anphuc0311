#include <iostream>
#include <SDL.h>
#include <random>
#include <vector>
#include <chrono>



using namespace std;
using namespace std::chrono;

const int WIDTH = 450;  //450
const int HEIGHT = 800; //800
const int BOXSIZE = 35; //35
random_device rando;

class MyCoord
{
public:
	int x;
	int y;
	MyCoord() {}
	MyCoord(int x, int y) {
		this->x = x;
		this->y = y;
	}
};

class MyShape
{
public:
	MyCoord anchorPoint;
	vector<MyCoord> squareCoordinates;
	vector<MyCoord> squareCoordinatesDefault;
	short r;
	short g;
	short b;

	//MyShape(const MyShape&);

	MyShape(MyCoord anchorPoint, vector<MyCoord> squareCoordinates, short r, short g, short b) {
		this->anchorPoint = anchorPoint;
		this->squareCoordinates = squareCoordinates;
		this->r = r;
		this->g = g;
		this->b = b;
		this->squareCoordinatesDefault = squareCoordinates;
	}
	void resetPiece() {
		squareCoordinates = squareCoordinatesDefault;
	}
	void rotatePiece(bool direction = true) {
		if (direction)
		{
			for (int i = 0; i < squareCoordinates.size(); i++)
			{
				int x = ((0 * squareCoordinates[i].x) + (-1 * squareCoordinates[i].y));
				int y = ((1 * squareCoordinates[i].x) + (0 * squareCoordinates[i].y));
				squareCoordinates[i].x = x;
				squareCoordinates[i].y = y;
				if (anchorPoint.y + y < 0)
				{
					anchorPoint.y++;
				}
			}
		}
		else
		{
			for (int i = 0; i < squareCoordinates.size(); i++)
			{
				int x = ((0 * squareCoordinates[i].x) + (1 * squareCoordinates[i].y));
				int y = ((-1 * squareCoordinates[i].x) + (0 * squareCoordinates[i].y));
				squareCoordinates[i].x = x;
				squareCoordinates[i].y = y;
			}
		}
	}
};

enum Direction
{
	UP,
	DOWN,
	LEFT,
	RIGHT
};



float deltaSpeed();
void resetBoard();
void gameLoop();
void setShapeLocation();
void drawAllShapes(vector<MyShape>);
void drawShape(MyShape);
void drawNextShape(MyShape);
void moveAllShapes(vector<MyShape>&, Direction);
void moveShape(MyShape&, Direction);
bool frameRate(int);
bool collision(MyShape, Direction);
void eraseLine(int);

Uint32* pixels = new Uint32[WIDTH * HEIGHT];
Uint32 pixelsBackground[WIDTH * HEIGHT];
Uint32 pixelLoc[WIDTH * HEIGHT];
Uint32 pixelsBackgroundTemp[WIDTH * HEIGHT];

SDL_PixelFormat* fmt;

bool squareLocation[(WIDTH / BOXSIZE)][(HEIGHT / BOXSIZE)] = {};
bool squareLocationTemp[(WIDTH / BOXSIZE)][(HEIGHT / BOXSIZE)] = {};

__int64 currentDeltaTime, nextDeltaTime;
__int64 currentFrameTime, nextFrameTime;
float delta, speed = 2;
int currentShapeChoice, nextShapeChoice;

vector<MyShape> allShapes =
{
	MyShape(MyCoord((WIDTH / BOXSIZE / 2),1),{ { 0,0 },{ 1,0 },{ 2,0 },{ 3,0 } },   0, 240, 240), // Line
	MyShape(MyCoord((WIDTH / BOXSIZE / 2),1),{ { 0,0 },{ 0,1 },{ 1,0 },{ 1,1 } }, 240, 240,   0), // Square
	MyShape(MyCoord((WIDTH / BOXSIZE / 2),1),{ { 0,0 },{ 1,0 },{ 1,1 },{ 2,1 } }, 240,   0,   0), // ¯-_
	MyShape(MyCoord((WIDTH / BOXSIZE / 2),1),{ { 0,1 },{ 1,1 },{ 1,0 },{ 2,0 } },   0, 240,   0), // _-¯
	MyShape(MyCoord((WIDTH / BOXSIZE / 2),1),{ { 0,1 },{ 1,1 },{ 1,0 },{ 2,1 } }, 160,   0, 240), // _|_
	MyShape(MyCoord((WIDTH / BOXSIZE / 2),1),{ { 0,0 },{ 1,0 },{ 2,0 },{ 2,1 } }, 240, 160,   0), // |¯¯
	MyShape(MyCoord((WIDTH / BOXSIZE / 2),1),{ { 0,0 },{ 1,0 },{ 2,0 },{ 0,1 } },   0,   0, 240)  // ¯¯|
};

int main(int argc, char** argv)
{


	bool leftMouseButtonDown = false;
	bool quit = false;
	SDL_Event event;

	SDL_Init(SDL_INIT_VIDEO);

	SDL_Window* window = SDL_CreateWindow("Game Tetris",
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, 0);
	Uint32 format = SDL_GetWindowPixelFormat(window);
	fmt = SDL_AllocFormat(format);


	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);
	SDL_Texture* texture = SDL_CreateTexture(renderer,
		SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, WIDTH, HEIGHT);
	currentShapeChoice = rando() % 7;
	nextShapeChoice = rando() % 7;
	resetBoard();


	while (!quit)
	{
		bool quit = false;
		SDL_UpdateTexture(texture, NULL, pixels, WIDTH * sizeof(Uint32));

		//SDL_WaitEvent(&event);
		SDL_WaitEventTimeout(&event, 100);
		//SDL_PollEvent(&event);
		switch (event.type)
		{
		case SDL_KEYDOWN:
			if (event.key.keysym.sym == SDLK_ESCAPE)
			{
				quit = true;
			}
			if (event.key.keysym.sym == SDLK_s)
			{
				moveShape(allShapes[currentShapeChoice], DOWN);
			}
			else if (event.key.keysym.sym == SDLK_w)
			{
				moveShape(allShapes[currentShapeChoice], UP);
			}
			if (event.key.keysym.sym == SDLK_d)
			{
				moveShape(allShapes[currentShapeChoice], RIGHT);
			}
			else if (event.key.keysym.sym == SDLK_a)
			{
				moveShape(allShapes[currentShapeChoice], LEFT);
			}

			if (event.key.keysym.sym == SDLK_SPACE)
			{
				if (allShapes[currentShapeChoice].anchorPoint.y >= 1)
				{
					do
					{
						moveShape(allShapes[currentShapeChoice], DOWN);

					} while (!collision(allShapes[currentShapeChoice], DOWN));
					//moveShape(allShapes[currentShapeChoice], UP);
				}
			}
			if (event.key.keysym.sym == SDLK_e)
			{
				allShapes[currentShapeChoice].rotatePiece();
				if (collision(allShapes[currentShapeChoice], RIGHT) || collision(allShapes[currentShapeChoice], DOWN) || collision(allShapes[currentShapeChoice], LEFT))
				{
					allShapes[currentShapeChoice].rotatePiece(false);
				}
			}
			if (event.key.keysym.sym == SDLK_q)
			{
				allShapes[currentShapeChoice].rotatePiece(false);
				if (collision(allShapes[currentShapeChoice], RIGHT) || collision(allShapes[currentShapeChoice], DOWN) || collision(allShapes[currentShapeChoice], LEFT))
				{
					allShapes[currentShapeChoice].rotatePiece();
				}
			}
			break;
		case SDL_QUIT:
			quit = true;
			break;
		case SDL_MOUSEBUTTONUP:
			if (event.button.button == SDL_BUTTON_LEFT)
				leftMouseButtonDown = false;
			break;
		case SDL_MOUSEBUTTONDOWN:
			if (event.button.button == SDL_BUTTON_LEFT)
				leftMouseButtonDown = true;
		case SDL_MOUSEMOTION:
			if (leftMouseButtonDown)
			{
				int mouseX = event.motion.x;
				int mouseY = event.motion.y;
				for (int i = mouseY; i < mouseY + 20; i++)
				{
					for (int j = mouseX; j < mouseX + 20; j++)
					{
						pixels[(i * WIDTH + j)] = SDL_MapRGB(fmt, 0, 0, 0);
					}
				}
			}
			break;
		}


		//pixels = new Uint32[WIDTH * HEIGHT];
		//_memccpy(pixels, pixelsBackground, 0, (WIDTH*HEIGHT));
		for (int i = 0; i < (WIDTH * HEIGHT); i++)
		{
			pixelLoc[i] = pixelsBackground[i];
		}

		pixels = pixelLoc;
		for (int i = 0; i < HEIGHT; i++)
		{
			for (int j = 0; j < WIDTH; j++)
			{
				if (i % BOXSIZE == 0 || j % BOXSIZE == 0)
				{
					pixels[(i * WIDTH + j)] = SDL_MapRGB(fmt, 0, 0, 0);
				}
				else if (j < BOXSIZE || j >(WIDTH / BOXSIZE) * BOXSIZE - BOXSIZE ||
					i > (HEIGHT / BOXSIZE) * BOXSIZE - BOXSIZE || i <= BOXSIZE)
				{
					pixels[(i * WIDTH + j)] = SDL_MapRGB(fmt, 100, 100, 100);
				}
				if (j < BOXSIZE * 2 && i < BOXSIZE)
				{
					pixels[(i * WIDTH + j)] = SDL_MapRGB(fmt, 150, 150, 150);
				}
			}
		}

		gameLoop();

		SDL_RenderClear(renderer);
		SDL_RenderCopy(renderer, texture, NULL, NULL);
		SDL_RenderPresent(renderer);
		if (quit)
		{
			break;
		}
	}

	//delete[] pixels;
	//delete[] pixelsBackground;
	SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(renderer);

	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}

void resetBoard()
{
	pixels = new Uint32[WIDTH * HEIGHT];
	for (int i = 0; i < WIDTH * HEIGHT; i++)
	{
		pixelsBackground[i] = SDL_MapRGB(fmt, 255, 255, 255);
		pixelLoc[i] = SDL_MapRGB(fmt, 255, 255, 255);
	}

	for (int i = 0; i < (HEIGHT / BOXSIZE); i++)
	{
		for (int j = 0; j < (WIDTH / BOXSIZE); j++)
		{
			squareLocation[j][i] = false;
		}
	}
	for (int i = (HEIGHT / BOXSIZE) - 1; i < (HEIGHT / BOXSIZE); i++)
	{
		for (int j = 0; j < (WIDTH / BOXSIZE); j++)
		{
			squareLocation[j][i] = true;
			squareLocation[j][0] = true;
		}
	}
	for (int i = 0; i < (HEIGHT / BOXSIZE); i++)
	{
		squareLocation[0][i] = true;
		squareLocation[(WIDTH / BOXSIZE) - 1][i] = true;
	}

	for (int i = 0; i < (HEIGHT / BOXSIZE); i++)
	{
		for (int j = 0; j < (WIDTH / BOXSIZE); j++)
		{
			cout << squareLocation[j][i];
		}
		cout << endl;
	}
}

float deltaSpeed()
{
	currentDeltaTime = duration_cast<nanoseconds>(system_clock::now().time_since_epoch()).count();
	float delta = (float)((currentDeltaTime - nextDeltaTime) / 1000000000.0f);
	nextDeltaTime = currentDeltaTime;
	return delta;
}

void gameLoop()
{
	drawShape(allShapes[currentShapeChoice]);
	drawNextShape(allShapes[nextShapeChoice]);
	if (frameRate(speed))
	{
		moveShape(allShapes[currentShapeChoice], DOWN);
	}


}

void setShapeLocation()
{
	for (int i = 0; i < allShapes[currentShapeChoice].squareCoordinates.size(); i++)
	{
		int x = allShapes[currentShapeChoice].squareCoordinates[i].x + allShapes[currentShapeChoice].anchorPoint.x;
		int y = allShapes[currentShapeChoice].squareCoordinates[i].y + allShapes[currentShapeChoice].anchorPoint.y;
		squareLocation[x][y] = true;
	}
}

void drawAllShapes(vector<MyShape> shapes) {
	for (int i = 0; i < shapes.size(); i++)
	{
		drawShape(shapes[i]);
	}
}

void drawShape(MyShape shape)
{
	int shapeLenght = shape.squareCoordinates.size();


	for (int i = 0; i < shapeLenght; i++)
	{
		int y = (shape.squareCoordinates[i].y + shape.anchorPoint.y) * BOXSIZE;
		for (int j = y; j < y + BOXSIZE; j++)
		{
			int x = (shape.squareCoordinates[i].x + shape.anchorPoint.x) * BOXSIZE;
			for (int k = x; k < x + BOXSIZE; k++)
			{
				if (j <= HEIGHT && k < WIDTH)
				{
					pixels[(j)*WIDTH + (k)] = SDL_MapRGB(fmt, shape.r, shape.g, shape.b);
				}
			}
		}
	}
}

void drawNextShape(MyShape shape)
{
	MyShape smallShape = MyShape(shape);
	smallShape.resetPiece();
	int shapeLenght = smallShape.squareCoordinates.size();
	smallShape.anchorPoint = { 1,1 };
	int smallBoxSize = BOXSIZE / 3;
	for (int i = 0; i < shapeLenght; i++)
	{
		int y = (smallShape.squareCoordinates[i].y + smallShape.anchorPoint.y) * smallBoxSize;
		for (int j = y; j < y + smallBoxSize; j++)
		{
			int x = (smallShape.squareCoordinates[i].x + smallShape.anchorPoint.x) * smallBoxSize;
			for (int k = x; k < x + smallBoxSize; k++)
			{
				if (j <= HEIGHT && k < WIDTH)
				{
					pixels[(j)*WIDTH + (k)] = SDL_MapRGB(fmt, smallShape.r, smallShape.g, smallShape.b);
				}
			}
		}
	}
}

void moveAllShapes(vector<MyShape>& shapes, Direction direction) {
	for (int i = 0; i < shapes.size(); i++)
	{
		moveShape(shapes[i], direction);
	}
}

void moveShape(MyShape& shape, Direction direction)
{
	cout << "Anchor Y: " << allShapes[currentShapeChoice].anchorPoint.y << ", Square Coord: ";
	for (int i = 0; i < 4; i++)
	{
		cout << allShapes[currentShapeChoice].squareCoordinates[i].y << ", ";
	}
	cout << endl;
	if (collision(allShapes[currentShapeChoice], direction))
	{
		if (direction == DOWN)
		{
			for (int i = 0; i < (WIDTH * HEIGHT); i++)
			{
				pixelsBackground[i] = pixels[i];
			}

			setShapeLocation();
			if (allShapes[currentShapeChoice].anchorPoint.y == 1 && allShapes[currentShapeChoice].anchorPoint.x == (WIDTH / BOXSIZE / 2))
			{
				resetBoard();
			}
			currentShapeChoice = nextShapeChoice;
			nextShapeChoice = rando() % 7;
			allShapes[nextShapeChoice].resetPiece();
			allShapes[currentShapeChoice].anchorPoint.x = (WIDTH / BOXSIZE / 2);
			allShapes[currentShapeChoice].anchorPoint.y = 1;
			allShapes[currentShapeChoice].resetPiece();
			for (int i = 1; i < (HEIGHT / BOXSIZE) - 1; i++)
			{
				bool line = true;
				for (int j = 1; j < (WIDTH / BOXSIZE) - 1; j++)
				{
					if (squareLocation[j][i] == false)
					{
						line = false;
						break;
					}
				}
				if (line)
				{
					cout << "Line" << endl;
					eraseLine(i);
				}
			}
			return;
		}
		else
		{
			cout << "collision L or R" << endl;
			return;
		}
	}
	switch (direction)
	{
	case UP:
		shape.anchorPoint.y--;
		if (shape.anchorPoint.y < 0)
		{
			//shape.anchorPoint.y = HEIGHT / BOXSIZE - 1;
			shape.anchorPoint.y = 0;
		}
		break;
	case DOWN:
		shape.anchorPoint.y++;
		if (shape.anchorPoint.y > HEIGHT / BOXSIZE - 1)
		{
			shape.anchorPoint.y = 0;
		}
		break;
	case LEFT:
		shape.anchorPoint.x--;
		if (shape.anchorPoint.x < 0)
		{
			shape.anchorPoint.x = WIDTH / BOXSIZE - 1;
		}
		break;
	case RIGHT:
		shape.anchorPoint.x++;
		if (shape.anchorPoint.x > WIDTH / BOXSIZE)
		{
			shape.anchorPoint.x = 0;
		}
		break;
	default:
		break;
	}
}

bool frameRate(int rate)
{
	currentFrameTime = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
	if (currentFrameTime >= nextFrameTime)
	{
		nextFrameTime = currentFrameTime + 1000 / rate;
		return true;
	}

	return false;
}

bool collision(MyShape shape, Direction direction) {
	switch (direction)
	{
	case UP:
		for (int i = 0; i < shape.squareCoordinates.size(); i++) {
			int x = shape.squareCoordinates[i].x + shape.anchorPoint.x;
			int y = shape.squareCoordinates[i].y + shape.anchorPoint.y;
			if (squareLocation[x][--y]) {
				cout << "up" << endl;
				return true;
			}
		}
		break;
	case DOWN:
		for (int i = 0; i < shape.squareCoordinates.size(); i++) {
			int x = shape.squareCoordinates[i].x + shape.anchorPoint.x;
			int y = shape.squareCoordinates[i].y + shape.anchorPoint.y;
			if (squareLocation[x][++y]) {
				cout << "down" << endl;
				return true;
			}
		}
		break;
	case LEFT:
		for (int i = 0; i < shape.squareCoordinates.size(); i++) {
			int x = shape.squareCoordinates[i].x + shape.anchorPoint.x;
			int y = shape.squareCoordinates[i].y + shape.anchorPoint.y;
			if (squareLocation[--x][y]) {
				cout << "left" << endl;
				return true;
			}
		}
		break;
	case RIGHT:
		for (int i = 0; i < shape.squareCoordinates.size(); i++) {
			int x = shape.squareCoordinates[i].x + shape.anchorPoint.x;
			int y = shape.squareCoordinates[i].y + shape.anchorPoint.y;
			if (squareLocation[++x][y]) {
				cout << "right" << endl;
				return true;
			}
		}
		break;
	default:
		break;
	}

	return false;
}

void eraseLine(int lineIndex) {

	for (int i = 0; i < WIDTH * HEIGHT; i++)
	{
		pixelsBackgroundTemp[i] = SDL_MapRGB(fmt, 255, 255, 255);
	}

	for (int i = 1; i < (WIDTH / BOXSIZE) - 1; i++)
	{
		squareLocation[i][lineIndex] = false;
		squareLocation[i][0] = false;
	}

	for (int i = (HEIGHT / BOXSIZE) - 1, ii = (HEIGHT / BOXSIZE) - 1; i >= 0; i--, ii--)
	{
		if (i == lineIndex)
		{
			ii--;
		}
		for (int j = (WIDTH / BOXSIZE) - 1; j >= 0; j--)
		{

			if (ii >= 0)
			{
				squareLocationTemp[j][i] = squareLocation[j][ii];
			}

		}
		if (i == 0)
		{
			squareLocationTemp[0][i] = false;
			squareLocationTemp[(WIDTH / BOXSIZE) - 1][i] = false;

			for (int j = 0; j < (WIDTH / BOXSIZE); j++)
			{
				squareLocationTemp[j][i] = true;
			}

		}
	}
	for (int i = 0; i < (HEIGHT / BOXSIZE); i++)
	{
		for (int j = 0; j < (WIDTH / BOXSIZE); j++)
		{
			squareLocation[j][i] = squareLocationTemp[j][i];
		}
	}

	int liner = (BOXSIZE * lineIndex);

	for (int i = HEIGHT - 1, ii = HEIGHT - 1; i >= 0; i--, ii--)
	{
		if (i == liner + BOXSIZE)
		{
			ii -= BOXSIZE;
		}
		for (int j = WIDTH - 1; j >= 0; j--)
		{

			if (ii >= BOXSIZE)
			{
				pixelsBackgroundTemp[(i * WIDTH + j)] = pixelsBackground[(ii * WIDTH + j)];
			}

		}
	}

	for (int i = 0; i < (WIDTH * HEIGHT); i++)
	{
		pixelsBackground[i] = pixelsBackgroundTemp[i];
	}

	for (int i = 0; i < (HEIGHT / BOXSIZE); i++)
	{
		for (int j = 0; j < (WIDTH / BOXSIZE); j++)
		{
			cout << squareLocation[j][i];
		}
		cout << endl;
	}
}