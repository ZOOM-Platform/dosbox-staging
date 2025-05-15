#include "gui_imgui.h"
#include "gui_imgui_res.h"
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_timer.h>
#include <thread>
#include <cpu.h>
#include <mixer.h>
#include <control.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

enum PillarBoxArt
{
	RATS,
	WOOD,
	STONE,
	NONE
};

PillarBoxArt pillarboxArtStyle = PillarBoxArt(-1);

SDL_GameController * gameController;

unsigned char scannedPixels[1][1][3];

float scaling; // Render scaling
float dpiScaling; // Windows DPI scaling

GLuint arrowTitleTexture = 0;
GLuint arrowPausedTexture = 0;
GLuint pillarboxArtLeftTexture = 0;
GLuint pillarboxArtRightTexture = 0;
GLuint weapon1SelectedTexture = 0;
GLuint weapon1DeselectedTexture = 0;
GLuint weapon2SelectedTexture = 0;
GLuint weapon2DeselectedTexture = 0;
GLuint weapon3SelectedTexture = 0;
GLuint weapon3DeselectedTexture = 0;
GLuint weapon4SelectedTexture = 0;
GLuint weapon4DeselectedTexture = 0;
GLuint weapon5SelectedTexture = 0;
GLuint weapon5DeselectedTexture = 0;
GLuint weapon6SelectedTexture = 0;
GLuint weapon6DeselectedTexture = 0;
GLuint weapon7SelectedTexture = 0;
GLuint weapon7DeselectedTexture = 0;
GLuint radialkiBackgroundTexture = 0;
GLuint crosshairTexture = 0;
GLint oglViewPort[4];
int32_t _default_CPU_CycleMax = CPU_CycleMax;
int arrowPlacement = 1;
int selectedWeapon = -1;
int renderedSelection = -1;
int mouseReleaseDelayConfig = 200;
int radialkiScrollAutoHideDelay = 500;
int pillarboxArtWidth = -1;
int pillarboxArtHeight = -1;

bool inPauseMenu = false;
bool inSaveLoadMenu = false;
bool inGame = false;
bool radialkiSlowdownConfig = true;
bool enableRadialki = true;
bool radialkiScrollSelection = true;
bool swapControllerButtonsConfig = false;
bool showRadialki = false;
bool titleScreenLoaded = false;
bool titleScreenFaded = false;
bool lMouseIsHeld = false;
bool rMouseIsHeld = false;
bool gameIsSlowedDown = false;
bool leftAltHeld = false;
bool showCrosshair = false;
bool hudShown = false;

SDL_Window * sdl_window;

SDL_Scancode autosprintScancode1 = SDL_SCANCODE_CAPSLOCK;
SDL_Scancode autosprintScancode2 = SDL_SCANCODE_CAPSLOCK;
SDL_Scancode autosprintScancode3 = SDL_SCANCODE_CAPSLOCK;
SDL_Scancode autosprintScancode4 = SDL_SCANCODE_CAPSLOCK;
int autosprintButton1 = 0;
int autosprintButton2 = 0;
int autosprintButton3 = 0;
int autosprintButton4 = 0;
int radialkiButton1 = 0;
int radialkiButton2 = 0;
int radialkiButton3 = 0;
int radialkiButton4 = 0;
int radialkiButton1Index = 0;
int radialkiButton2Index = 0;
int radialkiButton3Index = 0;
int radialkiButton4Index = 0;
int autosprintButton1Index = 0;
int autosprintButton2Index = 0;
int autosprintButton3Index = 0;
int autosprintButton4Index = 0;
SDL_Scancode sprintScancode = SDL_SCANCODE_LSHIFT;

std::chrono::steady_clock::time_point lastScroll = std::chrono::steady_clock::now();

