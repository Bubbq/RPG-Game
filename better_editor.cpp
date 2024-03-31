#include "raylib.h"
#include <raymath.h>
#include <string>
#include <vector>

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#undef RAYGUI_IMPLEMENTATION            
#define GUI_WINDOW_FILE_DIALOG_IMPLEMENTATION
#include "gui_window_file_dialog.h"

const int SCREEN_WIDTH = 992;
const int SCREEN_HEIGHT = 992;
const int PANEL_HEIGHT = 24;

const int TILE_SIZE = 16;
const float SCALE = 2.0f;
const int SCREEN_TILE_SIZE = TILE_SIZE * SCALE;
const int TILES_PER_ROW = SCREEN_WIDTH / SCREEN_TILE_SIZE;
int DISPLAY_TILE_SIZE = SCREEN_TILE_SIZE;

struct Tile
{
	Vector2 src;
	Vector2 sp;
	Texture2D tx;
	std::string fp;
};

void init()
{
    SetTraceLogLevel(LOG_ERROR);
    SetTargetFPS(60);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "EDITOR");
}

void deinit(Texture2D texture, std::vector<Tile>& tile_dict)
{
	for(int i = 0; i < tile_dict.size(); i++)
	{
		UnloadTexture(tile_dict[i].tx);
	}

    UnloadTexture(texture);
    CloseWindow();
}

// breaks the user-selected png into tiles that user can choose from to append to the world
void readPNG(Texture2D texture, std::vector<Tile>& tile_dict, std::string fp)
{
	Vector2 cp = {0, 0};
	// surface area of the image with respect to tilesize
	int sa = (texture.width / TILE_SIZE) * (texture.height / TILE_SIZE);
	for(int i = 0; i < sa; i++)
	{
		tile_dict.push_back({cp, { 0 }, texture, fp});

		if(cp.x + TILE_SIZE != texture.width)
		{
			cp.x += TILE_SIZE;
		}
			
		else
		{
			cp.x = 0;
			cp.y += TILE_SIZE;
		}
	}
}

// showing availible tiles that user can append the map to
void chooseTiles(std::vector<Tile> tile_dict, Rectangle side_panel, Tile& currTile)
{
	// the last tile before overflowing the side panel, TODO FIX HARDCODED Y VALUE FOR LT.Y
	Vector2 lt = {side_panel.x + side_panel.width - DISPLAY_TILE_SIZE,888};
	Vector2 cp1 = {side_panel.x,float(side_panel.y + PANEL_HEIGHT)};

	for(int i = 0; i < tile_dict.size(); i++)
	{
		tile_dict[i].sp = cp1;

		// shrink to fit panel
		if(tile_dict[i].sp.x == lt.x && tile_dict[i].sp.y == lt.y)
		{
			DISPLAY_TILE_SIZE /= 2;
		}
		
		DrawTexturePro(tile_dict[i].tx, {tile_dict[i].src.x, tile_dict[i].src.y, TILE_SIZE,TILE_SIZE}, {cp1.x, cp1.y, float(DISPLAY_TILE_SIZE), float(DISPLAY_TILE_SIZE)}, {0,0}, 0, WHITE);
		cp1.x += DISPLAY_TILE_SIZE;
		
		// moving to new row
		if((i + 1) % int(side_panel.width / DISPLAY_TILE_SIZE) == 0 && i != 0)
		{
			cp1.x = side_panel.x;
			cp1.y += DISPLAY_TILE_SIZE;
		}

		if(CheckCollisionPointRec(GetMousePosition(), {tile_dict[i].sp.x, tile_dict[i].sp.y, float(DISPLAY_TILE_SIZE), float(DISPLAY_TILE_SIZE)}))
		{
			// higlight tile that user is hovering over
			DrawRectangleLines(tile_dict[i].sp.x, tile_dict[i].sp.y, DISPLAY_TILE_SIZE, DISPLAY_TILE_SIZE, BLUE);
			// update current tile
			if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
			{
				currTile = tile_dict[i];
			}
		}
	}

	// highlight the current tile a user has selected
	if(currTile.tx.id != 0)
	{
		DrawRectangleLines(currTile.sp.x, currTile.sp.y, DISPLAY_TILE_SIZE,DISPLAY_TILE_SIZE, LIGHTGRAY);
	}
}

int main()
{
    init();
    
    GuiWindowFileDialogState fileDialogState = InitGuiWindowFileDialog(GetWorkingDirectory());
	Rectangle side_panel = {TILE_SIZE, float(SCREEN_TILE_SIZE), SCREEN_TILE_SIZE * 8.0f, SCREEN_TILE_SIZE * 28.0f};
	std::vector<Tile> tile_dict;
    std::string userFilePath;
    Texture2D texture = { 0 };
	Tile currTile = {};

    while (!WindowShouldClose())
    {
        // pressing "select" in file dialog
        if (fileDialogState.SelectFilePressed)
        {
            if (IsFileExtension(fileDialogState.fileNameText, ".png"))
            {
                userFilePath = TextFormat("%s" PATH_SEPERATOR "%s", fileDialogState.dirPathText, fileDialogState.fileNameText);
                // UnloadTexture(texture);
                texture = LoadTexture(userFilePath.c_str());
				readPNG(texture, tile_dict, userFilePath);
            }

            fileDialogState.SelectFilePressed = false;
        }

        BeginDrawing();
            ClearBackground(WHITE);
			// drawing tiles to the side
			GuiPanel(side_panel, "AVAILIBLE TILES");
			chooseTiles(tile_dict, side_panel, currTile);

            // only focus on the window choosing your file
            if (fileDialogState.windowActive) GuiLock();

            if (GuiButton((Rectangle){ GetScreenWidth() - (SCREEN_TILE_SIZE / 1.0f), 0, float(SCREEN_TILE_SIZE), float(SCREEN_TILE_SIZE) }, GuiIconText(ICON_FILE_OPEN, ""))) fileDialogState.windowActive = true;

            // able to click other buttons once you select the desired image
            GuiUnlock();

			// updates the file dialouge window
            GuiWindowFileDialog(&fileDialogState);

			// DrawRectangle(side_panel.x + side_panel.width - DISPLAY_TILE_SIZE,side_panel.height, SCREEN_TILE_SIZE, SCREEN_TILE_SIZE, RED);
        EndDrawing();
    }

    deinit(texture, tile_dict);
    return 0;
}