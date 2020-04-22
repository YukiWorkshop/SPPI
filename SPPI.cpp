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

using namespace YukiWorkshop;

void SPPI::__init() {
	page_size = sysconf(_SC_PAGESIZE);
	if (page_size == -1)
		throw std::system_error(errno, std::system_category(), "failed to get page size");

	fd = open(path_.c_str(), O_RDWR);
	if (page_size == -1)
		throw std::system_error(errno, std::system_category(), "failed to open device");
}

void SPPI::__init_params(int __mode, int __bits_per_word, int __max_speed_hz) {
	if (__mode > 0)
		set_mode(__mode);
	else
		mode_ = mode();

	if (__bits_per_word > 0)
		set_bits_per_word(__bits_per_word);
	else
		bits_per_word_ = bits_per_word();

	if (__max_speed_hz > 0)
		set_max_speed_hz(__max_speed_hz);
	else
		max_speed_hz_ = max_speed_hz();
}

ssize_t SPPI::write_all(int __fd, const void *__buf, size_t __n) {
	size_t written = 0;

	while (written < __n) {
		ssize_t rc = write(__fd, (uint8_t *)__buf + written, __n - written);
		if (rc > 0) {
			written += rc;
		} else if (rc == 0) {
			return written;
		} else {
			return -1;
		}
	}

	return written;
}

SPPI::SPPI(const std::string &__device_path, int __mode, int __bits_per_word, int __max_speed_hz) {
	path_ = __device_path;
	__init();
	__init_params(__mode, __bits_per_word, __max_speed_hz);
}

const std::string &SPPI::path() const noexcept {
	return path_;
}

uint32_t SPPI::mode() const {
	int __buf;
	if (ioctl(fd, SPI_IOC_RD_MODE32, &__buf) < 0)
		throw std::system_error(errno, std::system_category(), "failed to get mode");
	return __buf;
}

uint8_t SPPI::bits_per_word() const {
	uint8_t __buf;
	if (ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &__buf) < 0)
		throw std::system_error(errno, std::system_category(), "failed to get bits_per_word");
	return __buf;
}

uint32_t SPPI::max_speed_hz() const {
	uint32_t __buf;
	if (ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &__buf) < 0)
		throw std::system_error(errno, std::system_category(), "failed to get max_speed_hz");
	return __buf;
}

void SPPI::set_mode(uint32_t __mode) {
	if (ioctl(fd, SPI_IOC_WR_MODE32, &__mode) < 0)
		throw std::system_error(errno, std::system_category(), "failed to set mode");
	mode_ = __mode;
}

void SPPI::set_bits_per_word(uint8_t __bits_per_word) {
	if (ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &__bits_per_word) < 0)
		throw std::system_error(errno, std::system_category(), "failed to set bits_per_word");
	bits_per_word_ = __bits_per_word;
}

void SPPI::set_max_speed_hz(uint32_t __max_speed_hz) {
	if (ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &__max_speed_hz) < 0)
		throw std::system_error(errno, std::system_category(), "failed to set max_speed_hz");
	max_speed_hz_ = __max_speed_hz;
}

uint16_t SPPI::transfer(uint16_t data, bool __cs_change, uint16_t __delay_usecs, uint8_t __word_delay_usecs) {
	if (bits_per_word_ > 8) {
		data = htobe16(data);
		transfer(&data, &data, 2, __cs_change, __delay_usecs, __word_delay_usecs);
		return data;
	} else {
		uint8_t buf = data;
		transfer(&buf, &buf, 1, __cs_change, __delay_usecs, __word_delay_usecs);
		return buf;
	}
}

void SPPI::transfer(const void *__tx_buf, void *__rx_buf, uint32_t __len, bool __cs_change, uint16_t __delay_usecs,
		    uint8_t __word_delay_usecs) {
	spi_ioc_transfer tr{0};
	tr.tx_buf = (__u64)__tx_buf;
	tr.rx_buf = (__u64)__rx_buf;
	tr.len = __len;
	tr.delay_usecs = __delay_usecs;
	tr.cs_change = __cs_change;
	tr.word_delay_usecs = __word_delay_usecs;
	tr.speed_hz = max_speed_hz_;
	tr.bits_per_word = bits_per_word_;

	if (ioctl(fd, SPI_IOC_MESSAGE(1), &tr) < 0)
		throw std::system_error(errno, std::system_category(), "failed to transfer");
}

//void SPPI::transfer(std::vector<SPPI_Transfer>& __transfers) {
//	for (auto &it : __transfers) {
//		it.speed_hz = max_speed_hz_;
//		it.bits_per_word = bits_per_word_;
//	}
//
//	if (ioctl(fd, SPI_IOC_MESSAGE(__transfers.size()), __transfers.data()) < 0)
//		throw std::system_error(errno, std::system_category(), "failed to transfer");
//}

void SPPI::send(const void *__tx_buf, uint32_t __len) {
	if (write_all(fd, __tx_buf, __len) < 0)
		throw std::system_error(errno, std::system_category(), "failed to send");
}

void SPPI::recv(void *__rx_buf, uint32_t __len) {
	if (read(fd, __rx_buf, __len) < 0)
		throw std::system_error(errno, std::system_category(), "failed to recv");
}
