# File Compressor

**A command-line utility for efficient, lossless file compression and decompression using the Huffman Coding algorithm in C++.**

This project provides a command-line utility to compress and decompress text files using the Huffman Coding algorithm. It offers efficient, lossless compression, significantly reducing file sizes by assigning shorter binary codes to more frequent characters.

## Features

-   **Lossless Compression**: Compresses files without any loss of data.
-   **Lossless Decompression**: Perfectly reconstructs the original file from a compressed file.
-   **Command-Line Interface**: Simple and easy-to-use interface for both compressing and decompressing files.
-   **Cross-Platform**: Can be compiled and run on Windows, macOS, and Linux.

## Prerequisites

To build and run this project, you will need a C++ compiler that supports the C++17 standard, such as:
-   **GCC** (`g++`)
-   **Clang** (`clang++`)

## Building the Project

A `Makefile` is provided for easy compilation.

1.  **Clone the repository:**
    ```bash
    git clone <https://github.com/Amangaur31/File-Compressor/tree/main>
    cd <repository-directory>
    ```

2.  **Build the executable:**
    From the project's root directory, run the `make` command. This will compile the source code and create an executable named `huffman`.
    ```bash
    make
    ```

3.  **Clean up build files:**
    To remove the compiled executable, you can run:
    ```bash
    make clean
    ```

## Usage

The program is operated from the command line with the following syntax:
*(On Windows, you may need to run `huffman.exe` or `.\huffman` instead of `./huffman`)*

### Commands

-   `compress` or `c`: Compresses the `<input_file>`.
-   `decompress` or `d`: Decompresses the `<input_file>`.

### Examples

-   **To compress a file:**
    ```bash
    ./huffman compress original.txt compressed.bin
    ```

-   **To decompress a file:**
    ```bash
    ./huffman decompress compressed.bin restored.txt
    ```

## How It Works

The program follows the classic Huffman Coding algorithm:

1.  **Frequency Analysis**: It first reads the input file to build a frequency table of all characters.
2.  **Build Huffman Tree**: A priority queue is used to construct a Huffman Tree. Characters with lower frequencies are placed deeper in the tree.
3.  **Generate Codes**: The tree is traversed to generate a unique binary code for each character. More frequent characters get shorter codes.
4.  **Encoding**: The input file is read again, and each character is replaced with its corresponding Huffman code. This binary data, along with a header containing the frequency table, is written to the output file.
5.  **Decoding**: To decompress, the program reads the header to reconstruct the Huffman Tree. It then reads the compressed data bit-by-bit, traversing the tree to find the original characters and write them to the output file.
