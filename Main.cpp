void executeInstruction(void){
	int address;

	if ((IR & 0x80) == 0x80){ //Math Ops 1000****
		int destination;
		int source;

		switch(IR & 0x0c){ //Dest
			case 0x0:
				destination = mem[MAR]; // Indirect (MAR used as a pointer)
				break;
			case 0x4:
				destination = ACC; // Accumulator ACC
				break;
			case 0x8:
				destination = MAR; // Address register MAR
				break;
			case 0xc:
				destination = mem[((mem[oldPC + 1] << 8) + mem[oldPC + 2])]; // Memory
				break;
		}

		switch(IR & 0x03){ //Src
			case 0x0: //MAR pointer
				source = mem[MAR];
				break;
			case 0x1: //ACC
				source = ACC;
				break;
			case 0x2: //Const
				if((IR & 0x0c) == 0x8)
					source = (mem[PC - 2] << 8) + mem[PC -1];
				else
					source = mem[PC - 1];
				break;
			case 0x3: //Mem
				if((IR & 0x0c) == 0x8){
					address = ((mem[oldPC + 1] << 8) + mem[oldPC + 2]);
					source = (mem[address] << 8) + mem[address + 1];
				}
				else
					source = mem[((mem[oldPC + 1] << 8) + mem[oldPC + 2])];
				break;
		}

		switch (IR & 0x70){ //Determine func
			case 0x00: //AND
				destination &= source;
				break;
			case 0x10: //OR
				destination |= source;
				break;
			case 0x20: //XOR
				destination ^= source;
				break;
			case 0x30: //ADD
				destination += source;
				break;
			case 0x40: //SUB
				destination -= source;
				break;
			case 0x50: //INC
				destination++;
				break;
			case 0x60: //DEC
				destination--;
				break;
			case 0x70: //NOT
				destination = !destination;
			break;
			default:
				break;
		}
		// Store destination
		switch(IR & 0x0c){ // Isolate destination id
			case 0x0:
				mem[MAR] = destination & 0xff; // Indirect (MAR used as a pointer)
				break;
			case 0x4:
				ACC = destination & 0xff; // Accumulator ACC
				break;
			case 0x8:
				MAR = destination & 0xffff; // Address register MAR
				break;
			case 0xc:
				mem[((mem[oldPC + 1] << 8) + mem[oldPC + 2])] = destination & 0xff; // Memory
				break;
		}
	}

	else if ((IR & 0xf0) == 0){ //Memory Ops 0000****
		if ((IR & 0x08) == 0){ // Store
			if ((IR & 0x04) == 0){ // Storing from Register = Accumulator ACC
				switch(IR & 0x03){
					case 0:
						mem[((mem[oldPC + 1] << 8) + mem[oldPC + 2])] = ACC; // Storing with Method = Operand is used as address
						break;
					case 1: // Operand is used as a Constant
						break;
					case 2:
						mem[MAR] = ACC; // Storing with Method = Indirect (MAR used as a pointer)
						break;
					default:
						break;
				}
			}
			else{ // Storing from Register = Index register MAR
				switch(IR & 0x03){
					case 0:
						mem[((mem[oldPC + 1] << 8) + mem[oldPC + 2])] = (MAR >> 8) & 0xff; // Storing with Method = Operand is used as address
						mem[((mem[oldPC + 1] << 8) + mem[oldPC + 2]) + 1] = MAR & 0xff;
						break;
					case 1: // Operand is used as a Constant
						break;
					case 2:
						mem[MAR] = (MAR >> 8) & 0xff; // Storing with Method = Indirect (MAR used as a pointer)
						mem[MAR + 1] = MAR & 0xff;
						break;
					default:
						break;
				}
			}
		}
		else{ // Load
			if ((IR & 0x04) == 0){ // Loading into register Accumulator ACC
				switch(IR & 0x03){
					case 0:
						ACC = mem[((mem[oldPC + 1] << 8) + mem[oldPC + 2])]; // Method = Operand is used as address
						break;
					case 1:
						ACC = mem[oldPC + 1]; // Method = Operand is used as a constant
						break;
					case 2:
						ACC = mem[MAR]; // Method = Indirect (MAR used as a pointer)
						break;
					default:
						break;
				}
			}
			else{ // Loading into MAR
				int oldMAR = MAR;

				switch(IR & 0x03){
					case 0: // Method = Operand is used as address
						MAR = mem[((mem[oldPC + 1] << 8) + mem[oldPC + 2])];
						MAR <<=8;
						MAR += mem[((mem[oldPC + 1] << 8) + mem[oldPC + 2]) + 1];
						break;
					case 1: // Method = Operand is used as a constant
						MAR = mem[oldPC + 1];
						MAR <<= 8;
						MAR += mem[oldPC + 2];
						break;
					case 2: // Method = Indirect (MAR used as pointer)
						MAR = mem[oldMAR];
						MAR <<= 8;
						MAR += mem[oldMAR + 1];
						break;
					default:
						break;
				}
			}
		}
	}

	else if ((IR & 0xF8) == 0x10){ //Branches/Jumps 00010***
		address = (mem[oldPC + 1] << 8) + mem[oldPC + 2];

		switch (IR & 0x07){
			case 0: // BRA (Unconditional branch/branch always)
				PC = address;
				break;
			case 1: // BRZ (Branch if ACC = 0)
				if (ACC == 0)
					PC = address;
				break;
			case 2: // BNE (Branch if ACC != 0)
				if (ACC != 0)
					PC = address;
				break;
			case 3: // BLT (Branch if ACC < 0)
				if ((ACC & 0x80) != 0)
					PC = address;
				break;
			case 4: // BLE (Branch if ACC <= 0)
				if (((ACC & 0x80) != 0) || (ACC == 0))
					PC = address;
				break;
			case 5: // BGT (Branch if ACC > 0)
				if (((ACC & 0x80) == 0) && (ACC != 0))
					PC = address;
				break;
			case 6: // BGE (Branch if ACC >= 0)
				if ((ACC & 0x80) == 0)
					PC = address;
				break;
			default:
				break;
		}
	}

	else{ //Not a main OPCODE, special or invalid
		if(IR == 0x18) //No Op
		{

		}

		else if(IR == 0x19) //Halt
		{

		}

		else //Invalid Op
		{

		}
	}
}
