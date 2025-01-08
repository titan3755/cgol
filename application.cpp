#include <application.h>

Application::Application(int width, int height, std::string title) {
	this->scrn_width = width;
	this->scrn_height = height;
	this->primary_window_title = title;
	this->window = nullptr;
	this->renderer = nullptr;
	this->is_running = false;
	this->game_running = false;
}

Application::~Application() {
	this->cleanup();
}

void Application::init() {
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
		return;
	}

	if (IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG) {
		std::cerr << "IMG_Init Error: " << IMG_GetError() << std::endl;
		SDL_Quit();
		return;
	}

	if (TTF_Init() != 0) {
		std::cerr << "TTF_Init Error: " << TTF_GetError() << std::endl;
		IMG_Quit();
		SDL_Quit();
		return;
	}

	this->window = SDL_CreateWindow(
		this->primary_window_title.c_str(),
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		this->scrn_width,
		this->scrn_height,
		SDL_WINDOW_SHOWN
	);

	if (this->window == nullptr) {
		std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
		SDL_Quit();
		IMG_Quit();
		TTF_Quit();
		return;
	}

	this->renderer = SDL_CreateRenderer(this->window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

	if (this->renderer == nullptr) {
		std::cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
		SDL_DestroyWindow(this->window);
		SDL_Quit();
		IMG_Quit();
		TTF_Quit();
		return;
	}

	this->is_running = true;

	std::cout << "Game systems initialized successfully!" << std::endl;
}

void Application::run() {
	Render render(this->window, this->renderer, this->scrn_width, this->scrn_height);
	render.setup_grid();
	bool is_mouse_button_down = false;
	int update_counter = 0;
	int update_limit = 50;
	while (this->is_running) {
		std::vector<int> initVec = render.get_grid_internal_data_vec();
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				this->is_running = false;
			}
			if (event.type == SDL_KEYDOWN) {
				switch (event.key.keysym.sym) {
				case SDLK_ESCAPE:
					this->is_running = false;
					break;
				case SDLK_SPACE:
					this->game_running = !this->game_running;
					break;
				case SDLK_r:
					//reset the vector
					render.set_grid_internal_data_vec(std::vector<int>(render.get_grid_internal_data_vec().size(), 0));
					render.update_grid();
					this->game_running = false;
					update_limit = 50;
					update_counter = 0;
					break;
				case SDLK_w:
					update_limit += 5;
					break;
				case SDLK_s:
					update_limit -= 5;
					break;
				default:
					break;
				}
			}
			// mouse event
			// check mouse left click and take the coordinates
			// then update the grid vec accordingly
			if (event.type == SDL_MOUSEBUTTONDOWN) {
				if (event.button.button == SDL_BUTTON_LEFT) {
					if (!this->game_running) {
						is_mouse_button_down = true; // Mouse button is pressed
						int x, y;
						SDL_GetMouseState(&x, &y);

						// Calculate grid coordinates
						int grid_x = x / render.get_grid_cell_size();
						int grid_y = y / render.get_grid_cell_size();

						// Calculate the grid index
						int grid_width = this->scrn_width / render.get_grid_cell_size();
						int grid_height = this->scrn_height / render.get_grid_cell_size();

						// Check bounds to prevent out-of-range access
						if (grid_x >= 0 && grid_x < grid_width && grid_y >= 0 && grid_y < grid_height) {
							int index = grid_y * grid_width + grid_x;
							render.update_grid_data(index, 1); // Update the grid data to mark the cell as alive
						}
						else {
							std::cerr << "Mouse click outside grid bounds!" << std::endl;
						}
					}
				}
			}

			if (event.type == SDL_MOUSEMOTION) {
				if (is_mouse_button_down && !this->game_running) {
					int x, y;
					SDL_GetMouseState(&x, &y);

					// Calculate grid coordinates
					int grid_x = x / render.get_grid_cell_size();
					int grid_y = y / render.get_grid_cell_size();

					// Calculate the grid index
					int grid_width = this->scrn_width / render.get_grid_cell_size();
					int grid_height = this->scrn_height / render.get_grid_cell_size();

					// Check bounds to prevent out-of-range access
					if (grid_x >= 0 && grid_x < grid_width && grid_y >= 0 && grid_y < grid_height) {
						int index = grid_y * grid_width + grid_x;
						render.update_grid_data(index, 1); // Update the grid data to mark the cell as alive
					}
				}
			}

			if (event.type == SDL_MOUSEBUTTONUP) {
				if (event.button.button == SDL_BUTTON_LEFT) {
					is_mouse_button_down = false; // Mouse button is released, stop updating
				}
			}
		}

		// main game functionality here
		if (this->game_running && update_counter >= update_limit) {
			update_counter = 0;
			std::vector<int> newVec = render.get_grid_internal_data_vec();
			std::vector<int> updatedVec = newVec;
			int grid_width = this->scrn_width / render.get_grid_cell_size();
			int grid_height = this->scrn_height / render.get_grid_cell_size();

			for (int i = 0; i < newVec.size(); i++) {
				int x = i % grid_width;
				int y = i / grid_width;
				int alive_count = 0;

				// Check all 8 neighbors
				for (int j = -1; j <= 1; j++) {
					for (int k = -1; k <= 1; k++) {
						if (j == 0 && k == 0) {
							continue;
						}

						int new_x = x + j;
						int new_y = y + k;

						// Check bounds to prevent out-of-range access
						if (new_x >= 0 && new_x < grid_width && new_y >= 0 && new_y < grid_height) {
							int index = new_y * grid_width + new_x;
							if (newVec[index] == 1) {
								alive_count++;
							}
						}
					}
				}

				// Apply the rules of the game
				if (newVec[i] == 1) {
					if (alive_count < 2 || alive_count > 3) {
						updatedVec[i] = 0;
					}
				}
				else {
					if (alive_count == 3) {
						updatedVec[i] = 1;
					}
				}
			}

			render.set_grid_internal_data_vec(updatedVec);
		}

		// update grid
		if (initVec != render.get_grid_internal_data_vec()) {
			render.update_grid();
		}

		SDL_SetRenderDrawColor(this->renderer, 70, 80, 90, 255);
		SDL_RenderClear(this->renderer);

		render.render_grid();

		SDL_RenderPresent(this->renderer);

		update_counter++;
	}

	std::cout << "Game loop exited successfully!" << std::endl;

}

void Application::cleanup() {
	if (this->renderer != nullptr) {
		SDL_DestroyRenderer(this->renderer);
		this->renderer = nullptr;
	}

	if (this->window != nullptr) {
		SDL_DestroyWindow(this->window);
		this->window = nullptr;
	}

	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
}