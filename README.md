# Unilang programming language

Unilang is a high-level object-oriented compiled programming language that aims to be easy to use and memory safe.

## Installation
```bash
git clone https://github.com/Paul-Passeron/Unilang.git
cd Unilang
make
sudo make install
cd ..
rm -rf Unilang
```

## Usage
```bash
Unilang <source_file> [options]
```

### Example
main.ul:
```
@include "stdlib/io.ul"

let entry(): void => {
  println("Hello, World !");
}
```

```bash
Unilang main.ul -o main
./main
# Output: Hello, World !
```