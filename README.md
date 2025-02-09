# co_de

Data Compression Library and Command-line Tool

## Overview
This project implements popular data compression algorithms and provides a command-line utility for compressing/decompressing files. Currently supports:

- [Huffman Coding](https://en.wikipedia.org/wiki/Huffman_coding) - A lossless data compression algorithm using variable-length encoding
- [LZW (Lempel-Ziv-Welch)](https://en.wikipedia.org/wiki/Lempel%E2%80%93Ziv%E2%80%93Welch) - A universal lossless compression algorithm ideal for text files

## Project Structure

```
├── CMakeLists.txt          # Build configuration
├── README.md              
├── Harry_Potter.txt        # Sample test file
├── build/                  # Build artifacts
├── include/               
│   ├── huffman.hpp         # Huffman algorithm header
│   └── lzw.hpp             # LZW algorithm header  
├── src/
│   ├── huffman.cpp         # Huffman implementation
│   └── lzw.cpp             # LZW implementation
├── main.cpp                # Command-line interface
└── test/                   # Folder for testing
```

## Building from Source

```bash
mkdir build
cd build
cmake ..
make
```

## Usage
The command-line tool syntax:
```bash
compressor --algorithm lzw/huffman --mode compress/decompress -i <input_file_or_folder> -o <output_file_or_folder>
```

## Sample test case

### Example Usage

1. Compress/Decompress a folder using LZW:
```bash
# Compression
./co_de -a lzw -m compress -i test/ -o test
# Output: test.folder.lzw (10883ms)
Compressing folder: test/
Compression successful: test
Execution time: 10883 ms

# Decompression 
./co_de -a lzw -m decompress -i test.folder.lzw -o test_lzw
# Output: test_lzw folder (5583ms)
Decompressing folder archive: test.folder.lzw
Decompression successful: test_lzw
Execution time: 5583 ms
```

2. Compress/Decompress a file using Huffman:
```bash
# Compression
./co_de -a huffman -m compress -i Harry_Potter.txt -o Harry_Potter.txt.huff
# Output: Harry_Potter.txt.huff (6009ms)
Compression file: Harry_Potter.txt.huff
Compression successful: Harry_Potter.txt.huff
Execution time: 6009 ms

# Decompression
./co_de -a huffman -m decompress -i Harry_Potter.txt.huff -o Harry_Potter_huff.txt
# Output: Harry_Potter_huff.txt (13356ms)
Decompressing file: Harry_Potter.txt.huff
Decompression successful: Harry_Potter_huff.txt
Execution time: 13356 ms
```
