namespace eko {
	/* ---- copy and move semantics ---- */
	
	template <typename T> // copy constructor
	heightmap<T>::heightmap(const heightmap& other) :
		size{other.size[0], other.size[1]}, scale{other.scale[0], other.scale[1], other.scale[2]}
	{
		alloc();
		for (size_t i = 0; i < get_num_samples(); i++)
			data[i] = other.data[i];
	}
	
	template <typename T> // move constructor
	heightmap<T>::heightmap(heightmap&& other) :
		size{other.size[0], other.size[1]}, scale{other.scale[0], other.scale[1], other.scale[2]}, data(other.data)
	{
		other.size[0] = 0; other.size[1] = 0;
		other.scale[0] = 0; other.scale[1] = 0; other.scale[2] = 0;
		other.data = nullptr;
	}
	
	template <typename T> // copy assignment
	heightmap<T>& heightmap<T>::operator=(const heightmap& other) {
		size[0] = other.size[0]; size[1] = other.size[1];
		scale[0] = other.scale[0]; scale[1] = other.scale[1]; scale[2] = other.scale[2];
		
		if (data != nullptr) delete[] data;
		alloc(); // TODO: Don't reallocate if the size hasn't changed!
		
		for (size_t i = 0; i < get_num_samples(); i++)
			data[i] = other.data[i];
	}
	
	template <typename T> // move assignment
	heightmap<T>& heightmap<T>::operator=(heightmap&& other) {
		size[0] = other.size[0]; size[1] = other.size[1];
		scale[0] = other.scale[0]; scale[1] = other.scale[1]; scale[2] = other.scale[2];
		
		if (data != nullptr) delete[] data;
		data = other.data;
		
		other.size[0] = 0; other.size[1] = 0;
		other.scale[0] = 0; other.scale[1] = 0; other.scale[2] = 0;
		other.data = nullptr;
	}
	
	/* ---- load from file ---- */
	
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
		for (size_t i = 0; i < get_num_samples(); i++)
			fin.read(reinterpret_cast<char*>(&data[i]), sizeof(*data));
		
		if (fin.eof()) {
			size[0] = 0; size[1] = 0;
			scale[0] = 0; scale[1] = 0; scale[2] = 0;
			delete[] data; data = nullptr;
			throw std::invalid_argument("Encountered EOF while parsing body.");
		}
	}
	
	/* ---- utilities ---- */
	
	template <typename T>
	constexpr T heightmap<T>::get_min_sample() {
		if constexpr (std::is_same<T, uint8_t>::value)
			return 0;
		else if constexpr (std::is_same<T, uint16_t>::value)
			return 0;
		else if constexpr (std::is_same<T, uint32_t>::value)
			return 0;
		else if constexpr (std::is_same<T, uint64_t>::value)
			return 0;
		else if constexpr (std::is_same<T, int8_t>::value)
			return INT8_MIN;
		else if constexpr (std::is_same<T, int16_t>::value)
			return INT16_MIN;
		else if constexpr (std::is_same<T, int32_t>::value)
			return INT32_MIN;
		else if constexpr (std::is_same<T, int64_t>::value)
			return INT64_MIN;
		else if constexpr (std::is_same<T, float>::value)
			return -std::numeric_limits<float>::infinity();
		else if constexpr (std::is_same<T, double>::value)
			return -std::numeric_limits<double>::infinity();
		else {
			throw std::invalid_argument("Cannot retrieve minimum for given template.");
		}
	}
	
	template <typename T>
	constexpr T heightmap<T>::get_max_sample() {
		if constexpr (std::is_same<T, uint8_t>::value)
			return UINT8_MAX;
		else if constexpr (std::is_same<T, uint16_t>::value)
			return UINT16_MAX;
		else if constexpr (std::is_same<T, uint32_t>::value)
			return UINT32_MAX;
		else if constexpr (std::is_same<T, uint64_t>::value)
			return UINT64_MAX;
		else if constexpr (std::is_same<T, int8_t>::value)
			return INT8_MAX;
		else if constexpr (std::is_same<T, int16_t>::value)
			return INT16_MAX;
		else if constexpr (std::is_same<T, int32_t>::value)
			return INT32_MAX;
		else if constexpr (std::is_same<T, int64_t>::value)
			return INT64_MAX;
		else if constexpr (std::is_same<T, float>::value)
			return std::numeric_limits<float>::infinity();
		else if constexpr (std::is_same<T, double>::value)
			return std::numeric_limits<double>::infinity();
		else {
			throw std::invalid_argument("Cannot retrieve minimum for given template.");
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
	heightmap<T>::pass_results heightmap<T>::get_stats() const {
		pass_results res{0, 0, heightmap::get_min_sample(), heightmap::get_max_sample()};
		
		for (size_t i = 0; i < get_num_samples(); i++) {
			res.sum += data[i];
			if (data[i] > res.max) res.max = data[i];
			if (data[i] < res.min) res.min = data[i];
		}
		
		res.avg = res.sum / get_num_samples();
		
		return res;
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
		for (size_t i = 0; i < get_num_samples(); i++)
			fout.write(reinterpret_cast<const char*>(&data[i]), sizeof(*data));
		
		fout.close();
	}
	
	template <typename T>
	heightmap<T>::~heightmap() {
		if (data != nullptr) delete[] data;
		data = nullptr;
	}
}