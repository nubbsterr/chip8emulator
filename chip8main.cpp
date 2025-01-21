#include "chip8data.h"

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
