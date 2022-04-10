#include <stdio.h>
#include <string.h>
#include <SDL2/SDL.h>
#include "chip8.h"
#define DEBUG
void CPU_Cycle(uint16_t opcode){
	
	char stop;
	opcode = (memory[pc]<< 8) | memory[pc+1];	//carrega o opcode e os dados 
	rx 	   = (opcode & 0x0F00) >> 8;			//carrega o localizacao do registrador VX e 0x0400 >> 8 = 0x0004
	ry     = (opcode & 0x00F0) >> 4;			//carrega o localizacao do registrador VY e 0x0040 >> 4 = 0x0004
	N 	   = (opcode & 0x000F);					//carrega o valor de N
	NN	   = (opcode & 0x00FF);					//carrega o valor de NN
	NNN	   = (opcode & 0x0FFF);					//carrega o valor de NNN
	pc += 2;
#ifdef DEBUG
	printf("OPCODE: %x\nVX: %x	VY: %x 	 N: %x 	 NN: %x    NNN: %x\nMemory[%x]: %x 	- I: %x\nSTACK[%x]: %x	 SP: %x\n--------------------------\n",
		opcode, V[rx], V[ry], N, NN, NNN, I, memory[I], I, sp, stack[sp], sp);
#endif
	switch(opcode & 0xF000){
		case 0x0000:
			switch(opcode & 0x00FF){
				case 0x00E0://Limpa a Tela
					i00E0();
					break;
				case 0x00EE:
					i00EE();
					break;
			}
			break;
		case 0x1000: //Jumps to address NNN.
			i1000();
			break;
		case 0x2000: //Calls subroutine at NNN and store return. 
			i2000();
			break;
		case 0x3000://Skips the next instruction if VX equals NN. (Usually the next instruction is a jump to skip a code block); 
			i3000();
			break;
		case 0x4000://Skips the next instruction if VX does not equal NN. (Usually the next instruction is a jump to skip a code block); 
			i4000();
			break;
		case 0x5000://Skips the next instruction if VX equals VY. (Usually the next instruction is a jump to skip a code block); 
			i5000();
			break;
		case 0x6000://Sets VX to NN. 
			i6000();
			break;
		case 0x7000:
			i7000();//Adds NN to VX. (Carry flag is not changed);
			break;
		case 0x8000:
			switch(opcode & 0x000F){
				case 0x0000:
					i8000();
					break;
				case 0x0001:
					i8001();
					break;
				case 0x0002:
					i8002();
					break;
				case 0x0003:
					i8003();
					break;
				case 0x0004:
					i8004();
					break;
				case 0x0005:
					i8005();
					break;
				case 0x0006:
					i8006();
					break;
				case 0x0007:
					i8007();
					break;
				case 0x000E:
					i800E();
					break;

			}
			break;
		case 0x9000://Skips the next instruction if VX does not equal VY. (Usually the next instruction is a jump to skip a code block);
			i9000();
			break;
		case 0xA000:
			iA000();
			break;	
		case 0xC000:
			iC000();
			break;
		case 0xD000:
			iD000();
			break;
		case 0xE000:
			switch(opcode & 0x000F){
				case 0x000E:
					iE00E();
					break;
				case 0x0001:
					iE001();
					break;
			}
			break;
		case 0xF000:
			switch(opcode & 0x00FF){
				case 0x0007:
					iF007();
					break;
				case 0x000A:
					iF00A();
				case 0x001E:
					iF01E();
					break;
				case 0x0055:
					iF055();
					break;
				case 0x0033:
					iF033();
					break;
				case 0x0015:
					iF015();
					break;
				case 0x0018:
					iF018();
					break;
				case 0x0029:
					iF029();
					break;
			}
			break;
	}
	usleep(1850);
	//sleep(2);
}
int main(int argc, char *argv[]){
	int co = 0;
	srand(time(0));				//Prepara random number
	LoadFont();					//Carrega as fontes na memoria
	Video_Init();				//Inicia o video a janela
	CPU_Init();					//Inicia CPU, Registradores e Stack
	LoadGame(memory);			//Carrega o Game na memoria
	uint16_t opcode;	        //Segura os opcodes

	uint32_t previous_time = 0;
    uint32_t current_time, delta;
    int32_t time_accumulator;
    const float timer_rate = 1000.0f / 60.0f;
	for(;;){//Inicio do Loop (CPU Heart)
		current_time = SDL_GetTicks();
        delta = current_time - previous_time;
        previous_time = current_time;
        time_accumulator += delta;

        while(time_accumulator >= timer_rate){
            if(delay_timer){
                delay_timer--;
            }
            if(sound_timer){
                sound_timer--;
            }
            time_accumulator -= timer_rate;
        }

		SDL_PollEvent(&event);
		switch(event.type){      
            case SDL_KEYDOWN:
                for(int i = 0 ; i < KEYN ; i++){
                	if(event.key.keysym.sym == KEYMAP[i]){
                		keystate[i] = 1;
                	}
                }
                break;
           case SDL_KEYUP:
				for(int i = 0 ; i < KEYN ; i++){
					if(keystate[i]){
						keystate[i] = 0;
					}
				}
	            break;
        }
        
        CPU_Cycle(opcode);
        if(drawing){
        	UpdateGraphics();
    	}
#ifdef DEBUG
    	char stop;
    	gets(&stop);
    	if(strcmp(&stop, "adc") != -1)
    		co +=1;
    		printf("%x\n", memory[(0x200+(255+co))]);
#endif
	}//Fim do Loop(CPU Heart)
	
	return 0;
}
void UpdateGraphics(){
	//draw some stuff into the buffer
    //surface to texture
    SDL_BlitScaled(chip8sec, NULL, chip8win, NULL);			//Use this function to perform a scaled surface copy to a destination surface.
    // Update the intermediate texture with the contents of the RGBA buffer.
    SDL_UpdateTexture(texture, NULL, chip8win->pixels, chip8win->pitch);//Update the given texture rectangle with new pixel data.
    //SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);			//Copy a portion of the texture to the current rendering target.
    SDL_RenderPresent(renderer);
}
void CPU_Init(){
	pc = 0x200;
	delay_timer = 60;
	sound_timer = 60;
	memset(V, 0, sizeof(V));
	memset(keystate, 0, sizeof(keystate));
	memset(memory, 0, sizeof(memory));
	memset(display, 0, sizeof(display));
	memset(stack, 0, sizeof(stack));
	memset(&sp, 0, sizeof(sp));
	
}

