/*
    This file is part of SPPI.
    Copyright (C) 2020 ReimuNotMoe

    This program is free software: you can redistribute it and/or modify
    it under the terms of the MIT License.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*/

#include "SPPI.hpp"
#include <iostream>

using namespace YukiWorkshop;

int main() {
	SPPI s("/dev/spidev0.1");

	std::cout << "Device: " << s.path() << "\n";
	std::cout << "Mode: " << s.mode() << "\n";
	std::cout << "Bits per word: " << +s.bits_per_word() << "\n";
	std::cout << "Max speed: " << s.max_speed_hz() << " Hz\n";

//	s.select();
	std::cout << "Transfer test: " << std::hex << +s.transfer(0x00, false) << "\n";

//	while (1) {
//		s.transfer(0x00, true);
//	}

	sleep(5);
//	s.deselect();
	s.transfer(0x00, true);
//	sleep(5);



	return 0;
}