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
        case 0x0004: //8XY4 - Adds VY to VX. VF is set to 1 when there's a carry, and to 0 when there isn't
          V[(opcode & 0x0F00) >> 8] |= V[(opcode & 0x00F0) >> 4];
          pc += 2;
          break; 
      }
      break;
    case 0x9000:
      break;
    case 0xA000:
      break;
    case 0xB000:
      break;
    case 0xC000:
      break;
    case 0xD000:
      break;
    case 0xE000:
      break;
    case 0xF000:
      break;
  }
}