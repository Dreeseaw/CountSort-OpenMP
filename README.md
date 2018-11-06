# CountSort-OpenMP
## An OpenMP implementation of the count sort algorithm

For my OpenMP code, I use a private middle array to calculate the total appearances of each value in a given subsection of the unsorted array, and apply prefix sums in each thread. After prefix sums, I have to reduce each array index to a total array to use for the last part of my code. The trick here was to use a ‘for’ pragma with a ‘reduction(+:mid_array(K)' clause. Here my for loop ranged from 0 to the number of threads being used, so each thread got one instance of the for loop, which performed a reduction across the array wiht the respective thread's version of the middle array.

My trick to create more parallelism in the final part of the algorith. was to assign a range of K values to a certain thread, so that different threads weren’t trying to read and write from the same index simultaneously. To further alleviate this, I added a padding of 8 (value of CPAD) to each element of the middle array. These two combined allowed me to parallelise this last part of the code.

Here's some tests showing the implementation using N threads of dual 10- or 12-core Xeon V2 E5-2680 processors.
These tests sort a 100-million element array, with integers ranging 0-100K. 

| Serial  | 2 Threads |   4   |   6   |   8   |   10   |   12   |   14   |   16   |   18   |   20   |
| ------- | --------- | ----- | ----- | ----- | ------ | ------ | ------ | ------ | ------ | ------ |
| 2.50s   |   1.32s   | 1.23s | 0.98s | 0.84s |  0.77s |  0.73s |  0.71s |  0.69s |  0.69s |  0.71s |
