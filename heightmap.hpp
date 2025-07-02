#ifndef EKO_HEIGHTMAP
#define EKO_HEIGHTMAP

#if __cplusplus < 202302L
#error This code requires C++23 or later.
#endif

#include <cstdint>
#include <cstring>
#include <fstream>
#include <limits>
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
		struct pass_results {
			double sum;
			double avg;
			T max;
			T min;
		};
			
		heightmap(uint32_t width, uint32_t height) :
			size{width, height}, scale{1, 1, 1} {alloc();}
		
		heightmap(uint32_t width, uint32_t height, double xscale, double yscale, double zscale) :
			size{width, height}, scale{xscale, yscale, zscale} {alloc();}
		
		heightmap(const heightmap& other);
		heightmap(heightmap&& other);
		
		heightmap& operator=(const heightmap& other);
		heightmap& operator=(heightmap&& other);
		
		heightmap(std::ifstream& fin);
		
		/// Get the minimum representable sample value for the current sample encoding template parameter.
		static constexpr T get_min_sample();
		
		/// Get the maximum representable sample value for the current sample encoding template parameter.
		static constexpr T get_max_sample();
		
		/// Returns the width of the sample grid.
		uint32_t get_width() const;
		
		/// Returns the height of the sample grid.
		uint32_t get_height() const;
		
		/// Returns the size of the sample grid in bytes.
		size_t get_size() const;
		
		/// Returns the number of total grid samples (width * height)
		size_t get_num_samples() const;
		
		/// Returns max, min, and average values in the heightmap.
		pass_results get_stats() const;
		
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
		void save(const char* fn) const;
		
		/// Writes the heightmap as a viewable image. Creates a .pgm grayscale image.
		/// Linearly interpolates all values between the two passed min and max
		/// limits which correspond to black and white, respectively.
		void draw(const char* fn, T min_val, T max_val) const;
		
		/// Draws this heightmap as a file with the minimum extant sample
		/// corresponding to black and the maximum to white.
		void draw(const char* fn) const;
		
		~heightmap();
	};
}

#include "heightmap.cpp"

#endif