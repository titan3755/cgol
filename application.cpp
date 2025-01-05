#include <application.h>

Application::Application(int width, int height, std::string title) {
	this->scrn_width = width;
	this->scrn_height = height;
	this->primary_window_title = title;
	this->window = nullptr;
	this->renderer = nullptr;
	this->is_running = false;
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
	while (this->is_running) {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				this->is_running = false;
			}
		}

		SDL_SetRenderDrawColor(this->renderer, 70, 80, 90, 255);
		SDL_RenderClear(this->renderer);

		render.render_grid();

		SDL_RenderPresent(this->renderer);
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