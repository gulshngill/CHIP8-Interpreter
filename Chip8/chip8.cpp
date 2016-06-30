unsigned char fontset[80] =
{ 
    0xF0, 0x90, 0x90, 0x90, 0xF0, //0
    0x20, 0x60, 0x20, 0x20, 0x70, //1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, //2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, //3
    0x90, 0x90, 0xF0, 0x10, 0x10, //4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, //5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, //6
    0xF0, 0x10, 0x20, 0x40, 0x40, //7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, //8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, //9
    0xF0, 0x90, 0xF0, 0x90, 0x90, //A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, //B
    0xF0, 0x80, 0x80, 0x80, 0xF0, //C
    0xE0, 0x90, 0x90, 0x90, 0xE0, //D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, //E
    0xF0, 0x80, 0xF0, 0x80, 0x80  //F
};


void chip8::initialize()
{
  pc     = 0x200;  // Program counter starts at 0x200 because system expects application to be loaded at this memory location
  opcode = 0;      // Reset current opcode	
  I      = 0;      // Reset index register
  sp     = 0;      // Reset stack pointer 
   
  // Clear display (0 = black; 1 = whitle)
  for(int i = 0; i < 2048; ++i)
    gfx[i] = 0;

  // Clear stack
  for(int i = 0; i < 16; ++i)
    stack[i] = 0;
  
  // Clear registers V0-VF
  for(int i = 0; i < 16; ++i)
    key[i] = V[i] = 0;

  // Clear memory 
  for(int i = 0; i < 4096; ++i)
    memory[i] = 0;
   
  // Load fontset (fontset should be loaded in memory location 0x50 == 80 and onwards)
  for(int i = 0; i < 80; ++i)
    memory[i] = fontset[i];		
   
  // Reset timers
  delay_timer = 0;
  sound_timer = 0;
}

bool chip8::loadApplication(const char * filename)
{
  //reset
  initialize();
    
  // Open file
  printf("Loading: %s\n", filename);
  FILE * pFile = fopen(filename, "rb"); //open file as a binary file
  if (pFile == NULL)
  {
    fputs ("File error", stderr); 
    return false;
  }

  //Get file size
  fseek(pFile, 0, SEEK_END); //put position indicator to the end
  long size = ftell(pFile); //returns current value of position indicator 
  rewind(pFile); //return position indicator back to beginning
  printf("Filesize: %d\n", (int)lSize); 

  //Allocate memory to store file
  char * fileMem = (char*)malloc(sizeof(char) * lSize); //it is stored in binary and each digit is a char
  if(fileMem == NULL) {
    fputs ("Memory error", stderr);
    return false;
  } 

  //store file in fileMem
  size_t result = fread(fileMem, 1, lSize, pFile); //(target, size(byte)/element, no. of ele, inputstream)
  if(return != lSize) {
    fputs("Error", stderr);
    return false;
  }

  //copy to chip8 memory 
  if((4096-512) > lSize) {
    for (int i = 0; i < lSize; ++i) {
      memory[i+512] = fileMem[i];
    }
  } else {
    fputs("File too large", stderr);
  }

  fclose(pFile);
  free(fileMem);

  return true;
}