// Initialize ImGui, including textures
void InitImGui(SDL_Window * window, SDL_GLContext context)
{
	sdl_window = window;

	// Read config values
	const auto conf = control->GetSection("radiaki");
	if (conf)
	{
		const auto section = static_cast<Section_prop *>(conf);
		if (section)
		{
			enableRadialki = section->Get_bool("enable_radialki");
			radialkiSlowdownConfig = section->Get_bool("slowdown_on_radialki_open");
			showCrosshair = section->Get_bool("show_crosshair");
			swapControllerButtonsConfig = section->Get_bool("swap_controller_navigation_buttons");
			radialkiScrollSelection = section->Get_bool("use_radialki_with_scrollwheel");
			radialkiScrollAutoHideDelay = section->Get_int("radialki_autohide_delay_after_scrolling");
			std::string pillarboxArtStyleString = section->Get_string("pillarbox_art_style");
			if (pillarboxArtStyleString == "rats")
			{
				pillarboxArtStyle = RATS;
			}
			else if (pillarboxArtStyleString == "wood")
			{
				pillarboxArtStyle = WOOD;
			}
			else if (pillarboxArtStyleString == "stone")
			{
				pillarboxArtStyle = STONE;
			}
			else if (pillarboxArtStyleString == "none")
			{
				pillarboxArtStyle = NONE;
			}
			autosprintScancode1 = (SDL_Scancode)section->Get_int("autosprint_key_scancode_1");
			autosprintScancode2 = (SDL_Scancode)section->Get_int("autosprint_key_scancode_2");
			autosprintScancode3 = (SDL_Scancode)section->Get_int("autosprint_key_scancode_3");
			autosprintScancode4 = (SDL_Scancode)section->Get_int("autosprint_key_scancode_4");
			autosprintButton1 = (SDL_GameControllerButton)section->Get_int("autosprint_joystick_button_1");
			autosprintButton2 = (SDL_GameControllerButton)section->Get_int("autosprint_joystick_button_2");
			autosprintButton3 = (SDL_GameControllerButton)section->Get_int("autosprint_joystick_button_3");
			autosprintButton4 = (SDL_GameControllerButton)section->Get_int("autosprint_joystick_button_4");
			radialkiButton1 = (SDL_GameControllerButton)section->Get_int("radialki_joystick_button_1");
			radialkiButton2 = (SDL_GameControllerButton)section->Get_int("radialki_joystick_button_2");
			radialkiButton3 = (SDL_GameControllerButton)section->Get_int("radialki_joystick_button_3");
			radialkiButton4 = (SDL_GameControllerButton)section->Get_int("radialki_joystick_button_4");
			radialkiButton1Index = (SDL_GameControllerButton)section->Get_int("radialki_joystick_button_1_index");
			radialkiButton2Index = (SDL_GameControllerButton)section->Get_int("radialki_joystick_button_2_index");
			radialkiButton3Index = (SDL_GameControllerButton)section->Get_int("radialki_joystick_button_3_index");
			radialkiButton4Index = (SDL_GameControllerButton)section->Get_int("radialki_joystick_button_4_index");
			autosprintButton1Index = (SDL_GameControllerButton)section->Get_int("autosprint_joystick_button_1_index");
			autosprintButton2Index = (SDL_GameControllerButton)section->Get_int("autosprint_joystick_button_2_index");
			autosprintButton3Index = (SDL_GameControllerButton)section->Get_int("autosprint_joystick_button_3_index");
			autosprintButton4Index = (SDL_GameControllerButton)section->Get_int("autosprint_joystick_button_4_index");
			sprintScancode = (SDL_Scancode)section->Get_int("sprint_key_scancode");
		}
	}

	// Set random pillarbox by default
	if (pillarboxArtStyle < 0)
	{
		std::srand(static_cast<unsigned int>(std::time(0)));
		pillarboxArtStyle = static_cast<PillarBoxArt>(std::rand() % 3);
	}

	// Init ImGui and textures
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO & io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
	io.IniFilename = io.LogFilename = NULL;
	ImGui_ImplSDL2_InitForOpenGL(window, context);
	ImGui_ImplOpenGL3_Init();

	ImGuiLoadTexture(&crosshairTexture, CROSSHAIR_WIDTH, CROSSHAIR_HEIGHT, crosshair_pixels);
	ImGuiLoadTexture(&arrowTitleTexture, ARROW_WIDTH, ARROW_HEIGHT, arrow_pixels_title);
	ImGuiLoadTexture(&arrowPausedTexture, ARROW_WIDTH, ARROW_HEIGHT, arrow_pixels_paused);
	ImGuiLoadTexture(&weapon1SelectedTexture, RADIALKI_SLOT_SIZE, RADIALKI_SLOT_SIZE, weapon_slot1_s_pixels);
	ImGuiLoadTexture(&weapon1DeselectedTexture, RADIALKI_SLOT_SIZE, RADIALKI_SLOT_SIZE, weapon_slot1_d_pixels);
	ImGuiLoadTexture(&weapon2SelectedTexture, RADIALKI_SLOT_SIZE, RADIALKI_SLOT_SIZE, weapon_slot2_s_pixels);
	ImGuiLoadTexture(&weapon2DeselectedTexture, RADIALKI_SLOT_SIZE, RADIALKI_SLOT_SIZE, weapon_slot2_d_pixels);
	ImGuiLoadTexture(&weapon3SelectedTexture, RADIALKI_SLOT_SIZE, RADIALKI_SLOT_SIZE, weapon_slot3_s_pixels);
	ImGuiLoadTexture(&weapon3DeselectedTexture, RADIALKI_SLOT_SIZE, RADIALKI_SLOT_SIZE, weapon_slot3_d_pixels);
	ImGuiLoadTexture(&weapon4SelectedTexture, RADIALKI_SLOT_SIZE, RADIALKI_SLOT_SIZE, weapon_slot4_s_pixels);
	ImGuiLoadTexture(&weapon4DeselectedTexture, RADIALKI_SLOT_SIZE, RADIALKI_SLOT_SIZE, weapon_slot4_d_pixels);
	ImGuiLoadTexture(&weapon5SelectedTexture, RADIALKI_SLOT_SIZE, RADIALKI_SLOT_SIZE, weapon_slot5_s_pixels);
	ImGuiLoadTexture(&weapon5DeselectedTexture, RADIALKI_SLOT_SIZE, RADIALKI_SLOT_SIZE, weapon_slot5_d_pixels);
	ImGuiLoadTexture(&weapon6SelectedTexture, RADIALKI_SLOT_SIZE, RADIALKI_SLOT_SIZE, weapon_slot6_s_pixels);
	ImGuiLoadTexture(&weapon6DeselectedTexture, RADIALKI_SLOT_SIZE, RADIALKI_SLOT_SIZE, weapon_slot6_d_pixels);
	ImGuiLoadTexture(&weapon7SelectedTexture, RADIALKI_SLOT_SIZE, RADIALKI_SLOT_SIZE, weapon_slot7_s_pixels);
	ImGuiLoadTexture(&weapon7DeselectedTexture, RADIALKI_SLOT_SIZE, RADIALKI_SLOT_SIZE, weapon_slot7_d_pixels);
	ImGuiLoadTexture(&radialkiBackgroundTexture, RADIALKI_BG_SIZE, RADIALKI_BG_SIZE, radialki_bg_pixels);

	std::string pillarboxArtImageL = "";
	std::string pillarboxArtImageR = "";
	switch (pillarboxArtStyle)
	{
		case RATS:
			pillarboxArtImageL = "PILLARBOX/rats_L.bin";
			pillarboxArtImageR = "PILLARBOX/rats_R.bin";
			break;
		case WOOD:
			pillarboxArtImageL = "PILLARBOX/wood_L.bin";
			pillarboxArtImageR = "PILLARBOX/wood_R.bin";
			break;
		case STONE:
			pillarboxArtImageL = "PILLARBOX/stone_L.bin";
			pillarboxArtImageR = "PILLARBOX/stone_R.bin";
			break;
	}

	int tempWidth = -1;
	int tempHeight = -1;

	// Load pillarbox images from files
	ImGuiLoadTexture(&pillarboxArtLeftTexture, pillarboxArtImageL.c_str(), &tempWidth, &tempHeight);
	ImGuiLoadTexture(&pillarboxArtRightTexture, pillarboxArtImageR.c_str(), &tempWidth, &tempHeight);

	pillarboxArtWidth = tempWidth;
	pillarboxArtHeight = tempHeight;
}