void Video_Init(){//Inicia Janela, surface e texturas
	SDL_Init(SDL_INIT_VIDEO);

    window = SDL_CreateWindow(
            "Interpretor Cip8",
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            640,
            320,
            0
    );
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED); //SDL_CreateRGBSurface
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, 640, 320);
    chip8sec = SDL_CreateRGBSurface(0, 64, 32, 32, 0, 0, 0, 0);
    chip8win = SDL_CreateRGBSurface(0, 640, 320, 32, 0, 0, 0, 0);
    //SDL_RenderSetScale(renderer, 10, 10);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); //Set the color used for drawing operations (Rect, Line and Clear). https://wiki.libsdl.org/SDL_SetRenderDrawColor
    SDL_RenderClear(renderer);			    //Clear the current rendering target with the drawing color. https://wiki.libsdl.org/SDL_RenderClear
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderPresent(renderer);		    //Update the screen with any rendering performed since the previous call. https://wiki.libsdl.org/SDL_RenderPresent
}	

void LoadFont(){
	for(int i = 0; i <= 0x80 ; i++){
		memory[0x50+i] = font[i];
	}
}
void LoadGame(uint8_t *mem){
	char * dir = "SYZYGY";
	unsigned short size;
	FILE *fp = fopen(dir, "rb");
	fseek(fp, 0, SEEK_END);
	size = ftell(fp);
	printf("%d\n", size);
	rewind(fp);

	fread(&mem[0x200], 1, size, fp);

	fclose(fp);
}


