### Simple Virtual Memory Manager ###
* Version 1.0

### What is this repository for? ###

The objective of this project is to implement a simple virtual memory manager “virtualmem” in
C/C++ on a UNIX-based platform.

virtualmem is a simple virtual memory manager. It takes a sequence of page
references as an input, as well as the number of available frames. It performs the placement of
these pages to the available frames using the page replacement policy specified by the user.


### How do I get set up? ###

Compile the program using the make command.
SYNOPSIS: 

virtualmem [−h] [-f available-frames] [−r replacement-policy] [−i input_file]

	−h : Print a usage summary with all options and exit.

	−f : available-frames; Set the number of available frames. By default it should be 5.

	−r : replacement-policy; Set the page replacement policy. It can be either

		- FIFO (First-in-first-out)
		- LFU (Least-frequently-used)
		- LRU-STACK (Least-recently-used stack implementation)
		- LRU-CLOCK (Least-recently-used clock implementation – second chance alg.).
		- LRU-REF8 (Least-recently-used Reference-bit Implementation, using 8 reference bits) 
		The default will be FIFO.
	
	−i input file : Read the page reference sequence from a specified file. If not given, the sequence should be read from STDIN (ended with ENTER). 

### Who do I talk to? ###

  Admins:

Mark Abidargham - markabid@buffalo.edu,  
Syed Ali Hasan - syedhasa@buffalo.edu,  
Ahmad M. Temoor - ahmadtem@buffalo.edu

