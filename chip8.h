/* ____       '   __        _ __
* |      |  | |  |__|      |    |
* |      |--| |  |         |----|
* |____  |  | |  |         |_ __|
* 
* CHIP-8 Emulator Header
*/
#define Display_WIDTH 64
#define Display_HEIGHT  32
#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 320
#define PIXEL 64*32
#define KEYN 16

unsigned char KEYMAP[16] = {
    SDLK_1, //0
    SDLK_2, //1
    SDLK_3, //2
    SDLK_c, //3
    SDLK_4, //4
    SDLK_5, //5
    SDLK_6, //6
    SDLK_d, //7
    SDLK_7, //8
    SDLK_8, //9
    SDLK_9, //A
    SDLK_e, //B
    SDLK_a, //C
    SDLK_0, //D
    SDLK_b, //E
    SDLK_f  //F
};
unsigned int keystate[16];
unsigned char font[80] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};
/*Variaveis Graficas*/
SDL_Renderer *renderer = NULL;
SDL_Texture  *texture;
SDL_Surface *chip8sec = NULL;
SDL_Surface *chip8win = NULL; 
SDL_Event event;
SDL_Rect rect;
SDL_Window *window = NULL;
unsigned int drawing;
/*Variaveis Graficas*/
uint8_t display[Display_WIDTH*Display_HEIGHT];       //Display que tem 2048 pixels
unsigned char memory[0xFFF]; // 4Kilobytes de memoria ram
unsigned char V[16];         //general-purpose variable registers numbered 0 through F hexadecimal, 
                             //ie. 0 through 15 in decimal, called V0 through VF.
unsigned char delay_timer;   //which is decremented at a rate of 60 Hz (60 times per second) until it reaches 0
unsigned char sound_timer;   //which functions like the delay timer, but which also gives off a beeping sound as long as it’s not 0

unsigned short pc;           //Program counter aponta para a instrucao atual na memoria
unsigned short I;            //Index Reigster usado para apontar para endereços de memoria
unsigned short stack[16];    //which is used to call subroutines/functions and return from them
unsigned short sp;           //localizador stackpointer

unsigned char  rx;          //rx onde esta localizado registrador VX
unsigned char  ry;          //ry onde esta localizado registrador VY
unsigned char  N;           //N um numero de 4 bits
unsigned short NN;          //NN um numero de 8 bits
unsigned int   NNN;         //NNN um numero de 12 bits ou endereço na memoria

/*OPCODES*/
void i00E0();
void i1000();
void i2000();
void i00EE();
void i3000();
void i4000();
void i5000();
void i6000();
void i7000();
void i9000();
void i8000();
void i8001();
void i8002();
void i8003();
void i8004();
void i8005();
void i8007();
void i8006();
void i800E();
void iA000();
void iC000();
void iD000();
void iE00E();
void iE001();
void iF033();
void iF007();
void iF00A();
void iF01E();
void iF015();
void iF018();
void iF029();
void iF033();
void iF055();
/*OPCODES*/

void draw_screen();
void CPU_cycle();
void CPU_Init();
void Video_Init();
void UpdateVideo();
void LoadFont();
void LoadGame(uint8_t *);
void UpdateGraphics();