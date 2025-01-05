#pragma once

#ifndef APPLICATION_H
#define APPLICATION_H

// std lib includes
#include <iostream>
#include <string>

// application includes
#include <render.h>

// sdl includes
#include <SDL.h>
#undef main // undefine main for sdl
#include <SDL_image.h>
#include <SDL_ttf.h>

// application class and function defs
class Application
{
public:
	Application(int width, int height, std::string title);
	~Application();

	void init();
	void run();
	void cleanup();

private:
	int scrn_width;
	int scrn_height;
	std::string primary_window_title;
	SDL_Window* window;
	SDL_Renderer* renderer;
	bool is_running;
};

#endif // APPLICATION_H