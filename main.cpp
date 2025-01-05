#include <iostream>
#include <cstdlib>
#include <ctime>
#include <application.h>

constexpr int SCREEN_WIDTH = 1280;
constexpr int SCREEN_HEIGHT = 720;
constexpr auto WINDOW_TITLE = "Conway's Game of Life";

int main(int argc, char* argv[]) {
	std::srand(std::time(nullptr));
	Application app(SCREEN_WIDTH, SCREEN_HEIGHT, WINDOW_TITLE);
	app.init();
	app.run();
	// app.cleanup();
	// auto cleanup is called when app goes out of scope
	return 0;
}