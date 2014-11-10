This folder is for unit tests.

The runtest.sh script runs all *.exe programs in the directory and returns 
number of failed ones, or zero if all of them passed.

Every test should be self-contained, i.e., not to expect any user input/output 
or command line options. It can, however, expect certain files to exist in the 
same directory and be available to reading/writing.

TODO: add checking for timeouts for individual tests with 'timeout' program.