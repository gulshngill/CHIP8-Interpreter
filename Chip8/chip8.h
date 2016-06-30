class chip8 {
  public:
    chip8();  
    ~chip8();
    void initialize();
    bool loadApplication(const char* filename);
    void Emulate(); //emulates one cycle of the Chip 8 CPU

    unsigned char  gfx[64 * 32];	//2048 pixels

  private:  
    unsigned short pc;        // Program counter
    unsigned short opcode;      // Current opcode
    unsigned short I;       // Index register
    unsigned short sp;        // Stack pointer; to remember which level of the stack is used

    
    unsigned char  V[16];			// V-regs (V0-VF) (system contains 15 8-bit general purpose registers, 16th register is carry flag)
	unsigned short stack[16];		// Stack (16 levels); used to remember the current location before a jump is performed
	unsigned char  memory[4096];	// Memory (size = 4kb)

	//Timers (System has two timer registers that counts at 60Hz, will count down to 0)
	unsigned char  delay_timer;		// Delay timer: used for timing the events of the game 
	unsigned char  sound_timer;		// Sound timer: used for sound effects		
}

//Chip 8 memory map
//0x000-0x1FF (512) - Chip 8 interpreter (contains font set in emu) 
//0x050-0x0A0 - Used for the built in 4x5 pixel font set (0-F)
//0x200-0xFFF - Program ROM and work RAM