// Shutdown ImGui
void ShutdownImGui()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
}


// Update game states
void ImGuiUpdateStates()
{
	int windowHeight;
	int windowWidth;
	SDL_GetWindowSize(sdl_window, &windowWidth, &windowHeight);
	dpiScaling = windowWidth * 1.0 / (oglViewPort[2] * 1.0 + oglViewPort[0] * 2.0);

	scaling = (oglViewPort[3] / 200.00);

	int foundPixelSave = 0;
	int foundPixelLoad = 0;
	int foundPixelSaveLoadCommon = 0;
	bool foundPixelsHud = false;
	bool foundPixelsPaused = false;
	float halfScale = scaling / 2;

	if (inGame)
	{
		if (ImGuiCheckPixel(100 * scaling, 114 * scaling, 0, 0, 0, halfScale) &&
			ImGuiCheckPixel(100 * scaling, 112 * scaling, 134, 89, 4, halfScale) &&
			ImGuiCheckPixel(100 * scaling, 111 * scaling, 0, 0, 0, halfScale))
		{
			foundPixelsPaused = true;
		}

		if (foundPixelsPaused)
		{
			if (ImGuiCheckPixel(33 * scaling, 110 * scaling, 170, 130, 4, 2))
			{
				foundPixelSave = 2;
			}
		} else if (ImGuiCheckPixel(35 * scaling, 187 * scaling, 255, 0, 0, halfScale) &&
			ImGuiCheckPixel(110 * scaling, 181 * scaling, 255, 0, 0, halfScale) &&
			ImGuiCheckPixel(25 * scaling, 194 * scaling, 255, 0, 0, halfScale) &&
			ImGuiCheckPixel(250 * scaling, 12 * scaling, 255, 0, 0, halfScale) &&
			ImGuiCheckPixel(130 * scaling, 53 * scaling, 255, 0, 0, halfScale))
		{
			inGame = false;
			titleScreenLoaded = false;
			titleScreenFaded = false;
		}
		
		if (!foundPixelsPaused && 
			ImGuiCheckPixel(3 * scaling, 198 * scaling, 85, 85, 85, halfScale) &&
			ImGuiCheckPixel(10 * scaling, 199 * scaling, 101, 101, 101, halfScale) &&
			ImGuiCheckPixel(150 * scaling, 199 * scaling, 125, 125, 125, halfScale)) {

			foundPixelsHud = true;
		}
	}
	else if (!titleScreenLoaded && !titleScreenFaded &&
		ImGuiCheckPixel(35 * scaling, 187 * scaling, 202, 194, 121, halfScale) &&
		ImGuiCheckPixel(110 * scaling, 181 * scaling, 117, 113, 61, halfScale) &&
		ImGuiCheckPixel(25 * scaling, 194 * scaling, 45, 40, 8, halfScale) &&
		ImGuiCheckPixel(250 * scaling, 12 * scaling, 81, 73, 32, halfScale) &&
		ImGuiCheckPixel(130 * scaling, 53 * scaling, 202, 198, 125, halfScale))
	{
		titleScreenLoaded = true;
	}
	else if (titleScreenLoaded && !titleScreenFaded &&
		ImGuiCheckPixel(35 * scaling, 187 * scaling, 93, 89, 57, halfScale) &&
		ImGuiCheckPixel(110 * scaling, 181 * scaling, 57, 53, 28, halfScale) &&
		ImGuiCheckPixel(25 * scaling, 194 * scaling, 24, 20, 4, halfScale) &&
		ImGuiCheckPixel(250 * scaling, 12 * scaling, 40, 36, 16, halfScale) &&
		ImGuiCheckPixel(130 * scaling, 53 * scaling, 93, 93, 61, halfScale))
	{
		titleScreenFaded = true;
	}
	else if (titleScreenLoaded && titleScreenFaded && 
		ImGuiCheckPixel(35 * scaling, 187 * scaling, 0, 0, 0, halfScale) &&
		ImGuiCheckPixel(110 * scaling, 181 * scaling, 0, 0, 0, halfScale) &&
		ImGuiCheckPixel(25 * scaling, 194 * scaling, 0, 0, 0, halfScale) &&
		ImGuiCheckPixel(250 * scaling, 12 * scaling, 0, 0, 0, halfScale) &&
		ImGuiCheckPixel(130 * scaling, 53 * scaling, 0, 0, 0, halfScale))
	{
		inGame = true;
	}

	ImGuiLoadPixels(33 * scaling, 95 * scaling, 2);
	if (!inGame && scannedPixels[0][0][0] == 150 && scannedPixels[0][0][1] == 142 &&
		scannedPixels[0][0][2] == 85)
	{
		foundPixelLoad = 1;
	}
	else if (inGame && scannedPixels[0][0][0] == 170 && scannedPixels[0][0][1] == 130 &&
		scannedPixels[0][0][2] == 4)
	{
		foundPixelLoad = 2;
	}

	if (foundPixelLoad > 0 || foundPixelSave > 0)
	{
		ImGuiLoadPixels(150 * scaling, 50 * scaling, halfScale);
		if (scannedPixels[0][0][0] == 150 && scannedPixels[0][0][1] == 142 &&
			scannedPixels[0][0][2] == 85 && !inGame)
		{
			foundPixelSaveLoadCommon = 1;
		}
		else if (scannedPixels[0][0][0] == 170 && scannedPixels[0][0][1] == 130 &&
			scannedPixels[0][0][2] == 4 && inGame)
		{
			foundPixelSaveLoadCommon = 2;
		}

		if (foundPixelSaveLoadCommon > 0)
		{
			if (!ImGuiCheckPixel(150.82 * scaling, 50 * scaling, 0, 0, 0, halfScale))
			{
				foundPixelSaveLoadCommon = 0;
			}
			else
			{
				ImGuiLoadPixels(150 * scaling, 65 * scaling, halfScale);
				if (foundPixelSaveLoadCommon == 1 && scannedPixels[0][0][0] == 150 && scannedPixels[0][0][1] == 142 &&
					scannedPixels[0][0][2] == 85 && !inGame)
				{
					foundPixelSaveLoadCommon = 1;
				}
				else if (foundPixelSaveLoadCommon == 2 && scannedPixels[0][0][0] == 170 && scannedPixels[0][0][1] == 130 &&
					scannedPixels[0][0][2] == 4 && inGame)
				{
					foundPixelSaveLoadCommon = 2;
				}
				else
				{
					foundPixelSaveLoadCommon = 0;
				}
			}
		}
	}

	inPauseMenu = foundPixelsPaused;
	hudShown = foundPixelsHud;

	if ((foundPixelLoad == foundPixelSaveLoadCommon && foundPixelLoad > 0) || (foundPixelSave == foundPixelSaveLoadCommon && foundPixelSave > 0))
	{
		inSaveLoadMenu = true;
	}
	else
	{
		inSaveLoadMenu = false;
		arrowPlacement = 1;
	}
}

