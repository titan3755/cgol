#include <render.h>

Render::Render(SDL_Window* window, SDL_Renderer* renderer, int scrn_width, int scrn_height) {
	this->window = window;
	this->renderer = renderer;
	this->scrn_width = scrn_width;
	this->scrn_height = scrn_height;
	this->grid_texture = nullptr;
	this->grid_rect = { 0, 0, 0, 0 };
	this->grid_cell_count = 0;
	this->grid_cell_size = 5;
	this->grid_line_color = { 50, 50, 50, 255 };
	this->grid_cell_color = { 255, 255, 255, 255 };
	this->grid_cell_alive_color = { 0, 200, 20, 255 };
    this->grid_internal_data_vec = std::vector<int>();
}

Render::~Render() {
	this->cleanup_grid();
}

// setup the grid texture for an instance of conway's game of life
// setup the grid lines only accoring to the color and cell size and cell count
void Render::setup_grid() {
    // Validate grid cell size to avoid division by zero or invalid setup
    if (this->grid_cell_size <= 0) {
        std::cerr << "Error: Grid cell size must be greater than zero." << std::endl;
        return;
    }

    // Create an SDL surface to hold the grid
    SDL_Surface* grid_surface = SDL_CreateRGBSurfaceWithFormat(
        0, this->scrn_width, this->scrn_height, 32, SDL_PIXELFORMAT_RGBA32
    );

    if (!grid_surface) {
        std::cerr << "Error creating grid surface: " << SDL_GetError() << std::endl;
        return;
    }

    // Fill the entire surface with the cell background color
    SDL_FillRect(grid_surface, nullptr,
        SDL_MapRGBA(grid_surface->format, this->grid_cell_color.r, this->grid_cell_color.g, this->grid_cell_color.b, this->grid_cell_color.a)
    );

    // Draw vertical grid lines
    for (int x = 0; x < this->scrn_width; x += this->grid_cell_size) {
        SDL_Rect vertical_line = { x, 0, 1, this->scrn_height };
        SDL_FillRect(grid_surface, &vertical_line,
            SDL_MapRGBA(grid_surface->format, this->grid_line_color.r, this->grid_line_color.g, this->grid_line_color.b, this->grid_line_color.a)
        );
    }

    // Draw horizontal grid lines
    for (int y = 0; y < this->scrn_height; y += this->grid_cell_size) {
        SDL_Rect horizontal_line = { 0, y, this->scrn_width, 1 };
        SDL_FillRect(grid_surface, &horizontal_line,
            SDL_MapRGBA(grid_surface->format, this->grid_line_color.r, this->grid_line_color.g, this->grid_line_color.b, this->grid_line_color.a)
        );
    }

    // Create a texture from the surface
    this->grid_texture = SDL_CreateTextureFromSurface(this->renderer, grid_surface);
    if (!this->grid_texture) {
        std::cerr << "Error creating grid texture: " << SDL_GetError() << std::endl;
    }

    // Set the grid rectangle dimensions
    this->grid_rect = { 0, 0, this->scrn_width, this->scrn_height };

    // Calculate the total grid cell count
    this->grid_cell_count = (this->scrn_width / this->grid_cell_size) * (this->scrn_height / this->grid_cell_size);

    // set the grid internal data vec acc. to the cell count
    // the number of elements in the vector will be equal to the cell count - 1
    for (int i = 0; i < this->grid_cell_count; i++) {
        this->grid_internal_data_vec.push_back(0);
    }

    // Free the surface now that we have a texture
    SDL_FreeSurface(grid_surface);

}

// update the grid texture for an instance of conway's game of life
// update the grid cells only according to the cell size and cell count
void Render::update_grid() {
	// implement later
}

// cleanup the grid texture for an instance of conway's game of life
void Render::cleanup_grid() {
	if (this->grid_texture != nullptr) {
		SDL_DestroyTexture(this->grid_texture);
		this->grid_texture = nullptr;
	}
}

// render the grid texture for an instance of conway's game of life
void Render::render_grid() {
	SDL_RenderCopy(this->renderer, this->grid_texture, nullptr, &this->grid_rect);
}

// getters
SDL_Texture* Render::get_grid_texture() {
	return this->grid_texture;
}

SDL_Rect Render::get_grid_rect() {
	return this->grid_rect;
}

int Render::get_grid_cell_count() {
	return this->grid_cell_count;
}

int Render::get_grid_cell_size() {
	return this->grid_cell_size;
}

SDL_Color Render::get_grid_line_color() {
	return this->grid_line_color;
}

SDL_Color Render::get_grid_cell_color() {
	return this->grid_cell_color;
}

// setters

void Render::set_grid_texture(SDL_Texture* texture) {
	this->grid_texture = texture;
}

void Render::set_grid_rect(SDL_Rect rect) {
	this->grid_rect = rect;
}

void Render::set_grid_cell_count(int count) {
	this->grid_cell_count = count;
}

void Render::set_grid_cell_size(int size) {
	this->grid_cell_size = size;
}

void Render::set_grid_line_color(SDL_Color color) {
	this->grid_line_color = color;
}

void Render::set_grid_cell_color(SDL_Color color) {
	this->grid_cell_color = color;
}