void chip8::Emulate() {
  //fetch opcode
  opcode = memory[pc] << 8 | memory[pc+1]; //merge the two adjacent addresses to get opcode

  //decode opcode
  //execute opcode
  switch(opcode & 0xF000) { //opcode & 0xF000 to get first digit of opcode
    case 0x0000: 
      switch(opcode & 0x00FF) { //get last two digits
        //00E0 & 00EE
        case 0x00E0: //clears the screen
          for(int i = 0; i < 2048; ++i)
            gfx[i] = 0;
          pc += 2;
          break;
        case 0x00EE: //Returns from a subroutine
          --sp;    
          pc = stack[sp]; // Put previous address from the stack back into the program counter         
          pc += 2;    //Increase by 2 becasue each instruction is 2 bytes long
          break;
      }
      break;
    case 0x1000: //Jumps to address NNN.
      pc = opcode & 0x0FFF;
      break;
    case 0x2000: //Calls subroutine at NNN
      stack[sp] = pc; //store current address in stack
      ++sp; //increment stack pointer by 1
      pc = opcode & 0x0FFF;
      break;
    case 0x3000: //3XNN - Skips the next instruction if VX equals NN
      if(V[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF))
        pc += 4;
      else
        pc += 2;
      break;
    case 0x4000: //4XNN - Skips the next instruction if VX doesn't equal NN
      if(V[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF))
        pc += 4;
      else
        pc += 2;
      break;
    case 0x5000: //5XY0 - Skips the next instruction if VX equals VY
      if(V[(opcode & 0x0F00) >> 8] == V[(opcode & 0x00F0) >> 4]) 
        pc += 4;
      else
        pc +=2;
      break;
    case 0x6000: //6XNN - Sets VX to NN
      V[(opcode & 0x0F00) >> 8] = opcode & 0x00FF;
      pc += 2;
      break;
    case 0x7000: //// 0x7XNN - Adds NN to VX
      V[(opcode & 0x0F00) >> 8] += opcode & 0x00FF;
      pc += 2;
      break;
    case 0x8000:
      switch(opcode & 0x000F) {
        case 0x0000: //8XY0 - Sets VX to the value of VY
          V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4];
          pc += 2;
          break;
        case 0x0001: //8XY1 - Sets VX to VX or VY
          V[(opcode & 0x0F00) >> 8] |= V[(opcode & 0x00F0) >> 4];
          pc += 2;
          break;
        case 0x0002: //8XY2 - Sets VX to VX and VY.
          V[(opcode & 0x0F00) >> 8] &= V[(opcode & 0x00F0) >> 4];
          pc += 2;
          break;
        case 0x0003: //8XY3 - Sets VX to VX xor VY.
          V[(opcode & 0x0F00) >> 8] ^= V[(opcode & 0x00F0) >> 4];
          pc += 2;
          break;
        case 0x0004: //8XY4 - Adds VY to VX. VF is set to 1 when there's a carry, and to 0 when there isn't (VF = carry flag)
          V[(opcode & 0x0F00) >> 8] += V[(opcode & 0x00F0) >> 4]; //add VY to VX

          if(V[(opcode & 0x0F00) >> 8] > 0xFF) //if its larger than 2bits
            V[0xF] = 1; //carry
          else 
            V[0xF] = 0; 

          pc += 2;
          break; 
        case 0x0005: //8XY5 - VY is subtracted from VX. VF is set to 0 when there's a borrow, and 1 when there isn't.
          if(V[(opcode & 0x00F0) >> 4] > V[(opcode & 0x0F00) >> 8]) //borrow happens whem VY > VX
            V[0xF] = 0;
          else 
            V[0xF] = 1;

          V[(opcode & 0x0F00) >> 8] -= V[(opcode & 0x00F0) >> 4];

          pc += 2;
          break;
        case 0x0006: //8XY6 Shifts VX right by one. VF is set to the value of the least significant bit of VX before the shift
          V[0xF] = V[(opcode & 0x0F00) >> 8] & 0x1; //get least significant bit (before shifting)
          V[(opcode & 0x0F00) >> 8] >>= 1; //shift right by 1

          pc +=2;
          break;
        case 0x0007: //8XY7 - Sets VX to VY minus VX. VF is set to 0 when there's a borrow, and 1 when there isn't.
          if(V[(opcode & 0x0F00) >> 8] > V[(opcode & 0x00F0) >> 4]) //if VX > VY
            V[0xF] = 0; // there is a borrow
          else
            V[0xF] = 1;
          V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4] - V[(opcode & 0x0F00) >> 8];
          pc += 2;
          break;
        case 0x000E: //8XYE - Shifts VX left by one. VF is set to the value of the most significant bit of VX before the shift
          V[0xF] = V[(opcode & 0x0F00) >> 8] & 0x1000;
          V[(opcode & 0x0F00) >> 8] <<= 1; 
          pc+=2;
          break;

        default:
          printf ("Unknown opcode [0x8000]: 0x%X\n", opcode);
      }
      break;
    case 0x9000: //9XY0 - Skips the next instruction if VX doesn't equal VY
      if(V[(opcode & 0x0F00) >> 8] != V[(opcode & 0x00F0) >> 4])
        pc += 4;
      else
        pc += 2;
      break;
    case 0xA000: //ANNN - Sets I to the address NNN
      I = opcode & 0x0FFF;
      pc += 2;
      break;
    case 0xB000: //BNNN - Jumps to the address NNN plus V0
      pc = (opcode & 0x0FFF) + V[0];
      break;
    case 0xC000: // CXNN - Sets VX to the result of a bitwise and operation on a random number and NN
      srand (time(NULL));
      V[(opcode & 0x0F00) >> 8] = (rand % 0xFF) & (opcode & 0x00FF)
      pc += 2;
      break;
    case 0xD000: //DXYN - responsible for drawing to display
      //The interpreter reads 'n' bytes from memory, starting at the address stored in I. 
      //These bytes are then displayed as sprites on screen at coordinates (Vx, Vy). Sprites are XORed onto the existing screen. 
      //If this causes any pixels to be erased, VF is set to 1, otherwise it is set to 0. 
      //If the sprite is positioned so part of it is outside the coordinates of the display, it wraps around to the opposite side of the screen. 
      // If the current value is different from the value in the memory, the bit value will be 1. If both values match, the bit value will be 0
      {
        unsigned short x = V[(opcode & 0x0F00) >> 8]; //VX
        unsigned short y = V[(opcode & 0x00F0) >> 4]; //VY
        unsigned short height = opcode & 0x000F; //N
        unsigned short pixel;

        V[0xF] = 0; //reset VF back to 0
        for(int y-axis = 0; y < height; y++) { //navigate row by row
          pixel = memory[I + y-axis]; //Fetch the pixel value from the memory starting at location I (one memory address represents one row; 1 row is 8 bits) 
          for(int x-axis = 0; x-axis < 8; x-axis++) { //go trough each bit
            if((pixel & (0x80 >> x-axis)) >> 8 != 0) { //if pixel in memory is 1 
              unsigned short totalX = x + x-axis;
              unsigned short totalY = y + y-axis;
              unsigned short index = totalX + (totalY * 64)

              if(gfx[index] == 1) { //if on screen and memory == 1
                V[0xF] = 1; //Set VF to 1 (turn on collision detection)                                   
              } 
              gfx[index] ^= 1; //XORed onto the existing screen
            }
            else 

          }
        }
        pc += 2;
      }
      break;
    case 0xE000: 
      switch(opcode & 0x00FF) {
        case 0x009E: // EX9E - Skips the next instruction if the key stored in VX is pressed
          pc +=2;
          break;
        case 0x00A1: //EXA1 - Skips the next instruction if the key stored in VX isn't pressed
          pc += 2;
          break;
        default:
          printf ("Unknown opcode [0xE000]: 0x%X\n", opcode);
      }
      break;
    case 0xF000:
      switch(opcode & 0x00FF) {
        case 0x0007:
          break;
        case 0x0015:
          break;
        case 0x0018:
          break;
        case 0x001E:
          break;
        case 0x0029:
          break;
        case 0x0033:
          break;
        case 0x0055:
          break;
        case 0x0065:
          break;
        default:
          printf ("Unknown opcode [0xF000]: 0x%X\n", opcode);
      }
      break;
  } 
}