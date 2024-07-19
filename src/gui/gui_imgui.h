#pragma once
#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_opengl3.h>
#include <SDL2/SDL_gamecontroller.h>
#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_video.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_gamecontroller.h>

void ShutdownImGui();
void RenderImGui();
void ProcessImGuiEvents(SDL_Event &);
void InitImGui(SDL_Window *, SDL_GLContext);
void CreateImGuiFrame(SDL_Renderer *);
void ImGuiShowRadialki(SDL_GameController * gameController = NULL);
void ImGuiHideRadialki();
void ImGuiMoveArrowUp();
void ImGuiMoveArrowDown();
void ImGuiConfirm(int selection = -1);
void ImGuiCancel();
void ImGuiCreateArrow();
void ImGuiCreateRadialki();
void ImGuiLoadTexture(GLuint *, int, int, const GLvoid *);
void ImGuiLoadTexture(GLuint *, const char *, int * widthPointer, int * heightPointer);
void ImGuiAddImage(ImDrawList * drawlist, ImTextureID user_texture_id, ImVec2 p_min, ImVec2 p_max);
bool ImGuiIsInLoadSaveMenu();
void ImGuiLoadPixels(int, int, float leeway = 0);
bool ImGuiCheckPixel(int, int, int, int, int, float leeway = 0);
void ImGuiUpdateStates();
void ImGuiCreatePillarboxArt();
bool ImGuiControllerButtonsAreSwapped();
bool ImGuiIsInGame();
bool ImGuiIsPaused();
void ImGuiHoldLMouse();
void ImGuiReleaseLMouse();
void ImGuiHoldRMouse();
void ImGuiReleaseRMouse();
void ImGuiPressSpace();
bool ImGuiIsRadialkiiScrollSelectionEnabled();
void ImGuiSelectNextWeapon();
void ImGuiSelectPreviousWeapon();
void ImGuiToggleAutosprint();
void ImGuiSendMouseMovement(int movement);
void ImGuiCreateCrosshair();
SDL_Scancode ImGuiGetAutosprintScancode(int option_index);
int ImGuiGetAutosprintButton(int option_index);
int ImGuiGetAutosprintJoystick(int option_index);
int ImGuiGetRadialkiButton(int option_index);
int ImGuiGetRadialkiJoystick(int option_index);