// Render all the ImGui items on every frame
void RenderImGui()
{
	glGetIntegerv(GL_VIEWPORT, oglViewPort);

	ImGuiUpdateStates();
	ImGuiCreateArrow();
	ImGuiCreateRadialki();
	ImGuiCreatePillarboxArt();
	ImGuiCreateCrosshair();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

// Create a new ImGui frame
void CreateImGuiFrame(SDL_Renderer * renderer)
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();
	ImGui::EndFrame();
}

// Process ImGui events (through SDL_PollEvents)
void ProcessImGuiEvents(SDL_Event & event)
{
	ImGui_ImplSDL2_ProcessEvent(&event);
}

// Move the save/load arrow one position down
void ImGuiMoveArrowDown()
{


	if (ImGuiIsInLoadSaveMenu())
	{
		arrowPlacement++;
		if (arrowPlacement > 8)
		{
			arrowPlacement = 1;
		}
	} else if (!inGame || inPauseMenu) {

		auto _ImGuiSendDownTask = [](void) {
			SDL_Event sdlevent;

			sdlevent.type = SDL_KEYDOWN;
			sdlevent.key.keysym.sym = SDLK_F22;
			sdlevent.key.keysym.scancode = SDL_SCANCODE_F22;

			SDL_PushEvent(&sdlevent);

			std::this_thread::sleep_for(std::chrono::milliseconds(100));

			sdlevent.type = SDL_KEYUP;
			sdlevent.key.state = SDL_RELEASED;
			SDL_PushEvent(&sdlevent);
		};

		std::thread t1(_ImGuiSendDownTask);
		t1.detach();
	}
}

// Move the save/load arrow one position up
void ImGuiMoveArrowUp()
{
	if (ImGuiIsInLoadSaveMenu())
	{
		arrowPlacement--;
		if (arrowPlacement < 1)
		{
			arrowPlacement = 8;
		}
	} else if (!inGame || inPauseMenu) {
		auto _ImGuiSendUpTask = [](void) {
			SDL_Event sdlevent;

			sdlevent.type = SDL_KEYDOWN;
			sdlevent.key.keysym.sym = SDLK_F23;
			sdlevent.key.keysym.scancode = SDL_SCANCODE_F23;

			SDL_PushEvent(&sdlevent);

			std::this_thread::sleep_for(std::chrono::milliseconds(100));

			sdlevent.type = SDL_KEYUP;
			sdlevent.key.state = SDL_RELEASED;
			SDL_PushEvent(&sdlevent);
		};

		std::thread t1(_ImGuiSendUpTask);
		t1.detach();
	}
}

// Returns true if the game is currently in the save/load menu, and false otherwise
bool ImGuiIsInLoadSaveMenu()
{
	return inSaveLoadMenu;
}

// Returns true when the player is in gameplay (outside of the menus)
bool ImGuiIsInGame()
{
	return inGame;
}

// Returns true when the game is paused
bool ImGuiIsPaused()
{
	return inPauseMenu;
}

// Returns the value of swap_controller_navigation_buttons 
bool ImGuiControllerButtonsAreSwapped()
{
	return swapControllerButtonsConfig;
}

// Confirm the current selection (1-8). Sends an Return keypress when there was no selection
void ImGuiConfirm(int selection)
{
	auto _ImGuiConfirmTask = [](int selection)
	{
		SDL_Event sdlevent;

		sdlevent.type = SDL_KEYDOWN;

		switch (selection)
		{
			case 1:
				sdlevent.key.keysym.sym = SDLK_1;
				sdlevent.key.keysym.scancode = SDL_SCANCODE_1;
				break;
			case 2:
				sdlevent.key.keysym.sym = SDLK_2;
				sdlevent.key.keysym.scancode = SDL_SCANCODE_2;
				break;
			case 3:
				sdlevent.key.keysym.sym = SDLK_3;
				sdlevent.key.keysym.scancode = SDL_SCANCODE_3;
				break;
			case 4:
				sdlevent.key.keysym.sym = SDLK_4;
				sdlevent.key.keysym.scancode = SDL_SCANCODE_4;
				break;
			case 5:
				sdlevent.key.keysym.sym = SDLK_5;
				sdlevent.key.keysym.scancode = SDL_SCANCODE_5;
				break;
			case 6:
				sdlevent.key.keysym.sym = SDLK_6;
				sdlevent.key.keysym.scancode = SDL_SCANCODE_6;
				break;
			case 7:
				sdlevent.key.keysym.sym = SDLK_7;
				sdlevent.key.keysym.scancode = SDL_SCANCODE_7;
				break;
			case 8:
				sdlevent.key.keysym.sym = SDLK_8;
				sdlevent.key.keysym.scancode = SDL_SCANCODE_8;
				break;
			default: break;
		}

		SDL_PushEvent(&sdlevent);

		std::this_thread::sleep_for(std::chrono::milliseconds(150));

		sdlevent.type = SDL_KEYUP;
		sdlevent.key.state = SDL_RELEASED;
		SDL_PushEvent(&sdlevent);
	};

	auto _ImGuiSendEnterTask = [](void)
	{
		SDL_Event sdlevent;

		sdlevent.type = SDL_KEYDOWN;
		sdlevent.key.keysym.sym = SDLK_RETURN;
		sdlevent.key.keysym.scancode = SDL_SCANCODE_RETURN;

		SDL_PushEvent(&sdlevent);

		std::this_thread::sleep_for(std::chrono::milliseconds(100));

		sdlevent.type = SDL_KEYUP;
		sdlevent.key.state = SDL_RELEASED;
		SDL_PushEvent(&sdlevent);
	};

	if (selection > 0)
	{
		std::thread t1(_ImGuiConfirmTask, selection);
		t1.detach();
	}
	else
	{
		if (ImGuiIsInLoadSaveMenu())
		{
			std::thread t1(_ImGuiConfirmTask, arrowPlacement);
			t1.detach();
		}
		else if (!inGame || inPauseMenu)
		{
			std::thread t1(_ImGuiSendEnterTask);
			t1.detach();
		}
	}
}

