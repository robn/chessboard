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
 
u8 red_tile[64] = {
    1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1
};
 
u8 green_tile[64] = {
    2,2,2,2,2,2,2,2,
    2,2,2,2,2,2,2,2,
    2,2,2,2,2,2,2,2,
    2,2,2,2,2,2,2,2,
    2,2,2,2,2,2,2,2,
    2,2,2,2,2,2,2,2,
    2,2,2,2,2,2,2,2,
    2,2,2,2,2,2,2,2
};
 
void draw_board(u16 *map, int tile, int zoom) {
    int x, y;

    for (y = 0; y < 24; y++)
        for (x = 0; x < 32; x++)
            map[y*32+x] = ((x & zoom) ? 1 : 0) ^ ((y & zoom) ? 1 : 0);
}
 
int main(void) {
    irqInit();
    irqEnable(IRQ_VBLANK);

    //set video mode and map vram to the background
    videoSetMode(MODE_0_2D | DISPLAY_BG0_ACTIVE);
    //videoSetMode(MODE_0_2D | DISPLAY_BG0_ACTIVE | DISPLAY_BG1_ACTIVE);
    vramSetBankA(VRAM_A_MAIN_BG_0x06000000);
 
    //get the address of the tile and map blocks 
    u8 *tile = (u8 *) BG_TILE_RAM(1);
    u16 *map0 = (u16 *) BG_MAP_RAM(0);
    //u16 *map1 = (u16 *) BG_MAP_RAM(1);
    
    //tell the DS where we are putting everything and set 256 color mode and that we are using a 32 by 32 tile map.
    BG0_CR = BG_32x32 | BG_COLOR_256 | BG_MAP_BASE(0) | BG_TILE_BASE(1);
    //BG1_CR = BG_32x32 | BG_COLOR_256 | BG_MAP_BASE(1) | BG_TILE_BASE(1);
 
    //load our palette
    BG_PALETTE[0] = RGB15(0,0,0);
    BG_PALETTE[1] = RGB15(0x1b,0x1b,0x3);
    BG_PALETTE[2] = RGB15(0x3,0x3,0x1b);
  
    //copy the tiles into tile memory one after the other
    swiCopy(blank_tile, tile, 32);
    swiCopy(red_tile, tile + 64, 32);
    swiCopy(green_tile, tile + 128, 32);
    
    //create a map in map memory
    /*
    for (y = 0; y < 24; y++)
        for (x = 0; x < 32; x++) {
            map0[y*32+x] = ((x & 2) >> 1) ^ ((y & 2) >> 1);
            map1[y*32+x] = (((x & 2) >> 1) ^ (((y+2) & 2) >> 1)) << 1;
        }
    */

    int z = 1, dir = 0, n = 10;
    while (1) {
        swiWaitForVBlank();
        if (--n)
            continue;
        n = 10;

        draw_board(map0, 1, z);

        if (dir == 0)
            z <<= 1;
        else
            z >>= 1;

        if (z == 16)
            dir = 1;
        else if (z == 1)
            dir = 0;
    }
 
    return 0;
}
