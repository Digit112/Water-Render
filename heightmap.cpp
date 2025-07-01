namespace eko {
	template <typename T>
	heightmap<T>::heightmap(std::ifstream& fin) : size{0, 0}, scale{0, 0, 0} {
		if (!fin.is_open()) throw std::invalid_argument("File not open.");
		
		// Check signature
		char signature[] = "     ";
		fin.read(signature, 5);
		if (fin.eof())
			throw std::invalid_argument("Encountered EOF while parsing header.");
		if (strcmp(signature, "EKOHM") != 0)
			throw std::invalid_argument("File lacks signature of Eko Heightmap.");
		
		// Check version
		uint8_t major_version = 255;
		uint8_t minor_version = 255;
		fin >> major_version >> minor_version;
		if (fin.eof())
			throw std::invalid_argument("Encountered EOF while parsing header.");
		if (major_version > 1)
			throw std::invalid_argument("File specifies unsupported version.");
		
		// Check encoding
		uint8_t encoding = 255;
		bool type_match = true;
		fin >> encoding;
		if (fin.eof())
			throw std::invalid_argument("Encountered EOF while parsing header.");
		
		switch (encoding) {
			case 0x0:
				type_match = std::is_same<T, uint8_t>::value; break;
			case 0x1:
				type_match = std::is_same<T, uint16_t>::value; break;
			case 0x2:
				type_match = std::is_same<T, uint32_t>::value; break;
			case 0x3:
				type_match = std::is_same<T, uint64_t>::value; break;
			case 0x4:
				type_match = std::is_same<T, int8_t>::value; break;
			case 0x5:
				type_match = std::is_same<T, int16_t>::value; break;
			case 0x6:
				type_match = std::is_same<T, int32_t>::value; break;
			case 0x7:
				type_match = std::is_same<T, int64_t>::value; break;
			case 0x8:
				type_match = std::is_same<T, float>::value; break;
			case 0x9:
				type_match = std::is_same<T, double>::value; break;
			default:
				throw std::invalid_argument("File specifies invalid sample encoding.");
		}
		
		if (!type_match)
			throw std::invalid_argument("File specifies sample encoding which does not match the template parameter used to instantiate this eko::heightmap.");
		
		// Read params and scales.
		fin.read(reinterpret_cast<char*>(&size[0]), sizeof(*size));
		fin.read(reinterpret_cast<char*>(&size[1]), sizeof(*size));
		fin.read(reinterpret_cast<char*>(&scale[0]), sizeof(*scale));
		fin.read(reinterpret_cast<char*>(&scale[1]), sizeof(*scale));
		fin.read(reinterpret_cast<char*>(&scale[2]), sizeof(*scale));
		
		if (fin.eof()) {
			size[0] = 0; size[1] = 0;
			scale[0] = 0; scale[1] = 0; scale[2] = 0;
			throw std::invalid_argument("Encountered EOF while parsing header.");
		}
		
		// Read body.
		alloc();
		for (size_t i = 0; i < get_num_samples(); i++) fin.read(reinterpret_cast<char*>(&data[i]), sizeof(*data));
		
		if (fin.eof()) {
			size[0] = 0; size[1] = 0;
			scale[0] = 0; scale[1] = 0; scale[2] = 0;
			delete[] data; data = nullptr;
			throw std::invalid_argument("Encountered EOF while parsing body.");
		}
	}
	
	template <typename T>
	size_t heightmap<T>::get_size() {
		return get_num_samples() * sizeof(T);
	}
	
	template <typename T>
	size_t heightmap<T>::get_num_samples() {
		return (size_t) size[0] * size[1];
	}
	
	template <typename T>
	void heightmap<T>::fill(T value) {
		for (uint64_t i = 0; i < get_num_samples(); i++) data[i] = value;
	}
	
	template <typename T>
	T& heightmap<T>::get(uint32_t x, uint32_t y) {
		return data[(uint64_t) y * size[0] + x];
	}
	
	template <typename T>
	void heightmap<T>::save(const char* fn) {
		std::ofstream fout(fn, std::ios::binary | std::ios::trunc);
		
		// Write signature and version
		fout << "EKOHM" << (uint8_t) 1 << (uint8_t) 0;
		
		// Write encoding.
		if constexpr (std::is_same<T, uint8_t>::value)
			fout << (uint8_t) 0;
		else if constexpr (std::is_same<T, uint16_t>::value)
			fout << (uint8_t) 1;
		else if constexpr (std::is_same<T, uint32_t>::value)
			fout << (uint8_t) 2;
		else if constexpr (std::is_same<T, uint64_t>::value)
			fout << (uint8_t) 3;
		else if constexpr (std::is_same<T, int8_t>::value)
			fout << (uint8_t) 4;
		else if constexpr (std::is_same<T, int16_t>::value)
			fout << (uint8_t) 5;
		else if constexpr (std::is_same<T, int32_t>::value)
			fout << (uint8_t) 6;
		else if constexpr (std::is_same<T, int64_t>::value)
			fout << (uint8_t) 7;
		else if constexpr (std::is_same<T, float>::value)
			fout << (uint8_t) 8;
		else if constexpr (std::is_same<T, double>::value)
			fout << (uint8_t) 9;
		else {
			fout.close();
			throw std::invalid_argument("Cannot save with provided template parameter.");
		}
		
		fout.write(reinterpret_cast<const char*>(&size[0]), sizeof(*size));
		fout.write(reinterpret_cast<const char*>(&size[1]), sizeof(*size));
		fout.write(reinterpret_cast<const char*>(&scale[0]), sizeof(*scale));
		fout.write(reinterpret_cast<const char*>(&scale[1]), sizeof(*scale));
		fout.write(reinterpret_cast<const char*>(&scale[2]), sizeof(*scale));
		
		// Save body
		for (size_t i = 0; i < get_num_samples(); i++) fout.write(reinterpret_cast<const char*>(&data[i]), sizeof(*data));
		
		fout.close();
	}
	
	template <typename T>
	heightmap<T>::~heightmap() {
		if (data != nullptr) delete[] data;
		data = nullptr;
	}
}