// Cancel the current selection
void ImGuiCancel()
{
	auto _ImGuiCancelTask = [](void)
	{
		SDL_Event sdlevent;

		sdlevent.type = SDL_KEYDOWN;
		sdlevent.key.keysym.sym = SDLK_ESCAPE;
		sdlevent.key.keysym.scancode = SDL_SCANCODE_ESCAPE;

		SDL_PushEvent(&sdlevent);

		std::this_thread::sleep_for(std::chrono::milliseconds(100));

		sdlevent.type = SDL_KEYUP;
		sdlevent.key.state = SDL_RELEASED;
		SDL_PushEvent(&sdlevent);
	};

	if (!ImGuiIsInLoadSaveMenu())
	{
		return;
	}

	std::thread t1(_ImGuiCancelTask);
	t1.detach();
}

// Set left mouse button to pressed
void ImGuiHoldLMouse()
{
	auto _ImGuiHoldMouseTask = [](void)
	{
		SDL_Event sdlevent;

		sdlevent.type = SDL_MOUSEBUTTONDOWN;
		sdlevent.button.button = SDL_BUTTON_LEFT;
		sdlevent.button.state = SDL_PRESSED;
		sdlevent.button.y = -1;

		SDL_PushEvent(&sdlevent);
		std::this_thread::sleep_for(std::chrono::milliseconds(mouseReleaseDelayConfig));

		ImGuiReleaseLMouse();
	};

	if (lMouseIsHeld || !inGame || inPauseMenu)
	{
		return;
	}

	lMouseIsHeld = true;

	std::thread t1(_ImGuiHoldMouseTask);
	t1.detach();
}

// Release left mouse button
void ImGuiReleaseLMouse()
{
	auto _ImGuiReleaseMouseTask = [](void)
	{
		SDL_Event sdlevent;

		sdlevent.type = SDL_MOUSEBUTTONUP;
		sdlevent.button.button = SDL_BUTTON_LEFT;
		sdlevent.button.state = SDL_RELEASED;
		sdlevent.button.y = -1;

		SDL_PushEvent(&sdlevent);
	};


	if (!lMouseIsHeld)
	{
		return;
	}

	lMouseIsHeld = false;

	std::thread t1(_ImGuiReleaseMouseTask);
	t1.detach();
}

// Set right mouse button to pressed
void ImGuiHoldRMouse()
{
	auto _ImGuiHoldMouseTask = [](void)
	{
		SDL_Event sdlevent;

		sdlevent.type = SDL_MOUSEBUTTONDOWN;
		sdlevent.button.button = SDL_BUTTON_RIGHT;
		sdlevent.button.state = SDL_PRESSED;
		sdlevent.button.y = -1;

		SDL_PushEvent(&sdlevent);
	};

	if (rMouseIsHeld || !inGame || inPauseMenu)
	{
		return;
	}

	rMouseIsHeld = true;

	std::thread t1(_ImGuiHoldMouseTask);
	t1.detach();
}

// Release right mouse button
void ImGuiReleaseRMouse()
{
	auto _ImGuiReleaseMouseTask = [](void)
	{
		SDL_Event sdlevent;

		sdlevent.type = SDL_MOUSEBUTTONUP;
		sdlevent.button.button = SDL_BUTTON_RIGHT;
		sdlevent.button.state = SDL_RELEASED;
		sdlevent.button.y = -1;

		SDL_PushEvent(&sdlevent);
	};


	if (!rMouseIsHeld)
	{
		return;
	}

	rMouseIsHeld = false;

	std::thread t1(_ImGuiReleaseMouseTask);
	t1.detach();
}

// Show Radialki
void ImGuiShowRadialki(SDL_GameController * _gameController)
{
	if (!enableRadialki) {
		return;
	}

	showRadialki = true;
	gameController = _gameController;

	// If it's a mouse action, auto-hide Radialki
	if (!_gameController)
	{
		auto _ImGuiHideRadialkiTask = [](void)
		{
			std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();

			std::this_thread::sleep_for(std::chrono::milliseconds(radialkiScrollAutoHideDelay));
			while (std::chrono::duration_cast<std::chrono::milliseconds>(now - lastScroll).count() < radialkiScrollAutoHideDelay)
			{
				std::this_thread::sleep_for(std::chrono::duration_cast<std::chrono::milliseconds>(now - lastScroll));
				now = std::chrono::steady_clock::now();
			}
			ImGuiHideRadialki();
		};

		std::thread t1(_ImGuiHideRadialkiTask);
		t1.detach();
	}
}