/*OPCODES*/
void i00E0(){		//Clears the screen. 						
	SDL_LockSurface(chip8sec);
	uint32_t *pixels = (uint32_t *)chip8sec->pixels;
	for(int i = 0; i < 2048; i++){
		pixels[i] = 0x0;
	}
	SDL_UnlockSurface(chip8sec);
	drawing = 1;

}
void i1000(){		//Jumps to address NNN. 
	pc = NNN; 								//Chama NNN
}
void i2000(){		//Calls subroutine at NNN. And storage on stack
	sp++;
	stack[sp] = pc; 						//Guarda pc na stack
	pc = NNN;								//e chama NNN
}
void i00EE(){		//Returns from a subroutine. from stack
	pc = stack[sp];							//Coloca o valor de stack[sp] em pc
	stack[sp] = 0;							//Limpa stack onde estava o valor de pc
	sp--;									//limpa o stackpointer
}
void i3000(){		//Skips the next instruction if VX equals NN. (Usually the next instruction is a jump to skip a code block); 
	if(V[rx] == NN)
		pc += 2;							//pula a instrucao seguinte se VX == NN
}
void i4000(){		//Skips the next instruction if VX does not equal NN. (Usually the next instruction is a jump to skip a code block); 
	if(V[rx] != NN)
		pc += 2;							//pula a instrucao seguinte se VX != NN
}
void i5000(){		//Skips the next instruction if VX equals VY. (Usually the next instruction is a jump to skip a code block); 
	if(V[rx] == V[ry]) 
		pc += 2;							//pula a instrucao seguinte se VX == VY
}
void i6000(){		//Sets VX to NN.						
	V[rx] = NN;								//Coloca NN em VX
}
void i7000(){		//Adds NN to VX. (Carry flag is not changed); 
	V[rx] += NN;						
}
void i8000(){		//Sets VX to the value of VY. 
	V[rx] = V[ry];
}
void i8001(){		//Sets VX to VX or VY. (Bitwise OR operation); 
	V[rx] |= V[ry];
}
void i8002(){		//Sets VX to VX [and] VY. (Bitwise AND operation); 
	V[rx] &= V[ry];
}
void i8003(){		//Sets VX to VX [xor] VY. 
	V[rx] ^= V[ry];
}
void i8004(){		//Adds VY to VX. VF is set to 1 when there's a carry, and to 0 when there is not. 
	uint8_t overflow = (V[rx] + V[ry]) > 0xFF ? 1 : 0;	
	V[rx] += V[ry];						//se nao VX recebe VX + VY
	V[0xF] = overflow;					//VF(carryflag) se setada para ser 0
}
void i8005(){		//VY is subtracted from VX. VF is set to 0 when there's a borrow, and 1 when there is not. 
	uint8_t overflow = V[rx] >= V[ry] ? 1 : 0;
	V[rx] -= V[ry];
	V[0xF] = overflow;
}
void i8006(){		//Stores the least significant bit of VX in VF and then shifts VX to the right by 1.[b]
	uint8_t overflow = V[rx] & 0x1;
	V[rx] >>= 1;
	V[0xF] = overflow;
}
void i8007(){		//Sets VX to VY minus VX. VF is set to 0 when there's a borrow, and 1 when there is not. 
	uint8_t overflow = V[ry] >= V[rx] ? 1 : 0;
	V[rx] = V[ry] - V[rx];
	V[0xF] = overflow;
}
void i800E(){		//Stores the most significant bit of VX in VF and then shifts VX to the left by 1.[b]
	uint8_t overflow = V[rx] & 0x1;
	V[rx] <<= 1;
	V[0xF] = overflow;
}
void iA000(){		//Sets I to the address NNN. 
	I = NNN;
}
void iC000(){		//Sets VX to the result of a bitwise and operation on a random number (Typically: 0 to 255) and NN. 
	V[rx] = rand() & NN;
}
void iD000(){		//Draws a sprite at coordinate (VX, VY) that has a width of 8 pixels and a height of N pixels. Each row of 8 pixels is read as bit-coded starting from memory location I; I value does not change after the execution of this instruction. As described above, VF is set to 1 if any screen pixels are flipped from set to unset when the sprite is drawn, and to 0 if that does not happen 
	SDL_LockSurface(chip8sec);
	uint32_t *pixels = (uint32_t *)chip8sec->pixels;
	uint8_t tx = V[rx]; //sprite x
	uint8_t ty = V[ry]; //sprite y
	uint8_t height = N;
	uint8_t sprite;
	uint32_t disp;
	V[0xF] = 0;
	for(short int y = 0; y < height ; y++){
		sprite = memory[I + y];
		for (short int x = 0; x < 8; x++){
			disp = (tx+x)+(y + ty)*64;
			if((sprite & (0x80 >> x)) != 0){
				if(pixels[disp])
					V[0xF] = 1;
				pixels[disp] ^= 0xFFFFFFFF;

					//SDL_RenderDrawPoint(renderer, (tx+x)%64, (ty+y)%32);
				//printf("SPRITE: 0x%x & %d == %x\n[-------------]\nDrawing at: x:%d, y:%d\n",sprite,x,sprite &(0x80 >> x) ,(tx+x)%64, (y + ty)%32);
				
			}
			
		}
	}
	drawing = 1;
	SDL_UnlockSurface(chip8sec);

}
void i9000(){		//Skips the next instruction if VX does not equal VY. (Usually the next instruction is a jump to skip a code block); 
	if(V[rx] != V[ry])
		pc += 2;						//pula a instrucao seguinte se VX != VY
}

