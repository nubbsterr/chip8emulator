#include "chip8data.h"

const unsigned int START_ADDRESS = 0x200; // start address for Chip8, since 0x00 to 0x1FF is reserved for the Chip8

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
{
	// Initialize PC to 0x200
	pc = START_ADDRESS;
}
