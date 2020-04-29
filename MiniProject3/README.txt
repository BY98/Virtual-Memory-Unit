Part 1 ( no page replacement) and Part 2 ( with page replacement ) of the assignment are split into two seperate c files.

Part 1 and Part 2 can each be compiled and run normally using "gcc Part1.c" or "gcc Part2.c" followed by ./a.out 

Upon running Part 1, an output.csv file will be produced containing the statistics for part 1.


The test.sh file is used to verify the signed byte values, TBL hits and Page Faults of Part 1 given the addresses.txt file and correct.txt
to compare. Lines 159,167,168 of Part 1 should be uncommented and lines 170,171 should be commented for testing using test.sh 
Following this, the user types "sh test.sh" in the terminal to run the sh file

To see the page fault rate and tbl hit rate, lines 159,167,168 should be commented with lines 170,171 uncommented.


Upon running Part 2, an output2.csv file will be produced containing the statistics for part 2


