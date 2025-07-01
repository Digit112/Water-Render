#ifndef EKO_HEIGHTMAP
#define EKO_HEIGHTMAP

#if __cplusplus < 202302L
#error This code requires C++23 or later.
#endif

#include <cstdint>
#include <cstring>
#include <fstream>
#include <stdexcept>
#include <type_traits>

namespace eko {
	template <typename T>
	class heightmap {
	private:
		static_assert(std::is_integral<T>::value || std::is_floating_point<T>::value, "Template parameter T must be an integer or floating-point type.");
		
		uint32_t size[2];
		double scale[3];
		T* data;
		
		void alloc() {
			if (get_num_samples() == 0)
				throw new std::invalid_argument("Size of heightmap would be 0.");
			
			if (SIZE_MAX / get_num_samples() < sizeof(T))
				throw new std::invalid_argument("Size of heightmap would be greater than SIZE_MAX bytes.");
				
			data = new T[get_size()];
		}
	
	public:
		heightmap(uint32_t width, uint32_t height) :
			size{width, height}, scale{1, 1, 1} {alloc();}
		
		heightmap(uint32_t width, uint32_t height, double xscale, double yscale, double zscale) :
			size{width, height}, scale{xscale, yscale, zscale} {alloc();}
		
		heightmap(const heightmap& other);
		heightmap(heightmap&& other);
		
		heightmap& operator=(const heightmap& other);
		heightmap& operator=(heightmap&& other);
		
		heightmap(std::ifstream& fin);
		
		/// Returns the size of the sample grid in bytes.
		size_t get_size();
		
		/// Returns the number of total grid samples (width * height)
		size_t get_num_samples();
		
		/// Fills the entire grid with the passed value.
		void fill(T value);
		
		/// Return sample as read-only rvalue
		T get(uint32_t x, uint32_t y) const { return const_cast<heightmap&>(*this).get(x, y); }
		
		/// Return sample as writable lvalue reference
		T& get(uint32_t x, uint32_t y);
		
		/// Return sample as read-only rvalue
		T operator[](uint32_t x, uint32_t y) const { return get(x, y); }
		
		/// Return sample as writable lvalue reference
		T& operator[](uint32_t x, uint32_t y) { return get(x, y); }
		
		/// Save to file.
		void save(const char* fn);
		
		~heightmap();
	};
}

#include "heightmap.cpp"

#endif