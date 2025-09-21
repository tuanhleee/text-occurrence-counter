# Text Occurrence Counter

##  Project Goal
As part of the Algorithmics course (Licence 2 Informatique), this project aims to design and implement efficient data structures (custom hash table and binary tree) to filter and count occurrences of words/strings in large text files, while optimizing memory usage and execution time.

## Tech Stack
- **Language & Libraries**: C (libc)
- **Environment**: GCC on Linux, Geany IDE
- **Data Source**: Large text files

##  Features
- Custom hash table and binary tree for indexing words and special character strings
- Preprocessing to handle digits and special characters
- Iterative traversal and collision reduction strategies for faster execution
- Optimized memory management with fewer `malloc` calls

##  Optimization
- Reduction of dynamic memory allocations
- Iterative traversal of data structures
- Strategies to minimize hash collisions

## Results
- Ranked 2nd in class for memory and runtime efficiency
- Capable of processing large-scale text data with high performance

## ▶How to Run
```bash
gcc -o counter main.c
./counter input.txt
