namespace eko {
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