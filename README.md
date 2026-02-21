#  Huffman File Compressor (C++)

A lossless file compression tool written in C++ that implements the Huffman coding algorithm with bit-level encoding and decoding.

## Features

- Lossless compression and decompression
- Bit level conding for efficient storage
- Supports any file type (text, binary, image, etc.)
- Custom binary file format
- Autmoatic Huffman tree reconstruction during decoding
- Displays compression ratio

## How it Works

### Encoding

1. Frequency Analysis

  Reads the input file byte-by-byte and calculates occurrence frequency for each character

2. Huffman Tree Construction

  Builds a HUffman tree using a priority queue where:

  - Lowest probability node have highest priority
  - Nodes are merged until a single root remains
 
3. Code Generation

 Traverses the tree to assign: 

 - `0` -> left branch
 - `1` -> right branch

This produces variable length prefix codes.

4. File Compression

Writes to output file:

Header:

- Number of unique characters
- Each character and its probability

Body: 

- Bit packed encoded data

Bits are packed into bytes for maximum compression efficiency

### Decoding 

The decoder:

1. Reads Header

Extracts:

- Character probabilites
- Reconstructs exact Huffman tree

2. Bit-Level Decoding

Reads compressed data bit-by-bit and:

- Traverses Huffman tree
- Outputs original characters


## Compilation

Requires C++17 or newer and make

```
  make
```

## Usage

Encode:

```
  ./huffman -e -i input.txt -o compressed.bin
```

Decode:

```
  ./huffman -e -i compressed.bin -o output.txt
```

Example Output

```
  input.txt 125000 to 52000 : 0.416
```

Compression ratio: 41.6% of original size


## Performance

Encoding:
```
  O(n log n)
```

Decoding:
```
  O(n)
```

Memory:
```
  O(k)
```
Where: 
- n = file size
- k = number of unqiue symbols (max 256)

## Future Improvements
- Store frequency instead of probability (better precision)
- Canonical Huffman codess
- Streaming compression
- Multi-threaded compression
- Compression statistics
- GUI interface
- SIMD optimization

  
  
