#include <stdio.h>
#include <nds.h>

#define TILE_AT(x,y,z) ({ int _x = (x), _y = (y), _z = (z); (((_x + (_x < 0)) / _z) & 1) ^ (((_y + (_y < 0)) / _z) & 1) ^ (_x < 0) ^ (_y < 0); })

void draw_board(u16 *map, int tile, int zoom, int anchor) {
    int x, y;

    switch (anchor) {

        /* centre */
        case 0:
            for (y = 0; y < 24; y++)
                for (x = 0; x < 32; x++) {
                    map[y*32+x] = TILE_AT((15 - (zoom >> 1) - x), (11 - (zoom >> 1) - y), zoom) * tile;
                }
            break;

        /* top left */
        case 1:
            for (y = 0; y < 24; y++)
                for (x = 0; x < 32; x++)
                    map[y*32+x] = TILE_AT(x, y, zoom) * tile;
            break;

        /* top right */
        case 2:
            for (y = 0; y < 24; y++)
                for (x = 0; x < 32; x++)
                    map[y*32+x] = TILE_AT(31-x, y, zoom) * tile;
            break;

        /* bottom right */
        case 3:
            for (y = 0; y < 24; y++)
                for (x = 0; x < 32; x++)
                    map[y*32+x] = TILE_AT(31-x, 23-y, zoom) * tile;
            break;

        /* bottom left */
        case 4:
            for (y = 0; y < 24; y++)
                for (x = 0; x < 32; x++)
                    map[y*32+x] = TILE_AT(x, 23-y, zoom) * tile;
            break;
    }
}
 
int main(void) {
    irqInit();
    irqEnable(IRQ_VBLANK);

    videoSetMode(MODE_0_2D | DISPLAY_BG0_ACTIVE | DISPLAY_BG1_ACTIVE);
    vramSetBankA(VRAM_A_MAIN_BG_0x06000000);
 
    u8 *tile = (u8 *) BG_TILE_RAM(1);
    u16 *map0 = (u16 *) BG_MAP_RAM(0);
    u16 *map1 = (u16 *) BG_MAP_RAM(1);
    
    BG0_CR = BG_32x32 | BG_COLOR_256 | BG_MAP_BASE(0) | BG_TILE_BASE(1);
    BG1_CR = BG_32x32 | BG_COLOR_256 | BG_MAP_BASE(1) | BG_TILE_BASE(1);
 
    videoSetModeSub(MODE_0_2D | DISPLAY_BG0_ACTIVE);
    vramSetBankC(VRAM_C_SUB_BG);
    SUB_BG0_CR = BG_MAP_BASE(31);
    BG_PALETTE_SUB[255] = RGB15(0x1f, 0x1f, 0x1f);
    consoleInitDefault((u16 *) SCREEN_BASE_BLOCK_SUB(0x1f), (u16 *) CHAR_BASE_BLOCK_SUB(0), 16);

    BG_PALETTE[0] = RGB15(0,0,0);
    BG_PALETTE[1] = RGB15(0x1b,0x1b,0x3);
    BG_PALETTE[2] = RGB15(0x3,0x3,0x1b);

    int i;
    for (i = 0; i < 64; i++) {
        tile[i] = 0;
        tile[i+64] = 1;
        tile[i+128] = 2;
    }

    int z = 1, n = 4;
    while (1) {
        swiWaitForVBlank();
        if (--n)
            continue;
        n = 4;

        draw_board(map0, 1, z, 0);

        z++;
        if (z == 33)
            break;
    }
 
    return 0;
}
