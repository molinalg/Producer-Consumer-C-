# Producer Consumer Problem

## Description
Developed in 2022, "Producer Consumer Problem" is a university project made during the second course of Computer Engineering at UC3M in collaboration with @angelmrtn.

It was made for the subject "Operating Systems" and is the solution to a problem proposed. The main goal of this project was to put in practice our knowledge about the programming language **C** and the **Producer Consumer problem**. It also makes the students learn how to use parallelism to accelerate processes.

## Table of Contents
- [Installation](#installation)
- [Usage](#usage)
- [Problem Proposed](#problem-proposed)
- [License](#license)
- [Contact](#contact)

## Installation
No special installation is needed for this code to work. 

## Usage
To execute the program first use the following command to compile and create the executable:
```sh
make
```
Once compiled, the code can be executed using:
```sh
./calculator [operations file] [num producers] [num consumers] [queue size]
```
**NOTE:** The parameters are explained below.

## Problem Proposed
This program pretends to solve a calculation problem using the producer and consumer process. For each operation in the text file passed as a parameter, there is a line that contains 3 numbers: the ID of the operation, the type of the operation and the cost in minutes of it. This will be used by the code to calculate the total cost in euros using the formula **[ total = total + (cost per minute * minutes) ]**. The cost per minute is determined by the type of the operation:

- **Type 1:** 3€ per minute.

- **Type 2:** 6€ per minute.

- **Type 3:** 9€ per minute.

- **Any other number:** 0€ per minute.

**Producers** will be in charge of introducing the data in a circular queue whenever there is space in it and operations remaining (the number of operations is located in the first line of the text file).

**Consumers** are in charge of retrieving the operations from the queue (emptying the space as well) and calculating the cost as described above.

This process explores using parallelism to accelerate calculations as each producer and consumer is executed in a separate thread. It also lets the user experiment with different combinations of the number of producers and consumers as well as the size of the queue (these three elements are passed as parameters).

## License
This project is licensed under the **MIT License**. This means you are free to use, modify, and distribute the software, but you must include the original license and copyright notice in any copies or substantial portions of the software.

## Contact
If necessary, contact the owner of this repository.
