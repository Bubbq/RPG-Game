#include "Graphics.h"

// have to draw the element passed based on its position in the png
void renderGraphics(int element, Vector2 pos, float scale){

    // drawing fully enclosed room
    if((element & ROOM) != 0){
        // the shape we're extracting from the src png
        Rectangle src = (Rectangle){0,0,TILE_SIZE * 6, TILE_SIZE * 5};
       
        // where we want to draw this extracted rectangle to our screen
        Rectangle dst = (Rectangle){pos.x, pos.y, TILE_SIZE * 6, TILE_SIZE * 5};
        
        // scale the location of src rect up to fit our screen
        scaleRec(dst, scale);

        // finally draw the image to our screen
        DrawTexturePro(graphics.tile_texture, src, dst, (Vector2){0,0}, 0, WHITE);
    }

    // drawing horizontal wall
    if((element & WALL_H_1) != 0){
        Rectangle src_wall_h = (Rectangle){TILE_SIZE, 0, TILE_SIZE, TILE_SIZE};
        Rectangle dst_wall_h = (Rectangle){pos.x, pos.y,TILE_SIZE, TILE_SIZE};
        scaleRec(dst_wall_h, scale);
        DrawTexturePro(graphics.tile_texture, src_wall_h, dst_wall_h, (Vector2){0,0}, 0, WHITE);
    }

    // wall with corner (top right)
    if((element & WALL_H_2) != 0){
        Rectangle wall_h_2_src = (Rectangle){TILE_SIZE * 3, TILE_SIZE * 5, TILE_SIZE, TILE_SIZE};
        Rectangle wall_h_2_dst = (Rectangle){pos.x, pos.y, TILE_SIZE, TILE_SIZE};
        scaleRec(wall_h_2_dst, scale);
        DrawTexturePro(graphics.tile_texture, wall_h_2_src, wall_h_2_dst, (Vector2){0,0}, 0, WHITE);
    }

    // wall with corner (top left)
    if((element & WALL_H_3) != 0){
        Rectangle wall_h_2_src = (Rectangle){TILE_SIZE * 4, TILE_SIZE * 5, TILE_SIZE, TILE_SIZE};
        Rectangle wall_h_2_dst = (Rectangle){pos.x, pos.y, TILE_SIZE, TILE_SIZE};
        scaleRec(wall_h_2_dst, scale);
        DrawTexturePro(graphics.tile_texture, wall_h_2_src, wall_h_2_dst, (Vector2){0,0}, 0, WHITE);
    }

    // drawing the room's floor (left opening)
    if((element & ROOM_FLOOR) != 0){
        Rectangle room_floor_src = (Rectangle){TILE_SIZE * 6, 0, TILE_SIZE, TILE_SIZE};
        Rectangle room_floor_dst = (Rectangle){pos.x, pos.y, TILE_SIZE, TILE_SIZE};
        scaleRec(room_floor_dst, scale);
        DrawTexturePro(graphics.tile_texture, room_floor_src, room_floor_dst, (Vector2){0,0}, 0, WHITE);
    }

    // makes an opening  with 2 walls and a floor in between them
    if((element & DOOR_LEFT_OPEN) != 0){
        // draw the first wall
        renderGraphics(WALL_H_1, Vector2Add(pos, DOWN), scale);
        // draw the room floor right below it
        renderGraphics(ROOM_FLOOR, Vector2Add(Vector2Add(pos, DOWN), DOWN), scale);
        // draw the opposing wall
        renderGraphics(WALL_H_2, Vector2Add(Vector2Add(Vector2Add(pos, DOWN), DOWN), DOWN),scale);
    }

    if((element & DOOR_RIGHT_OPEN) != 0){
        // draw basic wall on the right side of the room (5 tiles away)
        renderGraphics(WALL_H_1, Vector2Add(pos, Vector2Add(RIGHT, DOWN)), scale);
        // draw filling room floor
        renderGraphics(ROOM_FLOOR, Vector2Add(Vector2Add(pos, Vector2Add(RIGHT, DOWN)), DOWN), scale);
        // draw opposing wall
        renderGraphics(WALL_H_3, Vector2Add(Vector2Add(Vector2Add(pos, Vector2Add(RIGHT, DOWN)), DOWN), DOWN), scale);
    }
}

// renders a room with open doors left and right
void draw_room_lrd(Vector2 pos, float scale){
    renderGraphics(ROOM | DOOR_LEFT_OPEN | DOOR_RIGHT_OPEN, pos, scale);
}

void initGraphics(){
    graphics.tile_image = LoadImage("assets/character and tileset/Dungeon_Tileset.png");
    graphics.tile_texture = LoadTextureFromImage(graphics.tile_image);
}

void unloadGraphics(){
    UnloadImage(graphics.tile_image);
    UnloadTexture(graphics.tile_texture);
}

// enlarging the src rectangle to fit the screen
void scaleRec(Rectangle& rec, float scale){
    rec.x *= scale;
    rec.y *= scale;
    rec.width *= scale;
    rec.height *= scale;
}
