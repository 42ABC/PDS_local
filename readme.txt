This is the README for the source code associated with the paper "New Strongly Regular Graphs Found via Local Search for Partial Difference Sets,"
by Andrew Brady, which has been published in the Electronic Journal for Combinatorics (Volume 32, Issue 1, 2025) (doi.org/10.37236/13296). 

For the PDSs in a cleaner form, see the results folder. 

Please contact me at acbrady2020 /at/ gmail with any questions/bugs.

PDS = Partial Difference Set
SRG = Strongly Regular Graph

_____________________
STEP 1: GENERATE GROUP TABLES
_____________________
In order to search a group for PDSs, one must first generate the group multiplication table (also called a convolution table) in the programming language GAP. To this end, 3 GAP scripts are provided. As I've included group tables themselves in the group_tables and tablesAll folders, one does not need to run these GAP scripts themselves in order to run the other programs in this project. 

gen_conv_table.g is for generating one group table. To run, open GAP and type
Read("gen_conv_table.g");

gen_tables_64.g is for generating the tables for all groups of order 64. To run, open GAP and type 
Read("gen_tables64.g");

gen_allTables.g is for generating the tables for all groups in a given order range. To run, open GAP and type
Read("gen_allTables.g");

The group table has 2 parts:
the first line of the table is two numbers: the order of the group and an identification number. This identification number for small groups is the SmallGrp id. For large groups, it is an arbitrary number. 


__________________________
STEP 2: RUN SEARCH
__________________________

Once you have group tables, a search can be ran. There are multiple scripts for different types of search.

search.cpp is for searching a single group for a specific type of PDS
compile with: g++ -o ac.out -std=c++17 search.cpp
and run with: ./ac.out GROUP_FILE n k lambda mu NUM_TRIALS
for example: ./ac.out group_tables/table64_226.txt 64 18 2 6 1000

Warning: the std::clock time may not report precisely accurate time, use at your discretion. 

search_parallel.cpp is for searching a single group for a specific type of PDS, but using shared memory parallelism with parlay. This is only effective at reducing runtime on a multicore machine.
Compile and run with:
Compile with: g++ -o ac.out -std=c++17 -I parlaylib/include search_parallel.cpp
Run with: ./ac.out ctpath n k lam mu num_trials
For example: ./ac.out group_tables/table64_226.txt 64 28 12 12 1000
num_trials trials of local search will be run

If you get the below compile error (or something similar), adding the -pthread flag to the compile command should fix your problem.
/tmp/ccMqWdGg.o: In function `std::thread::thread<parlay::scheduler<parlay::WorkStealingJob>::scheduler(unsigned long)::{lambda()#1}, , void>(parlay::scheduler<parlay::WorkStealingJob>::scheduler(unsigned long)::{lambda()#1}&&)':
search64_par.cpp:(.text._ZNSt6threadC2IZN6parlay9schedulerINS1_15WorkStealingJobEEC4EmEUlvE_JEvEEOT_DpOT0_[_ZNSt6threadC5IZN6parlay9schedulerINS1_15WorkStealingJobEEC4EmEUlvE_JEvEEOT_DpOT0_]+0x21): undefined reference to `pthread_create'
collect2: error: ld returned 1 exit status

search64.cpp is for searching all groups of order 64, in order to compare the performance of different error functions.
Compile with: g++ -o ac.out -std=c++17 search64.cpp
Run with: ./ac.out FOLDER n k lam mu NUM_TRIALS num_groups, where
-> FOLDER contains all tables of that group size
-> The code will check groups with id {1,...,num_groups} for PDSs
For example: ./ac.out tables64 64 18 2 6 10 267

search64_parallel.cpp is for searching all groups of order 64 in parallel, in order to compare the performance of different error functions.
Compile with: g++ -o ac.out -std=c++17 -I parlaylib/include search64_par.cpp
Run with: ./ac.out FOLDER n k lam mu NUM_TRIALS num_groups, where
-> FOLDER contains all tables of that group size
-> The code will check groups with id {1,...,num_groups} for PDSs
For example: ./ac.out tablesAll 64 18 2 6 10 267
If you get a compile error, adding the -pthread flag may fix your problem. 

searchMany.cpp will try to find PDSs from a list of dimensions, in any group that could work for that PDS (meaning that for parameters (n,k,lam,mu), all groups of order n will be searched)
Compile with: g++ -o ac.out -std=c++17 -I parlaylib/include searchMany.cpp
Run with: ./ac.out s t MULTIPLIER
For example: ./ac.out 0 133 5
s = start index (inclusive) from dimension list, available at run_input/AllDims.txt
t = end index (exclusive) from dimension list
MULTIPLIER <- for each group, MULTIPLIER * n^2 trials will be run. Thus Multiplier controls how many trials are run. 
MULTIPLIER * n^2 trials are run because the search space is bigger for groups with higher order, and so more trials are warranted
Note: the group tables must be in a folder called tablesAll and named "tableGROUPORDER_GID.txt" for searchMany.cpp to run.
__________________________
STEP 3: POLISH AND ANALYZE DATA
__________________________

sortAsData.py will sort output from SearchMany.cpp
Run with: python sortAsData.py 
To control the file it reads, see the line with CHANGEME in the code

checkPDS.py will double check that the PDSs outputted from SearchMany.cpp are actually PDSs
Run with: python checkPDS.py
To control the file it reads, see the line with CHANGEME in the code
However, checkPDS.py requires that the necessary group tables be created to run, in a folder called tablesAll, with 
the name "tableGROUPORDER_GID.txt".