// Hide Radialki
void ImGuiHideRadialki()
{
	if (!enableRadialki) {
		return;
	}

	if (radialkiSlowdownConfig && showRadialki && gameIsSlowedDown)
	{
		CPU_CycleMax = _default_CPU_CycleMax;
		MIXER_Unmute();
		gameIsSlowedDown = false;
	}

	showRadialki = false;

	if (selectedWeapon > 0)
	{
		ImGuiConfirm(selectedWeapon);
	}

	selectedWeapon = -1;
}

// Get Radialki's current selection from the controller
int getRadialkiSelection()
{
	if (!enableRadialki) {
		return -1;
	}

	int	selection = selectedWeapon;

	// Get game controller selection if a controller was used to open the radialki
	if (gameController)
	{
		int axisX = SDL_GameControllerGetAxis(gameController, SDL_CONTROLLER_AXIS_RIGHTX);
		int axisY = SDL_GameControllerGetAxis(gameController, SDL_CONTROLLER_AXIS_RIGHTY);

		int selectionX, selectionY;

		if (axisX <= SDL_JOYSTICK_AXIS_MAX * 0.33 && axisX >= SDL_JOYSTICK_AXIS_MIN * 0.33)
		{
			selectionX = 1;
		}
		else if (axisX < SDL_JOYSTICK_AXIS_MIN * 0.33)
		{
			selectionX = 0;
		}
		else
		{
			selectionX = 2;
		}

		if (axisY <= SDL_JOYSTICK_AXIS_MAX * 0.33 && axisY >= SDL_JOYSTICK_AXIS_MIN * 0.33)
		{
			selectionY = 1;
		}
		else if (axisY < SDL_JOYSTICK_AXIS_MIN * 0.33)
		{
			selectionY = 0;
		}
		else
		{
			selectionY = 2;
		}

		if (selectionX == 0 && selectionY == 0)
		{
			selection = 3;
		}
		else if (selectionX == 0 && selectionY == 1)
		{
			selection = 2;
		}
		else if (selectionX == 0 && selectionY == 2)
		{
			selection = 1;
		}
		else if (selectionX == 1 && selectionY == 0)
		{
			selection = 4;
		}
		else if (selectionX == 2 && selectionY == 0)
		{
			selection = 5;
		}
		else if (selectionX == 2 && selectionY == 1)
		{
			selection = 6;
		}
		else if (selectionX == 2 && selectionY == 2)
		{
			selection = 7;
		}
	}

	renderedSelection = selection;
	return selection;
}

// Create the Radialki weapon selection menu
void ImGuiCreateRadialki()
{
	if (showRadialki && inGame && !inPauseMenu && enableRadialki)
	{
		// Slow down game only when radialki was triggered via gamepad
		if (radialkiSlowdownConfig && CPU_CycleMax > 3000 && gameController)
		{
			MIXER_Mute();
			_default_CPU_CycleMax = CPU_CycleMax;
			CPU_CycleMax = 3000;
			gameIsSlowedDown = true;
		}

		ImDrawList * pDrawList = ImGui::GetBackgroundDrawList();
		int startX = oglViewPort[0];
		int startY = oglViewPort[1];
		int width = oglViewPort[2];
		int height = oglViewPort[3];

		auto weapon1Texture = weapon1DeselectedTexture;
		auto weapon2Texture = weapon2DeselectedTexture;
		auto weapon3Texture = weapon3DeselectedTexture;
		auto weapon4Texture = weapon4DeselectedTexture;
		auto weapon5Texture = weapon5DeselectedTexture;
		auto weapon6Texture = weapon6DeselectedTexture;
		auto weapon7Texture = weapon7DeselectedTexture;

		// Set tile scaling
		ImVec2 itemSize = ImVec2(RADIALKI_SLOT_SIZE * scaling, RADIALKI_SLOT_SIZE * scaling);

		// Get radialki center
		ImVec2 center = ImVec2(startX + scaling * 0.8333 + itemSize.x * 1.5, (startY * 2 + height) / 2);

		int newSelectedWeapon = getRadialkiSelection();
		if (newSelectedWeapon != selectedWeapon)
		{
			selectedWeapon = newSelectedWeapon;
		}

		// Get selected image
		switch (selectedWeapon)
		{
			case 1:
				weapon1Texture = weapon1SelectedTexture;
				break;
			case 2:
				weapon2Texture = weapon2SelectedTexture;
				break;
			case 3:
				weapon3Texture = weapon3SelectedTexture;
				break;
			case 4:
				weapon4Texture = weapon4SelectedTexture;
				break;
			case 5:
				weapon5Texture = weapon5SelectedTexture;
				break;
			case 6:
				weapon6Texture = weapon6SelectedTexture;
				break;
			case 7:
				weapon7Texture = weapon7SelectedTexture;
				break;
			default:
				break;
		}

		// Draw images
		ImGuiAddImage(pDrawList, (ImTextureID)radialkiBackgroundTexture, ImVec2(center.x - itemSize.x * 1.5 - scaling * 0.8333, center.y - itemSize.y * 1.5 - scaling), ImVec2(center.x + itemSize.x * 1.5 + scaling * 0.8333, center.y + itemSize.y * 1.5 + scaling));
		ImGuiAddImage(pDrawList, (ImTextureID)weapon1Texture, ImVec2(center.x - itemSize.x * 1.5, center.y + itemSize.y * 0.5), ImVec2(center.x - itemSize.x * 0.5, center.y + itemSize.y * 1.5));
		ImGuiAddImage(pDrawList, (ImTextureID)weapon2Texture, ImVec2(center.x - itemSize.x * 1.5, center.y - itemSize.y * 0.5), ImVec2(center.x - itemSize.x * 0.5, center.y + itemSize.y * 0.5));
		ImGuiAddImage(pDrawList, (ImTextureID)weapon3Texture, ImVec2(center.x - itemSize.x * 1.5, center.y - itemSize.y * 1.5), ImVec2(center.x - itemSize.x * 0.5, center.y - itemSize.y * 0.5));
		ImGuiAddImage(pDrawList, (ImTextureID)weapon4Texture, ImVec2(center.x - itemSize.x * 0.5, center.y - itemSize.y * 1.5), ImVec2(center.x + itemSize.x * 0.5, center.y - itemSize.y * 0.5));
		ImGuiAddImage(pDrawList, (ImTextureID)weapon5Texture, ImVec2(center.x + itemSize.x * 0.5, center.y - itemSize.y * 1.5), ImVec2(center.x + itemSize.x * 1.5, center.y - itemSize.y * 0.5));
		ImGuiAddImage(pDrawList, (ImTextureID)weapon6Texture, ImVec2(center.x + itemSize.x * 0.5, center.y - itemSize.y * 0.5), ImVec2(center.x + itemSize.x * 1.5, center.y + itemSize.y * 0.5));
		ImGuiAddImage(pDrawList, (ImTextureID)weapon7Texture, ImVec2(center.x + itemSize.x * 0.5, center.y + itemSize.y * 0.5), ImVec2(center.x + itemSize.x * 1.5, center.y + itemSize.y * 1.5));
	}
}

