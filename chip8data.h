#ifndef CHIP8DATA
#define CHIP8DATA

#include <random>  // for random number generation
#include <chrono>  // for system clock, to seed PRNG
#include <cstdint> // for fixed width ints below
#include <fstream> // for file IO with ROM files
#include <cstring> // for memset function in 00E0 opcode

class Chip8
{
public:
    Chip8();                    // Declare the constructor IN the constructor so we can modify its parameters as we wish in the .cpp file
    uint8_t registers[16]{};    // 16 8-bit registers on the chip8
    uint8_t memory[4096]{};     // 4KB of memory
    uint16_t index{};           // 16-bit Index Register, store memory address in use for operations, 16-bit to hold max 0xFFF (4095) memory address
    uint16_t pc{};              // Program Counter to point to next address of next instruction to execute
    uint16_t stack[16]{};       // 16-level stack, basically allows 16 PCs/program counters at once, more stuff can be executed
    uint8_t sp{};               // 8-bit stack pointer, increments every time we call something and decrements when we return
    uint8_t delayTimer{};       // Timer used for timing. Decrements at a rate of 60Hz
    uint8_t soundTimer{};       // Sound timer, plays a buzz/tone when non-zero, decrements at a rate of 60Hz
    uint8_t keypad[16]{};       // 16 input keys, mapped as 1234, QWER, ASDF, ZXCV for this implementation
    uint32_t video[64 * 32]{};  // 64*32 pixel monitor, every pixel runs off a XOR implementation. A sprite that is loaded to a pixel will be shown as on, but anything overlapping it will be turned off. See XOR truth tables online.
    uint16_t opcode;            // Opcode, will be used to instruct what operation to perform, here the Chip8 will take the opcode and do whatever it is instructed to do

   	std::default_random_engine randGen;              // random number engine that produces pseudo-random numbers
	std::uniform_int_distribution<uint8_t> randByte; // return uniformly distributed random integers over a given interval, returns unsigned 8-bit int

    void LoadROM(char const* filename); // load ROM file instructions into memory, starting at 0x200

private:
    void OP_00E0(); // CLS, Clear Screen Opcode, set video memory buffer entirely to 0
    void OP_00EE(); // RET, Return from a Subroutine, decrement sp and set pc equal to stack[sp], last instruction done before subroutine
    void OP_1nnn(); // JP addr, Jump to location 'nnn', sets pc to no stack interaction needed,
    void OP_2nnn(); // CALL addr, Call subroutine at 'nnn'
    void OP_3xkk(); // SE Vx, byte, skip next instruction if val in register Vx = byte
    void OP_4xkk(); // SNE Vx, byte, skip next instruction if val in register Vx != byte
    void OP_5xy0(); // SE Vx, Vy, skip next instruction if val in register Vx = val in register Vx
    void OP_6xkk(); // LD Vx, byte, load value of byte into Vx register
    void OP_7xkk(); // ADD Vx, byte, add values of byte and Vx, store result in register Vx
    void OP_8xy0(); // LD Vx, Vy, load value in register Vy into register Vx
    void OP_8xy1(); // OR Vx, Vy, perform bitwise OR on values in register Vy and register Vx, store result in register Vx
    void OP_8xy2(); // AND Vx, Vy, perform bitwise AND on values in register Vy and register Vx, store result in register Vx
    void OP_8xy3(); // XOR Vx, Vy, perform bitwise XOR on values in register Vx and register Vy, store result in register Vx
    void OP_8xy4(); // ADD Vx, Vy, add values in register Vx and register Vy and store result in register Vx. If result is greater than 256/8-bit limit, set register VF to 1 as an overflow/carry flag
    void OP_8xy5(); // SUB Vx, Vy, subtract values in register Vx and register Vy and store result in register Vx.
    void OP_8xy6(); // SHR Vx, perform right shift on value in regsiter Vx and save least significant bit in register VF.
    void OP_8xy7(); // SUBN Vx, Vy, subtract value in register Vy from register Vx and store result in register Vx. Set carry flag to 1 in VF if Vy > Vx.
    void OP_8xyE(); // SHL Vx, perform left shift on value in register Vx and save most significant bit in register VF.

};
#endif
