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
  size_t result = fread(fileMem, 1, lSize, pFile);
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
