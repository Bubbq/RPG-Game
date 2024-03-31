#include "raylib.h"
#include <cstddef>
#include <iostream>
#include <raymath.h>
#include <string>
#include <vector>

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#undef RAYGUI_IMPLEMENTATION            
#define GUI_WINDOW_FILE_DIALOG_IMPLEMENTATION
#include "gui_window_file_dialog.h"

const int SCREEN_WIDTH = 1120;
const int SCREEN_HEIGHT = 992;
const int PANEL_HEIGHT = 24;

const int TILE_SIZE = 16;
const float SCALE = 2.0f;
const int SCREEN_TILE_SIZE = TILE_SIZE * SCALE;
const int TILES_PER_ROW = SCREEN_WIDTH / SCREEN_TILE_SIZE;
int DISPLAY_TILE_SIZE = SCREEN_TILE_SIZE;

enum Element{
	WALL = 0,
	FLOOR = 1,
	DOOR = 2,
	BUFF = 3,
	INTERACTABLE = 4,
	UNDF = 5,
};

struct Tile
{
	Vector2 src;
	Vector2 sp;
	Texture2D tx;
	std::string fp;
	Element tt;
};

std::vector<Tile> undf;
std::vector<Tile> tmp = undf;
std::vector<Tile> walls;
std::vector<Tile> floors;
std::vector<Tile> doors;
std::vector<Tile> buffs;
std::vector<Tile> interactables;

std::string ReplaceString(std::string subject, const std::string& search, const std::string& replace)
{
 
  size_t pos = 0;
 
  while ((pos = subject.find(search, pos)) != std::string::npos)
  {
       subject.replace(pos, search.length(), replace);
       pos += replace.length();
  }
 
  return subject;
}

std::string linuxFormat(std::string input)
{
   // change special characters
   input = ReplaceString(input, " ", "\\ ");
   input = ReplaceString(input, "(", "\\(");
   input = ReplaceString(input, ")", "\\)");
   input = ReplaceString(input, "&", "\\&");
   input = ReplaceString(input, "'", "\\'");
   input = ReplaceString(input, "`", "\\`");
   input = ReplaceString(input, "’", "\\’");
   input = ReplaceString(input, "\"", "\\\"");

   return input;
}

// void saveLevel()
// {
// 	std::ofstream outFile;
// 	outFile.open("level.txt");
// 	if(!outFile)
// 	{
// 		std::cerr << "ERROR SAVING LEVEL \n";
// 		return;
// 	}

// 	for(int i = 0; i < worldTile.size(); i++)
// 	{
// 		outFile << worldTile[i].src.x << ","
// 		 		<< worldTile[i].src.y << ","
// 				<< worldTile[i].sp.x << ","
// 				<< worldTile[i].sp.y << ","
// 				<< linuxFormat(worldTile[i].fp) << ","
// 				<< worldTile[i].tt << std::endl;

// 		UnloadTexture(worldTile[i].tx);
// 	}

// 	outFile.close();
// }

// void loadTiles()
// {
//     std::ifstream inFile;
//     inFile.open("level.txt");
//     if (!inFile)
//     {
//         std::cerr << "NO FILE SAVED \n";
//         return;
//     }

//     Vector2 src = { 0 };
//     Vector2 sp = { 0 };
//     Texture2D tx = { 0 };
//     std::string fp = "";
//     int tt = 0;

//     while (inFile >> src.x >> src.y >> sp.x >> sp.y >> fp >> tt)
//     {
//         Image img = LoadImage(fp.c_str());
//         if (IsImageReady(img))
//         {
//             tx = LoadTextureFromImage(img);
//             worldTile.push_back({src, sp, tx, fp, (Element)tt});
//         }
//     }

//     std::cout << worldTile.size() << std::endl;
//     inFile.close();
// }

void init()
{
    SetTraceLogLevel(LOG_ERROR);
    SetTargetFPS(60);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "EDITOR");
	// loadTiles();
}

