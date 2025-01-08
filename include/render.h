#pragma once
#ifndef RENDER_H
#define RENDER_H

// std lib includes
#include <iostream>
#include <string>
#include <vector>

// sdl includes
#include <SDL.h>
#undef main
#include <SDL_image.h>
#include <SDL_ttf.h>

// render class and function defs
class Render
{
public:
	Render(SDL_Window* window, SDL_Renderer* renderer, int scrn_width, int scrn_height);
	~Render();

	// render setups for different components
	void setup_grid();

	// update different components
	void update_grid();
	void update_grid_data(int index, int data);

	// misc functions
	//void zoom_grid(float scale_factor);
	//void translate_camera(int delta_x, int delta_y);
	//void reset_camera();

	// cleanup different components
	void cleanup_grid();

	// render different components
	void render_grid();

	// getters
	SDL_Texture* get_grid_texture();
	SDL_Rect get_grid_rect();
	int get_grid_cell_count();
	int get_grid_cell_size();
	SDL_Color get_grid_line_color();
	SDL_Color get_grid_cell_color();
	std::vector<int> get_grid_internal_data_vec();

	// setters
	void set_grid_texture(SDL_Texture* texture);
	void set_grid_rect(SDL_Rect rect);
	void set_grid_cell_count(int count);
	void set_grid_cell_size(int size);
	void set_grid_line_color(SDL_Color color);
	void set_grid_cell_color(SDL_Color color);
	void set_grid_internal_data_vec(std::vector<int> vec);

private:
	SDL_Window* window;
	SDL_Renderer* renderer;
	int scrn_width;
	int scrn_height;
	SDL_Texture* grid_texture;
	SDL_Rect grid_rect;
	int grid_cell_count;
	float grid_cell_size;
	SDL_Color grid_line_color;
	SDL_Color grid_cell_color;
	SDL_Color grid_cell_alive_color;
	std::vector<int> grid_internal_data_vec;
	float scale_factor;
};

#endif // RENDER_H