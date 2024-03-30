#include "tile_generation.h"
#include <raylib.h>

void loadWorld(World& world, int& cl)
{
    std::string name;
    Vector2 screenPos;
    Vector2 src;
    bool walkable;
    int tileType;
    
    std::ifstream inFile;
    inFile.open("./levels/level" + std::to_string(cl) + "/walls.txt");
    if(!inFile)
    {
        std::cerr << "NO WALLS SAVED \n";
        return;
    }

    while(inFile >> src.x >> src.y >> name >> tileType >> screenPos.x >> screenPos.y)
        world.walls.push_back({src, screenPos, name, (Element)tileType});

    inFile.close();
    
    inFile.open("./levels/level" + std::to_string(cl) + "/floors.txt");
    if(!inFile)
    {
        std::cerr << "NO FLOORS SAVED \n";
        return;
    }

    while(inFile >> src.x >> src.y >> name >> tileType >> screenPos.x >> screenPos.y)
        world.floors.push_back({src, screenPos, name, (Element)tileType});

    inFile.close();

    inFile.open("./levels/level" + std::to_string(cl) + "/items.txt");
    if(!inFile)
    {
        std::cerr << "NO ITEMS SAVED \n";
        return;
    }

    while(inFile >> src.x >> src.y >> name >> tileType >> screenPos.x >> screenPos.y)
        world.items.push_back({src, screenPos, name, (Element)tileType});

    inFile.close();
}

// drawing the world
void drawWorld(World& world, Texture2D TILES, Texture2D ITEMS, int yOffset)
{
    for(int i = 0; i < world.walls.size(); i++)
        if(!world.walls[i].name.empty())
            DrawTexturePro(TILES, {world.walls[i].src.x, world.walls[i].src.y, TILE_SIZE, TILE_SIZE}, {world.walls[i].screenPos.x, world.walls[i].screenPos.y, TILE_SIZE * SCALE, TILE_SIZE * SCALE}, {0,0}, 0, WHITE);
    
    for(int i = 0; i < world.floors.size(); i++)
        if(!world.floors[i].name.empty())
            DrawTexturePro(TILES, {world.floors[i].src.x, world.floors[i].src.y, TILE_SIZE, TILE_SIZE}, {world.floors[i].screenPos.x, world.floors[i].screenPos.y, TILE_SIZE * SCALE, TILE_SIZE * SCALE}, {0,0}, 0, WHITE);
    
    for(int i = 0; i < world.items.size(); i++)
        if(!world.items[i].name.empty())
            DrawTexturePro(ITEMS, {world.items[i].src.x, world.items[i].src.y, TILE_SIZE, TILE_SIZE}, {world.items[i].screenPos.x, world.items[i].screenPos.y, TILE_SIZE * SCALE, TILE_SIZE * SCALE}, {0,0}, 0, WHITE);
}

void saveWorld(World& world, int& cl)
{
    std::ofstream outFile;
    outFile.open("./levels/level" + std::to_string(cl) + "/floors.txt");
    if(!outFile)
    {
        std::cerr << "ERROR TRYING TO SAVE FLOORS \n";
        return;
    }

    for(int i = 0; i < world.floors.size(); i++)
    {
        if(!world.floors[i].name.empty())
        {
                outFile << world.floors[i].src.x
                << " " << world.floors[i].src.y << " "
                << world.floors[i].name << " "
                << world.floors[i].tileType << " "
                << world.floors[i].screenPos.x << " "
                << world.floors[i].screenPos.y <<  std::endl;
        }
    }

    outFile.close();

    outFile.open("./levels/level" + std::to_string(cl) + "/walls.txt");
    if(!outFile)
    {
        std::cerr << "ERROR TRYING TO SAVE WALLS \n";
        return;
    }

    for(int i = 0; i < world.walls.size(); i++)
    {
        if(!world.walls[i].name.empty())
        {
                outFile << world.walls[i].src.x
                << " "<< world.walls[i].src.y << " "
                << world.walls[i].name << " "
                << world.walls[i].tileType << " "
                << world.walls[i].screenPos.x << " "
                << world.walls[i].screenPos.y <<  std::endl;
        }
    }

    outFile.close();

    outFile.open("./levels/level" + std::to_string(cl) + "/items.txt");
    if(!outFile)
    {
        std::cerr << "ERROR TRYING TO SAVE ITEMS \n";
        return;
    }

    for(int i = 0; i < world.items.size(); i++)
    {
        if(!world.items[i].name.empty())
        {
                outFile << world.items[i].src.x
                << " "<< world.items[i].src.y << " "
                << world.items[i].name << " "
                << world.items[i].tileType << " "
                << world.items[i].screenPos.x << " "
                << world.items[i].screenPos.y <<  std::endl;
        }
    }

    outFile.close();

    outFile.open("./levels/level" + std::to_string(cl) + "/spawn.txt");
    if(!outFile)
    {
        std::cerr << "ERROR TRYING TO SAVE SPAWN POINT \n";
        return;
    }

    outFile << world.spawn.x << " " << world.spawn.y;

    outFile.close();
}

// reads and stores the information of every tile in a vector
void readTiles(std::vector<Tile>& allTiles)
{
    std::ifstream inFile;
    inFile.open("src/tiles.txt");

    if(!inFile)
    {
        std::cerr << "ERROR READING TILES \n" << std::endl;
        return;
    }

    Vector2 src;
    std::string name;
    int tileType;

    while(inFile >> src.x >> src.y >> tileType >> name)
        allTiles.push_back((Tile){src, {}, name, (Element)tileType});
    
    inFile.close();

    inFile.open("src/items.txt");
    if(!inFile)
    {
        std::cerr << "ERROR WHEN TRYING TO READ ITEM TILES /n";
        return;
    }
    
    while(inFile >> src.x >> src.y >> tileType >> name)
        allTiles.push_back((Tile){src, {}, name, (Element)tileType});

    inFile.close();
}