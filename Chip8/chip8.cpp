
void chip8::initialize()
{
  pc     = 0x200;  // Program counter starts at 0x200 because system expects application to be loaded at this memory location
  opcode = 0;      // Reset current opcode	
  I      = 0;      // Reset index register
  sp     = 0;      // Reset stack pointer
   
  // Clear display
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
    memory[i] = chip8_fontset[i];		
   
  // Reset timers
  delay_timer = 0;
  sound_timer = 0;
}