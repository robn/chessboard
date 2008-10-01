#include <stdio.h>
#include <nds.h>

u8 blank_tile[64] = {
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0
};
 
u8 yellow_tile[64] = {
    1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1
};
 
u8 blue_tile[64] = {
    2,2,2,2,2,2,2,2,
    2,2,2,2,2,2,2,2,
    2,2,2,2,2,2,2,2,
    2,2,2,2,2,2,2,2,
    2,2,2,2,2,2,2,2,
    2,2,2,2,2,2,2,2,
    2,2,2,2,2,2,2,2,
    2,2,2,2,2,2,2,2
};

#define ABS(n) ({ int _n = (n); _n < 0 ? -_n : _n; })

#define DO_TILE(x,y,z) ({ int _x = (x), _y = (y), _z = (z); ((_x/_z) & 1) ^ ((_y/_z) & 1) ^ (_x < 0) ^ (_y < 0); })

void draw_board(u16 *map, int tile, int zoom, int anchor) {
    int x, y;

    switch (anchor) {

        /* centre */
        case 0:
            for (y = 0; y < 24; y++)
                for (x = 0; x < 32; x++) {
                    int p = DO_TILE(16-zoom+1-x, 12-zoom+1-y, zoom);
                    map[y*32+x] = !p ? 0 : (x == 16 || y == 12) ? 1 : 2;
                    //map[y*32+x] = DO_TILE(16-x, 12-y, 2) * tile;
                }
            break;

        /* top left */
        case 1:
            for (y = 0; y < 24; y++)
                for (x = 0; x < 32; x++)
                    map[y*32+x] = DO_TILE(x, y, zoom) * tile;
            break;

        /* top right */
        case 2:
            for (y = 0; y < 24; y++)
                for (x = 0; x < 32; x++)
                    map[y*32+x] = DO_TILE(31-x, y, zoom) * tile;
            break;

        /* bottom right */
        case 3:
            for (y = 0; y < 24; y++)
                for (x = 0; x < 32; x++)
                    map[y*32+x] = DO_TILE(31-x, 23-y, zoom) * tile;
            break;

        /* bottom left */
        case 4:
            for (y = 0; y < 24; y++)
                for (x = 0; x < 32; x++)
                    map[y*32+x] = DO_TILE(x, 23-y, zoom) * tile;
            break;
    }
}
 
int main(void) {
    irqInit();
    irqEnable(IRQ_VBLANK);

    //set video mode and map vram to the background
    videoSetMode(MODE_0_2D | DISPLAY_BG0_ACTIVE | DISPLAY_BG1_ACTIVE);
    vramSetBankA(VRAM_A_MAIN_BG_0x06000000);
 
    //get the address of the tile and map blocks 
    u8 *tile = (u8 *) BG_TILE_RAM(1);
    u16 *map0 = (u16 *) BG_MAP_RAM(0);
    u16 *map1 = (u16 *) BG_MAP_RAM(1);
    
    //tell the DS where we are putting everything and set 256 color mode and that we are using a 32 by 32 tile map.
    BG0_CR = BG_32x32 | BG_COLOR_256 | BG_MAP_BASE(0) | BG_TILE_BASE(1);
    BG1_CR = BG_32x32 | BG_COLOR_256 | BG_MAP_BASE(1) | BG_TILE_BASE(1);
 
    videoSetModeSub(MODE_0_2D | DISPLAY_BG0_ACTIVE);
    vramSetBankC(VRAM_C_SUB_BG);
    SUB_BG0_CR = BG_MAP_BASE(31);
    BG_PALETTE_SUB[255] = RGB15(0x1f, 0x1f, 0x1f);
    consoleInitDefault((u16 *) SCREEN_BASE_BLOCK_SUB(0x1f), (u16 *) CHAR_BASE_BLOCK_SUB(0), 16);

    //load our palette
    BG_PALETTE[0] = RGB15(0,0,0);
    BG_PALETTE[1] = RGB15(0x1b,0x1b,0x3);
    BG_PALETTE[2] = RGB15(0x3,0x3,0x1b);
  
    //copy the tiles into tile memory one after the other
    swiCopy(blank_tile, tile, 32);
    swiCopy(yellow_tile, tile + 64, 32);
    swiCopy(blue_tile, tile + 128, 32);
    
    //create a map in map memory
    /*
    for (y = 0; y < 24; y++)
        for (x = 0; x < 32; x++) {
            map0[y*32+x] = ((x & 2) >> 1) ^ ((y & 2) >> 1);
            map1[y*32+x] = (((x & 2) >> 1) ^ (((y+2) & 2) >> 1)) << 1;
        }
    */

    int z = 1, dir = 0, n = 4;
    while (1) {
        swiWaitForVBlank();
        if (--n)
            continue;
        n = 4;

        draw_board(map0, 1, z, 0);
        //draw_board(map1, 2, z, 2);

        if (dir == 0)
            z++;
        else
            z--;

        if (z == 16)
            dir = 1;
        else if (z == 1)
            dir = 0;
    }
 
    return 0;
}
