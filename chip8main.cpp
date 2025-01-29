#include "chip8data.h"
#include <cstdint>
#include <ctime>

const unsigned int START_ADDRESS = 0x200;        // start address for Chip8, since 0x00 to 0x1FF is reserved for the Chip8
const unsigned int FONTSET_SIZE = 80;            // 16 characters used in total, each character is 5 bytes, so we have 80 bytes to use for font storage
const unsigned int FONTSET_START_ADDRESS = 0x50; // fontset is loaded from 0x50 onwards


// each line is for a given character, where if the binary data was lined up, it'd look like the corresponding character! See FONTS.txt.
uint8_t fontset[FONTSET_SIZE] =
{
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

void Chip8::LoadROM(char const* filename)
{
	// Open the file as a stream of binary/in binary mode and move the file pointer to the end
	std::ifstream file(filename, std::ios::binary | std::ios::ate);

	if (file.is_open())
	{
		// Get size of file and allocate a buffer to hold the ROM contents
		std::streampos size = file.tellg();   // get size of file equatable to the position of the file pointer
		char* buffer = new char[size];        // new is used to allocate memory, wowza

		// Go back to the beginning of the file and fill the buffer
		file.seekg(0, std::ios::beg);         // retrn to beginning of file
		file.read(buffer, size);              // read file contents to buffer and close
		file.close();

		// Load the ROM contents into the Chip8's memory, starting at 0x200
		for (long i = 0; i < size; ++i)
		{
			memory[START_ADDRESS + i] = buffer[i];
		}

		// Free the buffer from memory
		delete[] buffer;
	}
}

Chip8::Chip8()
// Initialize random number engine with time since clock start
: randGen(std::chrono::system_clock::now().time_since_epoch().count())
{
	// Initialize PC to 0x200
	pc = START_ADDRESS;

	// Load fonts into memory, starting at 0x50
	for (unsigned int i = 0; i < FONTSET_SIZE; ++i)
	{
		memory[FONTSET_START_ADDRESS + i] = fontset[i];
	}

	// Get random integer from distribution, 255U means UNSIGNED 255, constants/literals can have suffixes for this stuff
	randByte = std::uniform_int_distribution<uint8_t>(0, 255U);
}

void Chip8::OP_00E0()
{
	memset(video, 0, sizeof(video));
}

void Chip8::OP_00EE()
{
    --sp;
    pc = stack[sp];
}

void Chip8::OP_1nnn()
{
    uint16_t address = opcode & 0x0FFFu;
    /*
    0x0FFF is 12 bits of 1s in binary, opcode is 16 bits,
    where the first 4 bits are to identify the opcode as a JUMP,
    and the remaining 12 is the JUMP address!
    */
    pc = address; // next instruction to run will be at JUMP address naturally
}

void Chip8::OP_2nnn()
{
    // Same logic as previous opcode, gets address of next instruction
	uint16_t address = opcode & 0x0FFFu;

	stack[sp] = pc; // set pc to sp index
	++sp;
	pc = address;   // set pc to next instruction to execute
}

void Chip8::OP_3xkk()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u; // bitwise shift right 8 units
	uint8_t byte = opcode & 0x00FFu;

	if (registers[Vx] == byte)
	{
		pc += 2; // skip next instruction and point to instruction after it to execute
	}
}

void Chip8::OP_4xkk()
{
   	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t byte = opcode & 0x00FFu;

	if (registers[Vx] != byte)
	{
		pc += 2;
	}
}

void Chip8::OP_5xy0()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;

	if (registers[Vx] == registers[Vy])
	{
		pc += 2;
	}
}

void Chip8::OP_6xkk()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t byte = opcode & 0x00FFu;

	registers[Vx] = byte;
}

void Chip8::OP_7xkk()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t byte = opcode & 0x00FFu;

	registers[Vx] += byte;
}

void Chip8::OP_8xy0()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;

	registers[Vx] = registers[Vy];
}

void Chip8::OP_8xy1()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;

	registers[Vx] |= registers[Vy];
}

void Chip8::OP_8xy2()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;

	registers[Vx] &= registers[Vy];
}

void Chip8::OP_8xy3()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;

	registers[Vx] ^= registers[Vy];
}

void Chip8::OP_8xy4()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;

	uint16_t sum = registers[Vx] + registers[Vy];

	if (sum > 255U)
	{
		registers[0xF] = 1; // 0xF = VF register
	}
	else
	{
		registers[0xF] = 0;
	}

	registers[Vx] = sum & 0xFFu; // Store lowest 8 bits of calculated sum
}

void Chip8::OP_8xy5()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;

	if (registers[Vx] > registers[Vy])
	{
		registers[0xF] = 1;
	}
	else
	{
		registers[0xF] = 0;
	}

	registers[Vx] -= registers[Vy];
}

void Chip8::OP_8xy6()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;

	// Store least significant bit in VF
	registers[0xF] = (registers[Vx] & 0x1u);
	// Bitwise right shift/division by 2
	registers[Vx] >>= 1;
}

void Chip8::OP_8xy7()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;

	if (registers[Vy] > registers[Vx])
	{
		registers[0xF] = 1;
	}
	else
	{
		registers[0xF] = 0;
	}

	registers[Vx] = registers[Vy] - registers[Vx];
}

void Chip8::OP_8xyE()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;

	// Store most significant bit in VF
	registers[0xF] = (registers[Vx] & 0x80u) >> 7u;
	// Bitwise left shift/multiplication by 2
	registers[Vx] <<= 1;
}