// Load pixels into the "scannedPixels" array
void ImGuiLoadPixels(int x, int y, float leeway)
{
	glReadPixels(oglViewPort[0] + x + leeway,
		oglViewPort[1] + oglViewPort[3] - y - leeway,
		1,
		1,
		GL_RGB,
		GL_UNSIGNED_BYTE,
		scannedPixels);
}

// Check if a given pixel is currently displayed
bool ImGuiCheckPixel(int x, int y, int R, int G, int B, float leeway)
{
	ImGuiLoadPixels(x, y, leeway);

	if (scannedPixels[0][0][0] == R && scannedPixels[0][0][1] == G &&
		scannedPixels[0][0][2] == B)
		return true;

	return false;
}

// Create the save/load arrow
void ImGuiCreateArrow()
{
	ImVec2 imageStart = ImVec2(0, 0);
	ImVec2 imageEnd = ImVec2(0, 0);

	if (ImGuiIsInLoadSaveMenu())
	{
		int posX = 172;
		int posY = 46 + (arrowPlacement - 1) * 15;

		imageStart = ImVec2(oglViewPort[0] + posX * scaling * 0.833,
			oglViewPort[1] + posY * scaling);
		imageEnd = ImVec2(oglViewPort[0] + (posX + ARROW_WIDTH) * scaling * 0.833,
			oglViewPort[1] + (posY + ARROW_HEIGHT) * scaling);
	}
	else
	{
		imageStart = imageEnd = ImVec2(0, 0);
	}

	ImTextureID texture = (ImTextureID)arrowTitleTexture;
	if (inGame)
	{
		texture = (ImTextureID)arrowPausedTexture;
	}

	ImGuiAddImage(ImGui::GetBackgroundDrawList(), texture,
		imageStart,
		imageEnd);
}

// Create the pillarbox art
void ImGuiCreatePillarboxArt()
{
	if (pillarboxArtStyle != NONE)
	{
		ImDrawList * pDrawList = ImGui::GetBackgroundDrawList();
		int startX = oglViewPort[0];
		int startY = oglViewPort[1];
		int width = oglViewPort[2];
		int height = oglViewPort[3];

		int textureHeight = startY * 2 + height;
		int textureWidth = textureHeight * pillarboxArtWidth / pillarboxArtHeight;
		ImVec2 imageEndLeft = ImVec2(startX, textureHeight);
		ImVec2 imageStartLeft = ImVec2(imageEndLeft.x - textureWidth, 0);

		ImVec2 imageStartRight = ImVec2(startX + width, imageStartLeft.y);
		ImVec2 imageEndRight = ImVec2(imageStartRight.x + textureWidth, imageStartRight.y + textureHeight);

		ImGuiAddImage(pDrawList, (ImTextureID)pillarboxArtLeftTexture, imageStartLeft, imageEndLeft);
		ImGuiAddImage(pDrawList, (ImTextureID)pillarboxArtRightTexture, imageStartRight, imageEndRight);
	}
}

// Create the crosshair
void ImGuiCreateCrosshair()
{
	if (showCrosshair && inGame && !inPauseMenu && hudShown)
	{
		ImDrawList * pDrawList = ImGui::GetBackgroundDrawList();
		int startX = oglViewPort[0];
		int startY = oglViewPort[1];
		int width = oglViewPort[2];
		int height = oglViewPort[3];

		ImVec2 imageStart = ImVec2(startX + width / 2 - CROSSHAIR_WIDTH / 2 * scaling, startY + height / 2 - CROSSHAIR_HEIGHT / 2 * scaling);
		ImVec2 imageEnd = ImVec2(startX + width / 2 + CROSSHAIR_WIDTH / 2 * scaling, startY + height / 2 + CROSSHAIR_HEIGHT / 2 * scaling);

		ImGuiAddImage(pDrawList, (ImTextureID)crosshairTexture, imageStart, imageEnd);
	}
}

