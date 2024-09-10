#include <SDL.h>
#include <vector>
#include <algorithm>
#include <deque>

#undef main

#define WINDOW_HEIGH 720
#define WINDOW_WIDTH 1280
#define FOOD_LENGTH_MULTIPLIER 10

enum GAME_DIFFICULTY {
	EASY = 100,
	MEDIUM = 50,
	HARD = 25,
	INSANE = 10
};

enum SNAKE_DIRECTION {
	UP,
	DOWN,
	LEFT,
	RIGHT
};

void placeFoods(std::vector<SDL_Rect>& foods);
void handlingDirection(SDL_Event& e, SNAKE_DIRECTION& direction);
void handlingMove(SNAKE_DIRECTION direction, SDL_Rect& snakeHead);
void handlingEat(int& size, SDL_Rect& snakeHead, std::vector<SDL_Rect>& foods);
void handlingCollision(SDL_Rect& snakeHead, std::deque<SDL_Rect> bodyQueue);
void handlingOutOfMap(SDL_Rect& snakeHead, int& size, std::deque<SDL_Rect>& bodyQueue);

GAME_DIFFICULTY difficulty = MEDIUM;
SNAKE_DIRECTION direction = UP;
bool running = true;

SDL_Rect snakeHead{ WINDOW_HEIGH / 2, WINDOW_WIDTH / 2, 10, 10 };

// Snake body
std::deque<SDL_Rect> bodyQueue;
int size = 1;

// Food
std::vector<SDL_Rect> foods;

int main() {
	// Base components
	SDL_Init(SDL_INIT_EVERYTHING);
	auto window = SDL_CreateWindow("Snake", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGH, 0);
	auto renderer = SDL_CreateRenderer(window, -1, 0);
	SDL_Event e;

	placeFoods(foods);

	while (running) {
		// Event handling
		handlingDirection(e, direction);
		handlingMove(direction, snakeHead);
		handlingEat(size, snakeHead, foods);
		handlingCollision(snakeHead, bodyQueue);
		handlingOutOfMap(snakeHead, size, bodyQueue);

		bodyQueue.push_front(snakeHead);
		

		while (bodyQueue.size() > size) bodyQueue.pop_back();

		#ifdef _DEBUG
			printf("Size: %d, Head[x: %d, y: %d]\n", size, snakeHead.x, snakeHead.y);
		#endif // DEBUG

		

		// Clear window
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);

		// Draw snake
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

		std::for_each(bodyQueue.begin(), bodyQueue.end(), [&](auto& segment) {
			SDL_RenderFillRect(renderer, &segment);
		});
		SDL_RenderFillRect(renderer, &snakeHead);

		// Draw Food
		SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);

		std::for_each(foods.begin(), foods.end(), [&](auto& food) {
			SDL_RenderFillRect(renderer, &food);
			});

		SDL_RenderPresent(renderer);
		SDL_Delay(difficulty);
	}

	return 0;
}

void placeFoods(std::vector<SDL_Rect>& foods) {
	for (int i = 0; i < 10; i++) {
		SDL_Rect toAppend{ rand() % WINDOW_WIDTH / 10 * 10, rand() % WINDOW_HEIGH / 10 * 10, 10, 10 };
		foods.emplace_back(toAppend);
	}
}

void handlingDirection(SDL_Event& e, SNAKE_DIRECTION& direction) {
	while (SDL_PollEvent(&e)) {
		if (e.type == SDL_QUIT) running = false;
		if (e.type == SDL_KEYDOWN) {
			if (e.key.keysym.sym == SDLK_DOWN) {
				if (direction != UP)
					direction = DOWN;
			}
			if (e.key.keysym.sym == SDLK_UP) {
				if (direction != DOWN)
					direction = UP;
			}
			if (e.key.keysym.sym == SDLK_LEFT) {
				if (direction != RIGHT)
					direction = LEFT;
			}
			if (e.key.keysym.sym == SDLK_RIGHT) {
				if (direction != LEFT)
					direction = RIGHT;
			}
		}
	}
}

void handlingMove(SNAKE_DIRECTION direction, SDL_Rect& snakeHead) {
	switch (direction)
	{
		case UP:
			snakeHead.y -= 10;
			break;
		case DOWN:
			snakeHead.y += 10;
			break;
		case LEFT:
			snakeHead.x -= 10;
			break;
		case RIGHT:
			snakeHead.x += 10;
			break;
		default:
			break;
	}
}

void handlingEat(int& size, SDL_Rect& snakeHead, std::vector<SDL_Rect>& foods) {
	std::for_each(foods.begin(), foods.end(), [&](auto& food) {
		if (snakeHead.x == food.x && snakeHead.y == food.y) {
			size += FOOD_LENGTH_MULTIPLIER;
			food.x = (int)(rand() % WINDOW_WIDTH) / 10 * 10;
			food.y = (int)(rand() % WINDOW_HEIGH) /10 * 10;


		}
	});
}

void handlingCollision(SDL_Rect& snakeHead, std::deque<SDL_Rect> bodyQueue) {
	// Body Collision
	std::for_each(bodyQueue.begin(), bodyQueue.end(), [&](auto& segment) {
		if (snakeHead.x == segment.x && snakeHead.y == segment.y) {
			size = 1;
		}
	});
}

void handlingOutOfMap(SDL_Rect& snakeHead, int& size, std::deque<SDL_Rect>& bodyQueue) {
	int x = snakeHead.x, y = snakeHead.y;
	if (x > WINDOW_WIDTH || y > WINDOW_HEIGH || x < 0 || y < 0) {
		size = 1;
		bodyQueue.clear();

		snakeHead.x = WINDOW_WIDTH / 2;
		snakeHead.y = WINDOW_HEIGH / 2;
	}
}