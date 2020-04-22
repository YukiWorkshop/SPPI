/*
    This file is part of SPPI.
    Copyright (C) 2020 ReimuNotMoe

    This program is free software: you can redistribute it and/or modify
    it under the terms of the MIT License.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*/

#include <string>
#include <vector>
#include <algorithm>
#include <system_error>

#include <cstring>
#include <cinttypes>

#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>


namespace YukiWorkshop {

	class SPPI_Transfer : public spi_ioc_transfer {
	public:
		SPPI_Transfer(const void *__tx_buf, void *__rx_buf, uint32_t __len, uint16_t __delay_usecs = 0, bool __cs_change = true, uint8_t __word_delay_usecs = 0) {
			memset(this, 0, sizeof(spi_ioc_transfer));
			len = __len;
			delay_usecs = __delay_usecs;
			word_delay_usecs = __word_delay_usecs;
			cs_change = __cs_change ? 1 : 0;
			tx_buf = (__u64)__tx_buf;
			rx_buf = (__u64)__rx_buf;
		}
	};


	class SPPI {
	protected:
		int fd = -1;
		std::string path_;
		long page_size = -1;

		uint32_t mode_ = 0;
		uint8_t bits_per_word_ = 0;
		uint32_t max_speed_hz_ = 0;

		void __init();
		void __init_params(int __mode, int __bits_per_word, int __max_speed_hz);

		static ssize_t write_all(int __fd, const void *__buf, size_t __n);
	public:
		explicit SPPI(const std::string& __device_path, int __mode = -1, int __bits_per_word = -1, int __max_speed_hz = -1);

		const std::string& path() const noexcept;
		uint32_t mode() const;
		uint8_t bits_per_word() const;
		uint32_t max_speed_hz() const;

		void set_mode(uint32_t __mode);
		void set_bits_per_word(uint8_t __bits_per_word);
		void set_max_speed_hz(uint32_t __max_speed_hz);

		uint16_t transfer(uint16_t data, bool __cs_change = true, uint16_t __delay_usecs = 0, uint8_t __word_delay_usecs = 0);
		void transfer(const void *__tx_buf, void *__rx_buf, uint32_t __len, bool __cs_change = true, uint16_t __delay_usecs = 0, uint8_t __word_delay_usecs = 0);
//		void transfer(std::vector<SPPI_Transfer>& __transfers);

		template <typename T>
		void transfer(std::vector<T> &__tx_buf, std::vector<T> &__rx_buf) {
			transfer({__tx_buf.data(), __rx_buf.data(), std::min(__tx_buf.size(), __tx_buf.size()) * sizeof(T)});
		}

		void send(const void *__tx_buf, uint32_t __len);

		template <typename T>
		void send(const std::vector<T> &__tx_buf) {
			send(__tx_buf.data(), __tx_buf.size() * sizeof(T));
		}

		void recv(void *__rx_buf, uint32_t __len);

		template <typename T>
		void recv(const std::vector<T> &__rx_buf) {
			recv(__rx_buf.data(), __rx_buf.size() * sizeof(T));
		}

	};
}