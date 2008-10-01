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

    videoSetMode(MODE_0_2D | DISPLAY_BG0_ACTIVE | DISPLAY_BG1_ACTIVE | DISPLAY_BG2_ACTIVE | DISPLAY_BG3_ACTIVE);
    vramSetBankA(VRAM_A_MAIN_BG_0x06000000);
 
    u8 *tile = (u8 *) BG_TILE_RAM(1);
    u16 *map0 = (u16 *) BG_MAP_RAM(0);
    u16 *map1 = (u16 *) BG_MAP_RAM(1);
    u16 *map2 = (u16 *) BG_MAP_RAM(2);
    u16 *map3 = (u16 *) BG_MAP_RAM(3);
    
    BG0_CR = BG_32x32 | BG_COLOR_256 | BG_MAP_BASE(0) | BG_TILE_BASE(1) | BG_PRIORITY_3;
    BG1_CR = BG_32x32 | BG_COLOR_256 | BG_MAP_BASE(1) | BG_TILE_BASE(1) | BG_PRIORITY_2;
    BG2_CR = BG_32x32 | BG_COLOR_256 | BG_MAP_BASE(2) | BG_TILE_BASE(1) | BG_PRIORITY_1;
    BG3_CR = BG_32x32 | BG_COLOR_256 | BG_MAP_BASE(3) | BG_TILE_BASE(1) | BG_PRIORITY_0;
 
    videoSetModeSub(MODE_0_2D | DISPLAY_BG0_ACTIVE);
    vramSetBankC(VRAM_C_SUB_BG);
    SUB_BG0_CR = BG_MAP_BASE(31);
    BG_PALETTE_SUB[255] = RGB15(0x1f, 0x1f, 0x1f);
    consoleInitDefault((u16 *) SCREEN_BASE_BLOCK_SUB(0x1f), (u16 *) CHAR_BASE_BLOCK_SUB(0), 16);

    BG_PALETTE[0] = RGB15(0,0,0);
    BG_PALETTE[1] = RGB15(0x1b,0x1b,0x3);
    BG_PALETTE[2] = RGB15(0x3,0x3,0x1b);
    BG_PALETTE[3] = RGB15(0x1b,0x3,0x3);
    BG_PALETTE[4] = RGB15(0x3,0x1b,0x3);

    int i;
    for (i = 0; i < 64; i++) {
        tile[i] = 0;
        tile[i+64] = 1;
        tile[i+128] = 2;
        tile[i+192] = 3;
        tile[i+256] = 4;
    }

    int z0 = 0, z1 = 8, z2 = 16, z3 = 24;
    int n = 4;
    while (1) {
        swiWaitForVBlank();
        if (--n)
            continue;
        n = 4;

        if (z0 < 32)
            draw_board(map0, 1, z0 + 1, 0);
        if (z1 < 32)
            draw_board(map1, 2, z1 + 1, 0);
        if (z2 < 32)
            draw_board(map2, 3, z2 + 1, 0);
        if (z3 < 32)
            draw_board(map3, 4, z3 + 1, 0);

        z0 = (z0 + 1) % 32;
        z1 = (z1 + 1) % 32;
        z2 = (z2 + 1) % 32;
        z3 = (z3 + 1) % 32;

        if (!z0 || !z1 || !z2 || !z3) {
            int pt = BG0_CR & 0x3;
            BG0_CR = (BG0_CR & 0xfffc) | (BG1_CR & 0x3);
            BG1_CR = (BG1_CR & 0xfffc) | (BG2_CR & 0x3);
            BG2_CR = (BG2_CR & 0xfffc) | (BG3_CR & 0x3);
            BG3_CR = (BG3_CR & 0xfffc) | (pt & 0x3);
        }
    }
 
    return 0;
}
