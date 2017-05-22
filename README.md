# SimpleRISC-Functional-simulator

================================================
Functional Simulator for simpleRISC processor
================================================

README

Table of contents
1. Directory Structure
2. How to build
3. How to execute

Directory Structure:
--------------------
CS211-Project
  |
  |- bin
      |
  |- doc
      |
  |- include
      |- mySimpleSim.h
  |- src
      |- main.c
      |- Makefile
      |- mySimpleSim.c
  |- test
      |
      |- ReadME_Test_Cases
      |- sum.txt
      |- fibonacci.txt
      |- parity.txt
      |- power.txt
      |- array_sum.txt
      |- foo.txt
      |- simple_add.mem
How to build
------------
For building:
	$cd src
	$make

For cleaning the project:
	$cd src
	$make clean

1. What our program is doing?
	
	After building the project using makefile, our code takes input from the txt file related to the question
	which consists of some instructions in hexadecimal format. Then our code initialises it through the main 
	function which further calls run_simplesim function which performs the basic five steps using the instruction.
	
	First of all, our instruction passes through the fetch unit which stores the instruction in instruction memory
	and updates the pc.
	
	After that we decode the opcode, source registers, destination register and find the immediate value, branch target.
	
	In this step, we get further details like branchPc, branch target, results of arithmetic logic operations based 
	on the value of opcode. We set the flags and update the proper pc according to the alu operation on instruction.
	In this unit, we also calculate and save the results after performing alu operations.

	In memory access, we store the instruction to memory or load the instruction from the memory based on the instruction.
	
	Finally, we write the values in register files(memory location) using the Write Back unit if isWb is 1 and calculating 
	which address(i.e. register) to go. And write the ldresult and aluresult in register file depending on the values of 
	isLd and isCall.
	
	
2. Limitations/bugs in the code:

	if very large of number of instructions are there it might lead to incorrect results or segmentation faults because of memory constraints
	(an issue wth basic assumption of SimpleRisc and practically available memory in a system i.e theoritical vs practical). 

3. How to compile ?

	$cd src
	$make


4. How to use/run ?
									//After Compiling.
	$cd -
	$cd test
	$./mySimpleSim sum.txt           // Or name of any other txt file that we want to run.
 //for cleaning
	$cd -
	$cd src
	$make clean

5. How output looks like ?

	After processing all the instructions from the txt file, We are printing the values of different 
	operands and the immediate for each instruction and after that we are also printing the final values 
	of registers from R0 to R15 and different stages start and end of execution.

