class chip8 {
  public:
    chip8();  
    ~chip8();
    void initialize();

    unsigned char  gfx[64 * 32];	//2048 pixels

  private:  
    unsigned short pc;        // Program counter
    unsigned short opcode;      // Current opcode
    unsigned short I;       // Index register
    unsigned short sp;        // Stack pointer

    
    unsigned char  V[16];			// V-regs (V0-VF)
	unsigned short stack[16];		// Stack (16 levels)
	unsigned char  memory[4096];	// Memory (size = 4k)

	//Timers (System has two timer registers that counts at 60Hz, will count down to 0)
	unsigned char  delay_timer;		// Delay timer: used for timing the events of the game 
	unsigned char  sound_timer;		// Sound timer: used for sound effects		
}