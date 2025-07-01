namespace eko {
	template <typename T>
	heightmap<T>::heightmap(std::ifstream& fin) : size{0, 0}, scale{0, 0, 0} {
		if (!fin.is_open()) throw new std::invalid_argument("File not open.");
		
		// Check signature
		char signature[] = "     ";
		fin.get(signature, 5);
		if (fin.eof())
			throw new std::invalid_argument("Encountered EOF while parsing header.");
		if (strcmp(signature, "EKOHM") != 0)
			throw new std::invalid_argument("File lacks signature of Eko Heightmap.");
		
		// Check version
		uint8_t version = 255;
		fin >> version;
		if (fin.eof())
			throw new std::invalid_argument("Encountered EOF while parsing header.");
		if (version > 1)
			throw new std::invalid_argument("File specifies unsupported version.");
		
		// Check encoding
		uint8_t encoding = 255;
		bool type_match = true;
		fin >> encoding;
		if (fin.eof())
			throw new std::invalid_argument("Encountered EOF while parsing header.");
		
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
				throw new std::invalid_argument("File specifies invalid sample encoding.");
		}
		
		if (!type_match)
			throw new std::invalid_argument("File specifies sample encoding which does not match the template parameter used to instantiate this eko::heightmap.");
		
		// Read params and scales.
		fin >> size[0] >> size[1] >> scale[0] >> scale[1] >> scale[2];
		if (fin.eof()) {
			size[0] = 0; size[1] = 0;
			scale[0] = 0; scale[1] = 0; scale[2] = 0;
			throw new std::invalid_argument("Encountered EOF while parsing header.");
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
}