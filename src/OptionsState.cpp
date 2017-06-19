#include "OptionsState.h"
#include "MenuState.h"
#include "InputManager.h"
#include "Game.h"
#include "Config.h"

#define FONT_X 640
#define FONT_Y 680
#define TEXT_OFFSET 8
#define TEXT_HEIGHT 30
#define OPTION_OFFSET 70
#define BACK_BUTTON 2

#define DARK_GREY { 80, 80, 80, 1 }
#define DARK_GREEN { 55, 74, 38, 1 }
#define LIGHT_GREEN { 181, 201, 60, 1 }
#define WHITE { 255, 255, 255, 255 }

OptionsState::OptionsState(){
	current_option = 0;
	on_submenu = false;

	background = Sprite("menu/background.jpg");

	title = new Text("font/8-BIT WONDER.ttf", 50, Text::TextStyle::SOLID, "OPTIONS", WHITE, FONT_X, 100);

	build_options();

	for(unsigned i = 0; i < options.size(); ++i){
		current_sub_option.push_back(get_current_sub_option(i));
	}
}

void OptionsState::update(float){
	InputManager * input_manager = InputManager::get_instance();

	// inputs
	if(input_manager->quit_requested() || input_manager->key_press(SDLK_ESCAPE) || input_manager->joystick_button_press(InputManager::B, 0)){
		if(on_submenu){
			on_submenu = false;
			for(unsigned i = 0; i < options.size(); ++i){
				current_sub_option[i] = get_current_sub_option(i);
			}
		}
		else{
			m_quit_requested = true;
			Game::get_instance().push(new MenuState(true));
			return;
		}
	}

	if(input_manager->key_press(SDLK_UP)  || input_manager->joystick_button_press(InputManager::UP, 0)){
		if(not on_submenu){
			if(current_option != 0){
				current_option--;
			}
		}
		else{
			if(current_sub_option[current_option] != 0){
				current_sub_option[current_option]--;
			}
		}
	}

	if(input_manager->key_press(SDLK_DOWN) || input_manager->joystick_button_press(InputManager::DOWN, 0)){
		if(not on_submenu){
			if(current_option != (int)options.size() - 1){
				current_option++;
			}
		}
		else{
			string text = options[current_option]->get_text();
			if(current_sub_option[current_option] != (int)sub_options[text].size() - 1){
				current_sub_option[current_option]++;
			}
		}
	}

	if(input_manager->key_press(SDLK_RETURN) || input_manager->joystick_button_press(InputManager::START, 0) || input_manager->joystick_button_press(InputManager::A, 0)) {
		if(not on_submenu){
			if(current_option == 2){ // back button
				m_quit_requested = true;
				Game::get_instance().push(new MenuState(true));
				return;
			}
			else{
				on_submenu = true;
				current_sub_option[current_option] = get_current_sub_option(current_option);
			}
		}
		else{
			if(current_option == 0){ // screen resolution
				int new_width = 0, new_height = 0;
				switch(current_sub_option[current_option]){
					case 0:
						new_width = 800;
						new_height = 600;
						break;
					case 1:
						new_width = 1024;
						new_height = 768;
						break;
					case 2:
						new_width = 1280;
						new_height = 720;
						break;
					case 3:
						new_width = 1366;
						new_height = 768;
						break;
					case 4:
						new_width = 1920;
						new_height = 1080;
						break;
				}

				Game::get_instance().change_resolution(new_width, new_height);
			}
			else if(current_option == 1){ // fullscreen
				bool fullscreen = (current_sub_option[current_option] == 0 ? false : true);
				Game::get_instance().set_fullscreen(fullscreen);
			}
		}
	}

	// positions
	for(int i=0; i<(int)options.size(); i++){
		Text* cur_text = options[i];

		int prev_text_size = (i ? sub_options[options[i-1]->get_text()].size() : 1);
		int prev_text_height = (TEXT_HEIGHT + TEXT_OFFSET * 2) * prev_text_size;

		int text_x = 250;
		int text_y = (i ? options[i-1]->get_y() + prev_text_height : 200);

		cur_text->set_pos(text_x, text_y, false, false);

		for(int j=0; j<(int)sub_options[cur_text->get_text()].size(); j++){
			int x = 800;
			int y = text_y;

			if(j){
				Text* prev_option = sub_options[cur_text->get_text()][j-1];
				y = prev_option->get_y() + prev_option->get_height() + TEXT_OFFSET;
			}

			Text* option = sub_options[cur_text->get_text()][j];
			option->set_pos(x, y);
		}
	}

	// set back button position
	options.back()->set_pos(FONT_X, FONT_Y, true, true);
}