void deinit(Texture2D texture, std::vector<Tile>& tile_dict)
{
	for(int i = 0; i < tile_dict.size(); i++)
	{
		UnloadTexture(tile_dict[i].tx);
	}
	
	// saveLevel();
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

		// move to next row	
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
	// the last tile before overflowing the side panel, TODO FIX HARDCODED Y VALUE
	Vector2 lt = {side_panel.x + side_panel.width - DISPLAY_TILE_SIZE,888};
	Vector2 cp1 = {side_panel.x,float(side_panel.y + PANEL_HEIGHT)};

	for(int i = 0; i < tile_dict.size(); i++)
	{
		tile_dict[i].sp = cp1;

		// shrink to fit
		if(tile_dict[i].sp.x == lt.x && tile_dict[i].sp.y == lt.y)
		{
			DISPLAY_TILE_SIZE /= 2;
			currTile = { 0 };
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

void editWorld(Rectangle worldArea, Tile& currTile, int ce)
{
	// get nearest coord divisible by screen tile size
	int mpx = (((int)GetMousePosition().x >> (int)log2(SCREEN_TILE_SIZE)) << (int)log2(SCREEN_TILE_SIZE));
    int mpy = (((int)GetMousePosition().y >> (int)log2(SCREEN_TILE_SIZE)) << (int)log2(SCREEN_TILE_SIZE));
	
	// find corresp vect for selected layer
	switch (ce)
	{
		case WALL: tmp = walls; break;
		case FLOOR: tmp = floors; break;
		case DOOR: tmp = doors; break;
		case BUFF: tmp = buffs; break;
		case INTERACTABLE: tmp = interactables; break;

		// in payer view, draw all tiles only
		default:
			for(int i = 0; i < walls.size(); i++)
				DrawTexturePro(walls[i].tx, {walls[i].src.x, walls[i].src.y, TILE_SIZE, TILE_SIZE}, {walls[i].sp.x, walls[i].sp.y, float(SCREEN_TILE_SIZE), float(SCREEN_TILE_SIZE)}, {0,0}, 0, WHITE);
			for(int i = 0; i < floors.size(); i++)
				DrawTexturePro(floors[i].tx, {floors[i].src.x, floors[i].src.y, TILE_SIZE, TILE_SIZE}, {floors[i].sp.x, floors[i].sp.y, float(SCREEN_TILE_SIZE), float(SCREEN_TILE_SIZE)}, {0,0}, 0, WHITE);
			for(int i = 0; i < doors.size(); i++)
				DrawTexturePro(doors[i].tx, {doors[i].src.x, doors[i].src.y, TILE_SIZE, TILE_SIZE}, {doors[i].sp.x, doors[i].sp.y, float(SCREEN_TILE_SIZE), float(SCREEN_TILE_SIZE)}, {0,0}, 0, WHITE);
			for(int i = 0; i < buffs.size(); i++)
				DrawTexturePro(buffs[i].tx, {buffs[i].src.x, buffs[i].src.y, TILE_SIZE, TILE_SIZE}, {buffs[i].sp.x, buffs[i].sp.y, float(SCREEN_TILE_SIZE), float(SCREEN_TILE_SIZE)}, {0,0}, 0, WHITE);
			for(int i = 0; i < interactables.size(); i++)
				DrawTexturePro(interactables[i].tx, {interactables[i].src.x, interactables[i].src.y, TILE_SIZE, TILE_SIZE}, {interactables[i].sp.x, interactables[i].sp.y, float(SCREEN_TILE_SIZE), float(SCREEN_TILE_SIZE)}, {0,0}, 0, WHITE);
			break;
	}

	if(CheckCollisionPointRec(GetMousePosition(), worldArea))
	{
		DrawRectangleLines(mpx, mpy, SCREEN_TILE_SIZE, SCREEN_TILE_SIZE, BLUE);
		// add tiles
		if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !currTile.fp.empty() && ce != UNDF)
		{
			Tile newTile = {currTile.src, {float(mpx), float(mpy)}, currTile.tx, currTile.fp, (Element)ce};
			tmp.push_back(newTile);
		}
	}

	for(int i = 0; i < tmp.size(); i++)
	{
		if(!tmp[i].fp.empty())
		{
			// tile erasure
			if(CheckCollisionPointRec(GetMousePosition(), {tmp[i].sp.x, tmp[i].sp.y, float(SCREEN_TILE_SIZE), float(SCREEN_TILE_SIZE)}) && IsMouseButtonDown(MOUSE_RIGHT_BUTTON))
			{
				tmp.erase(tmp.begin() + i);
			}
			
			// draw tile to that specific layer
			DrawTexturePro(tmp[i].tx, {tmp[i].src.x, tmp[i].src.y, TILE_SIZE, TILE_SIZE}, {tmp[i].sp.x, tmp[i].sp.y, float(SCREEN_TILE_SIZE), float(SCREEN_TILE_SIZE)}, {0,0}, 0, WHITE);
		}
	}

	// update the vects with the new appended temp
	switch (ce)
	{
		case WALL: walls = tmp; break;
		case FLOOR: floors = tmp; break;
		case DOOR: doors = tmp; break;
		case BUFF: buffs = tmp; break;
		case INTERACTABLE: interactables = tmp; break;
		default:
			break;
	}

}

int main()
{
    init();
    
    GuiWindowFileDialogState fileDialogState = InitGuiWindowFileDialog(GetWorkingDirectory());
	Rectangle side_panel = {TILE_SIZE, float(SCREEN_TILE_SIZE), SCREEN_TILE_SIZE * 8.0f, SCREEN_TILE_SIZE * 28.0f};
	Rectangle layer_panel = {GetScreenWidth() - TILE_SIZE - SCREEN_TILE_SIZE * 5.0f, float(SCREEN_TILE_SIZE), SCREEN_TILE_SIZE * 5.0f, SCREEN_TILE_SIZE * 7.0f};
	// current layer user selects
	int cl = 0;
	std::vector<Tile> tile_dict;
    std::string userFilePath;
    Texture2D texture = { 0 };
	Rectangle worldArea = {side_panel.x + side_panel.width + TILE_SIZE, float(SCREEN_TILE_SIZE), 512,512};
	Camera2D camera = { 0 };
	camera.zoom = 1.0f;
	Tile currTile = { 0 };

    while (!WindowShouldClose())
    {
        // moving screen based on mouse drag
		if(IsMouseButtonDown(MOUSE_RIGHT_BUTTON))
		{
			// get the difference in mouse pos and scale relative to zoom
			Vector2 delta = GetMouseDelta();
			delta = Vector2Scale(delta, -1.0f / camera.zoom);

			camera.target = Vector2Add(delta, camera.target);
		}
		
		// pressing "select" in file dialog
        if (fileDialogState.SelectFilePressed)
        {
            if (IsFileExtension(fileDialogState.fileNameText, ".png"))
            {
                userFilePath = TextFormat("%s" PATH_SEPERATOR "%s", fileDialogState.dirPathText, fileDialogState.fileNameText);
                texture = LoadTexture(userFilePath.c_str());
				readPNG(texture, tile_dict, userFilePath);
            }

            fileDialogState.SelectFilePressed = false;
        }

        BeginDrawing();
            ClearBackground(WHITE);
			
			// drawing tiles to the side
			GuiPanel(side_panel, "TILES");
			chooseTiles(tile_dict, side_panel, currTile);
			DrawRectangleLines(worldArea.x, worldArea.y, worldArea.width, worldArea.height, GRAY);
			editWorld(worldArea, currTile, (Element)cl);

            // only focus on the window choosing your file
            if (fileDialogState.windowActive) GuiLock();

            if (GuiButton((Rectangle){ GetScreenWidth() - float(SCREEN_TILE_SIZE / 2.0f), 0, float(SCREEN_TILE_SIZE / 2.0f), float(SCREEN_TILE_SIZE / 2.0f) }, GuiIconText(ICON_FILE_OPEN, ""))) fileDialogState.windowActive = true;

            // able to click other buttons once you select the desired image
            GuiUnlock();

			// updates the file dialouge window
            GuiWindowFileDialog(&fileDialogState);

			GuiPanel(layer_panel, "LAYERS");
			// showing layers
            GuiToggleGroup((Rectangle){ GetScreenWidth() - SCREEN_TILE_SIZE * 5.0f, float(SCREEN_TILE_SIZE + PANEL_HEIGHT + TILE_SIZE), SCREEN_TILE_SIZE * 4.0f, 24}, "WALLS \n FLOORS \n DOORS \n BUFFS \n INTERACTABLES \n PLAYER VIEW", &cl);
        EndDrawing();
    }

    deinit(texture, tile_dict);
    return 0;
}