#include <render.h>

Render::Render(SDL_Window* window, SDL_Renderer* renderer, int scrn_width, int scrn_height) {
	this->window = window;
	this->renderer = renderer;
	this->scrn_width = scrn_width;
	this->scrn_height = scrn_height;
	this->grid_texture = nullptr;
	this->grid_rect = { 0, 0, 0, 0 };
	this->grid_cell_count = 0;
	this->grid_cell_size = 25;
	this->grid_line_color = { 50, 50, 50, 255 };
	this->grid_cell_color = { 255, 255, 255, 255 };
	this->grid_cell_alive_color = { 0, 200, 20, 255 };
    this->grid_internal_data_vec = std::vector<int>();
	this->scale_factor = 1.0f;
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

    // Create a streaming texture
    if (this->grid_texture != nullptr) {
        SDL_DestroyTexture(this->grid_texture);
    }
    this->grid_texture = SDL_CreateTexture(
        this->renderer,
        SDL_PIXELFORMAT_RGBA32,
        SDL_TEXTUREACCESS_STREAMING,
        this->scrn_width,
        this->scrn_height
    );

    if (!this->grid_texture) {
        std::cerr << "Error creating streaming texture: " << SDL_GetError() << std::endl;
        SDL_FreeSurface(grid_surface);
        return;
    }

    // Lock the streaming texture
    void* pixels = nullptr;
    int pitch = 0;
    if (SDL_LockTexture(this->grid_texture, nullptr, &pixels, &pitch) != 0) {
        std::cerr << "Error locking texture: " << SDL_GetError() << std::endl;
        SDL_FreeSurface(grid_surface);
        return;
    }

    // Copy surface pixels to the streaming texture
    memcpy(pixels, grid_surface->pixels, grid_surface->h * pitch);

    // Unlock the texture
    SDL_UnlockTexture(this->grid_texture);

    // Set the grid rectangle dimensions
    this->grid_rect = { 0, 0, this->scrn_width, this->scrn_height };

    // Calculate the total grid cell count
    this->grid_cell_count = (this->scrn_width / this->grid_cell_size) * (this->scrn_height / this->grid_cell_size);

    // Initialize the grid internal data vector
    this->grid_internal_data_vec.assign(this->grid_cell_count, 0);

    // Free the surface now that the texture has been created
    SDL_FreeSurface(grid_surface);
}

// update the grid texture for an instance of conway's game of life
void Render::update_grid() {
    // create an entirely new textxure updated with current data in grid vector
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

	// Ensure that grid_cell_size is an integer type
	for (int i = 0; i < this->grid_internal_data_vec.size(); i++) {
		if (this->grid_internal_data_vec[i] == 1) {
			// Ensure this->scrn_width and grid_cell_size are integers for calculations
			int grid_width_in_cells = this->scrn_width / static_cast<int>(this->grid_cell_size);
			int x = (i % grid_width_in_cells) * static_cast<int>(this->grid_cell_size);
			int y = (i / grid_width_in_cells) * static_cast<int>(this->grid_cell_size);

			// Define the cell rectangle to draw
			SDL_Rect cell_rect = { x, y, static_cast<int>(this->grid_cell_size), static_cast<int>(this->grid_cell_size) };

			// Fill the cell if it's alive
			SDL_FillRect(grid_surface, &cell_rect,
				SDL_MapRGBA(grid_surface->format,
					this->grid_cell_alive_color.r,
					this->grid_cell_alive_color.g,
					this->grid_cell_alive_color.b,
					this->grid_cell_alive_color.a
				)
			);
		}
	}

	// Lock the streaming texture
	void* pixels = nullptr;
	int pitch = 0;
	if (SDL_LockTexture(this->grid_texture, nullptr, &pixels, &pitch) != 0) {
		std::cerr << "Error locking texture: " << SDL_GetError() << std::endl;
		SDL_FreeSurface(grid_surface);
		return;
	}

	// Copy surface pixels to the streaming texture
	memcpy(pixels, grid_surface->pixels, grid_surface->h * pitch);

	// Unlock the texture
	SDL_UnlockTexture(this->grid_texture);

	// Free the surface now that the texture has been created
	SDL_FreeSurface(grid_surface);
}

void Render::update_grid_data(int index, int data) {
    // update the grid vector
	// first check if the given index exists in the grid vector
	if (index < 0 || index >= this->grid_internal_data_vec.size()) {
		std::cerr << "Error: Invalid index for grid data update." << std::endl;
		return;
	}
	// update the grid vector
	this->grid_internal_data_vec[index] = data;
}

//// zoom the grid texture for an instance of conway's game of life
//void Render::zoom_grid(float scale_factor_l) {
//    if (scale_factor_l <= 0.1f) {
//        std::cerr << "Scale factor too small, zoom aborted." << std::endl;
//        return;
//    }
//
//    // Update the grid cell size
//    if (scale_factor_l >= 1.0f) {
//        this->grid_cell_size = this->grid_cell_size * scale_factor_l;
//	}
//	else if (scale_factor_l < 1.0f && scale_factor_l > 0.0f) {
//		this->grid_cell_size = this->grid_cell_size * scale_factor_l;
//	}
//
//    if (this->grid_cell_size < 5) this->grid_cell_size = 5; // Prevent zero or negative sizes
//
//    // Recalculate the grid dimensions
//    this->grid_rect.w = this->grid_cell_count * this->grid_cell_size;
//    this->grid_rect.h = this->grid_cell_count * this->grid_cell_size;
//
//    // Recreate the grid texture based on new cell size
//    this->update_grid();
//}
//
//// translate the grid texture for an instance of conway's game of life
//void Render::translate_camera(int delta_x, int delta_y) {
//    this->grid_rect.x += delta_x;
//    this->grid_rect.y += delta_y;
//
//	// Prevent the grid from moving off-screen
//	if (this->grid_rect.x < 0) this->grid_rect.x = 0;
//	if (this->grid_rect.y < 0) this->grid_rect.y = 0;
//	if (this->grid_rect.x > this->scrn_width - this->grid_rect.w) this->grid_rect.x = this->scrn_width - this->grid_rect.w;
//	if (this->grid_rect.y > this->scrn_height - this->grid_rect.h) this->grid_rect.y = this->scrn_height - this->grid_rect.h;
//
//	// Recreate the grid texture based on new position
//    this->update_grid();
//}
//
//// reset the grid texture for an instance of conway's game of life
//void Render::reset_camera() {
//	this->grid_rect.x = 0;
//	this->grid_rect.y = 0;
//
//	// Recreate the grid texture based on new position
//	this->update_grid();
//}

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

std::vector<int> Render::get_grid_internal_data_vec() {
	return this->grid_internal_data_vec;
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

void Render::set_grid_internal_data_vec(std::vector<int> vec) {
	this->grid_internal_data_vec = vec;
}