void iE00E(){		//Skips the next instruction if the key stored in VX is pressed. (Usually the next instruction is a jump to skip a code block); 
	if(keystate[V[rx]]){
		pc += 2;
	}
}
void iE001(){		//Skips the next instruction if the key stored in VX is not pressed. (Usually the next instruction is a jump to skip a code block); 
	if(keystate[V[rx]] == 0){
		pc += 2;
	}

}
void iF007(){		//Sets VX to the value of the delay timer. 
	V[rx] = delay_timer;
}

void iF00A(){		//A key press is awaited, and then stored in VX. (Blocking Operation. All instruction halted until next key event); 
	short int keypress = 0;

	if(!keypress){
		for(int i =0; i < KEYN; i++){
			if(keystate[i]){
				V[rx] = i;
				keypress = 1; 
			}
		}
	}

	if(keypress == 0)
		pc -= 2;
}

void iF015(){		//Sets the delay timer to VX. 
	V[rx] = delay_timer;
}
void iF018(){		//Sets the sound timer to VX. 
	V[rx] = sound_timer;
}
void iF01E(){		//Adds VX to I. VF is not affected.
	I += V[rx];
}
void iF029(){		//Sets I to the location of the sprite for the character in VX. Characters 0-F (in hexadecimal) are represented by a 4x5 font. 
	I = 0x50 + V[rx] * 5;
}
void iF033(){		//Stores the binary-coded decimal representation of VX, with the most significant of three digits at the address in I, the middle digit at I plus 1, and the least significant digit at I plus 2. (In other words, take the decimal representation of VX, place the hundreds digit in memory at location in I, the tens digit at location I+1, and the ones digit at location I+2.); 
	memory[I] = V[rx] / 100;
	memory[I + 1] = (V[rx] / 10) % 10;
	memory[I + 2] = (V[rx] % 100) % 10;
}
void iF055(){		//Stores from V0 to VX (including VX) in memory, starting at address I. The offset from I is increased by 1 for each value written, but I itself is left unmodified
	for(int i = 0; i <= rx; i++){
		memory[I + i] = V[i];
	}
}
void iF066(){		//Fills from V0 to VX (including VX) with values from memory, starting at address I. The offset from I is increased by 1 for each value written, but I itself is left unmodified
	for(int i = 0; i <= rx; i++){
		V[i] = memory[I + i];
	}
}
/*OPCODES*/

