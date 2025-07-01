# Water-Render

A simple script for generating and rendering heightmaps which represent the surface of the ocean.

## Eko Heightmap (.ekhm) Version 1.0

Header (40 bytes):

1. ASCII-encoded `EKOHM` (5 bytes)

	A 5-byte filetype signature.

2. Version Number (2 byte)

	A 1-byte major version number followed by a 1-byte minor version number.
	Must be 1 and 0 respectively.

3. Sample Encoding (1 byte)

	Gives the data type. Only the least-significant 4 bits are used.

	- 0x0 (0b0000) uint8
	- 0x1 (0b0001) uint16
	- 0x2 (0b0010) uint32
	- 0x3 (0b0011) uint64
	- 0x4 (0b0100) int8
	- 0x5 (0b0101) int16
	- 0x6 (0b0110) int32
	- 0x7 (0b0111) int64
	- 0x8 (0b1000) single-precision float
	- 0x9 (0b1001) double-precision float

4. Sample Grid Dimensions (8 bytes)

	A pair of 4-byte unsigned integers denoting the width and height of the sample grid respectively. Multiplied by each other and by the width of the sample representation, these give the size of the sample body in bytes.

5. Scales (24 bytes)

	A trio of double-precision floating point numbers representing the x, y, and z scaling.
	
	The first value (x) represents the distance between adjacent samples in the same row. The second value (y) represents the distance between adjacent samples in the same column. The third value (z) represents the vertical distance denoted by a difference of 1 between two samples, which is also the least possible representable positive height using any of the integer sample types.

The body simply consists of the samples themselves written end-to-end in row-major order.