// Load an OpenGL texture from array
void ImGuiLoadTexture(GLuint * texturePointer, int width, int height, const GLvoid * pixelsArray)
{
	// Create a OpenGL texture identifier
	GLuint imageTexture;
	glGenTextures(1, &imageTexture);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glBindTexture(GL_TEXTURE_2D, imageTexture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glDisable(GL_TEXTURE_2D);
	glTexImage2D(GL_TEXTURE_2D,
		0,
		GL_SRGB8_ALPHA8,
		width,
		height,
		0,
		GL_RGBA,
		GL_UNSIGNED_BYTE,
		pixelsArray);

	*texturePointer = imageTexture;
}

// Load an OpenGL texture from file
void ImGuiLoadTexture(GLuint * texturePointer, const char * filename, int * widthPointer, int * heightPointer)
{
	// Load file data
	FILE * f = fopen(filename, "rb");
	if (f == NULL)
		return;
	fseek(f, 0, SEEK_END);
	size_t file_size = (size_t)ftell(f);
	if (file_size == -1)
		return;
	fseek(f, 0, SEEK_SET);
	auto * file_data = IM_ALLOC(file_size);
	fread(file_data, 1, file_size, f);

	// Load from file
	int width = 0;
	int height = 0;
	unsigned char * image_data = stbi_load_from_memory((const unsigned char *)file_data, (int)file_size, &width, &height, NULL, 4);
	if (image_data == NULL)
		return;

	// Create a OpenGL texture identifier
	GLuint imageTexture;
	glGenTextures(1, &imageTexture);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glBindTexture(GL_TEXTURE_2D, imageTexture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glDisable(GL_TEXTURE_2D);
	glTexImage2D(GL_TEXTURE_2D,
		0,
		GL_SRGB8_ALPHA8,
		width,
		height,
		0,
		GL_RGBA,
		GL_UNSIGNED_BYTE,
		image_data);

	*widthPointer = width;
	*heightPointer = height;
	*texturePointer = imageTexture;
}

// Send a Space keypress
void ImGuiPressSpace()
{
	auto _ImGuiPressSpaceTask = [](void)
	{
		SDL_Event sdlevent;

		sdlevent.type = SDL_KEYDOWN;
		sdlevent.key.keysym.sym = SDLK_SPACE;
		sdlevent.key.keysym.scancode = SDL_SCANCODE_SPACE;

		SDL_PushEvent(&sdlevent);

		std::this_thread::sleep_for(std::chrono::milliseconds(100));

		sdlevent.type = SDL_KEYUP;
		sdlevent.key.state = SDL_RELEASED;
		SDL_PushEvent(&sdlevent);
	};

	if (!inGame || inPauseMenu)
	{
		return;
	}

	std::thread t1(_ImGuiPressSpaceTask);
	t1.detach();
}

// Returns the state of radialkiScrollSelection
bool ImGuiIsRadialkiiScrollSelectionEnabled()
{
	return radialkiScrollSelection;
}

// Select next weapon in radialki
void ImGuiSelectNextWeapon()
{
	lastScroll = std::chrono::steady_clock::now();

	selectedWeapon = renderedSelection + 1;
	if (selectedWeapon > 7)
	{
		selectedWeapon = 1;
	}
	else if (selectedWeapon < 1)
	{
		selectedWeapon = 1;
	}
}

// Select previous weapon in radialki
void ImGuiSelectPreviousWeapon()
{
	lastScroll = std::chrono::steady_clock::now();

	selectedWeapon = renderedSelection - 1;
	if (selectedWeapon > 7)
	{
		selectedWeapon = 1;
	}
	else if (selectedWeapon < 1)
	{
		selectedWeapon = 7;
	}
}

// Toggle Auto-Sprint
void ImGuiToggleAutosprint()
{

	if (!inGame || inPauseMenu)
	{
		return;
	}

	auto _ImGuiHoldAutosprintKeyTask = []()
	{
		SDL_Event sdlevent;

		sdlevent.type = SDL_KEYDOWN;
		sdlevent.key.keysym.scancode = sprintScancode;

		SDL_PushEvent(&sdlevent);
	};

	auto _ImGuiReleaseAutosprintKeyTask = []()
	{
		SDL_Event sdlevent;

		sdlevent.type = SDL_KEYUP;
		sdlevent.key.state = SDL_RELEASED;
		sdlevent.key.keysym.scancode = sprintScancode;

		SDL_PushEvent(&sdlevent);
	};

	if (!leftAltHeld)
	{
		leftAltHeld = true;
		std::thread t1(_ImGuiHoldAutosprintKeyTask);
		t1.detach();
	}
	else
	{
		leftAltHeld = false;
		std::thread t1(_ImGuiReleaseAutosprintKeyTask);
		t1.detach();
	}
}

// Get keyboard scancode for autosprint
SDL_Scancode ImGuiGetAutosprintScancode(int option_index)
{
	switch (option_index) {
		case 2:
			return autosprintScancode2;
		case 3:
			return autosprintScancode3;
		case 4:
			return autosprintScancode4;
		case 1:
		default:
			return autosprintScancode1;
	}
}

// Get gamepad button for autosprint
int ImGuiGetAutosprintButton(int option_index)
{
	switch (option_index) {
		case 2:
			return autosprintButton2;
		case 3:
			return autosprintButton3;
		case 4:
			return autosprintButton4;
		case 1:
		default:
			return autosprintButton1;
	}
}

// Get index of gamepad for autosprint button
int ImGuiGetAutosprintJoystick(int option_index) {
	switch (option_index) {
		case 2:
			return autosprintButton2Index;
		case 3:
			return autosprintButton3Index;
		case 4:
			return autosprintButton4Index;
		case 1:
		default:
			return autosprintButton1Index;
	}
}

// Get gamepad button for radialki
int ImGuiGetRadialkiButton(int option_index) {
	switch (option_index) {
		case 2:
			return radialkiButton2;
		case 3:
			return radialkiButton3;
		case 4:
			return radialkiButton4;
		case 1:
		default:
			return radialkiButton1;
	}
}

// Get index of gamepad for a radialki button
int ImGuiGetRadialkiJoystick(int option_index) {
	switch (option_index) {
		case 2:
			return radialkiButton2Index;
		case 3:
			return radialkiButton3Index;
		case 4:
			return radialkiButton4Index;
		case 1:
		default:
			return radialkiButton1Index;
	}
}

// Send mouse movement
void ImGuiSendMouseMovement(Sint32 movement)
{
	if (!showRadialki) {
		SDL_Event event;
		event.type = SDL_MOUSEMOTION;
		event.motion.xrel = movement;
		event.motion.windowID = SDL_GetWindowID(sdl_window);
		SDL_PushEvent(&event);
	}
}

// Wrapper function for adding images, handling dpi properly
void ImGuiAddImage(ImDrawList * drawlist, ImTextureID user_texture_id, ImVec2 p_min, ImVec2 p_max) {
	p_min.x = dpiScaling * p_min.x;
	p_min.y = dpiScaling * p_min.y;
	p_max.x = dpiScaling * p_max.x;
	p_max.y = dpiScaling * p_max.y;
	drawlist->AddImage(user_texture_id, p_min, p_max);
}