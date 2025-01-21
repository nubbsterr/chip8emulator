#ifndef CHIP8DATA
#define CHIP8DATA

#include <random>  // for random number generation
#include <chrono>  // for system clock, to seed PRNG
#include <cstdint> // for fixed width ints below
#include <fstream> // for file IO with ROM files

class Chip8
{
public:
    Chip8();                    // Declare the constructor IN the constructor so we can modify its parameters as we wish in the .cpp file
    uint8_t registers[16]{};    // 16 8-bit registers on the chip8
    uint8_t memory[4096]{};     // 4KB of memory
    uint16_t index{};           // 16-bit Index Register, store memory address in use for operations, 16-bit to hold max 0xFFF (4095) memory address
    uint16_t pc{};              // Program Counter to point to next address of next instruction to execute
    uint16_t stack[16]{};       // 16-level stack, basically allows 16 PCs/program counters at once, more stuff can be executed
    uint8_t sp{};               // 6-bit stack pointer, increments every time we call something and decrements when we return
    uint8_t delayTimer{};       // Timer used for timing. Decrements at a rate of 60Hz
    uint8_t soundTimer{};       // Sound timer, plays a buzz/tone when non-zero, decrements at a rate of 60Hz
    uint8_t keypad[16]{};       // 16 input keys, mapped as 1234, QWER, ASDF, ZXCV for this implementation
    uint32_t video[64 * 32]{};  // 64*32 pixel monitor, every pixel runs off a XOR implementation. A sprite that is loaded to a pixel will be shown as on, but anything overlapping it will be turned off. See XOR truth tables online.
    uint16_t opcode;            // Opcode, will be used to instruct what operation to perform, here the Chip8 will take the opcode and do whatever it is instructed to do

   	std::default_random_engine randGen;              // random number engine that produces pseudo-random numbers
	std::uniform_int_distribution<uint8_t> randByte; // return uniformly distributed random integers over a given interval, returns unsigned 8-bit int

    void LoadROM(char const* filename); // load ROM file instructions into memory, starting at 0x200
};
#endif