void OptionsState::render(){
	background.render(0, 0);

	title->render();

	for(int i=0; i<(int)options.size(); i++){
		if(on_submenu && i != BACK_BUTTON && i != current_option){
			options[i]->set_color(DARK_GREY);
		}
		else{
			if(current_option == i)
				options[i]->set_color(LIGHT_GREEN);
			else
				options[i]->set_color(WHITE);
		}

		options[i]->render();

		string text = options[i]->get_text();
		for(int j=0; j<(int)sub_options[text].size(); j++){
			// TODO get option selected from db
			if(on_submenu && current_option == i){
				if(current_sub_option[i] == j)
					sub_options[text][j]->set_color(LIGHT_GREEN);
				else
					sub_options[text][j]->set_color(WHITE);
			}
			else{
				if(current_sub_option[i] == j)
					sub_options[text][j]->set_color(DARK_GREEN);
				else
					sub_options[text][j]->set_color(DARK_GREY);
			}

			sub_options[text][j]->render();
		}
	}
}

void OptionsState::build_options(){
	options.push_back(new Text("font/8-BIT WONDER.ttf", 30, Text::TextStyle::SOLID, "SCREEN RESOLUTION", WHITE, 100, 200));
	options.back()->set_pos(100, 200, false, false);
	options.push_back(new Text("font/8-BIT WONDER.ttf", 30, Text::TextStyle::SOLID, "FULLSCREEN", WHITE));
	options.push_back(new Text("font/8-BIT WONDER.ttf", 30, Text::TextStyle::SOLID, "BACK", WHITE));

	sub_options["SCREEN RESOLUTION"].push_back(new Text("font/8-BIT WONDER.ttf", 30, Text::TextStyle::SOLID, "800 x 600", WHITE));
	sub_options["SCREEN RESOLUTION"].push_back(new Text("font/8-BIT WONDER.ttf", 30, Text::TextStyle::SOLID, "1024 x 768", WHITE));
	sub_options["SCREEN RESOLUTION"].push_back(new Text("font/8-BIT WONDER.ttf", 30, Text::TextStyle::SOLID, "1280 x 720", WHITE));
	sub_options["SCREEN RESOLUTION"].push_back(new Text("font/8-BIT WONDER.ttf", 30, Text::TextStyle::SOLID, "1366 x 768", WHITE));
	sub_options["SCREEN RESOLUTION"].push_back(new Text("font/8-BIT WONDER.ttf", 30, Text::TextStyle::SOLID, "1920 x 1080", WHITE));

	sub_options["FULLSCREEN"].push_back(new Text("font/8-BIT WONDER.ttf", 30, Text::TextStyle::SOLID, "OFF", WHITE));
	sub_options["FULLSCREEN"].push_back(new Text("font/8-BIT WONDER.ttf", 30, Text::TextStyle::SOLID, "ON", WHITE));
}

void OptionsState::pause(){

}

void OptionsState::resume(){

}

int OptionsState::get_current_sub_option(int option){
	if(option == 0){ //screen resolution
		int width = Config::get_width();
		int height = Config::get_height();
		string resolution = std::to_string(width) + " x " + std::to_string(height);
		int sub_option = 0;
		for(auto text : sub_options["SCREEN RESOLUTION"]){
			if(text->get_text() == resolution) return sub_option;
			sub_option++;
		}
		return 0;
	}else{ //fullscreen
		return Config::is_fullscreen();
	}
}