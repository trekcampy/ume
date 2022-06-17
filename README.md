# URL Matching Engine
This is URL filtering tool written in C. It supports wildcard `*` notation.

## Build Instruction
`gcc *.c  -o ume` 

## Usage Instructions
`ume <pattern file> <url file>`

Where 
    <pattern file> : is the absolute path to the file containing patterns, one pattern per line. 
    <url file>.    : is the absolute path to the file containing URLs that require matching, one URL per line.
