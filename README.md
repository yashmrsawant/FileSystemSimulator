****************************************************************************
*Group :
* 1) Abhilash Kurmi
* 2) Munendra Singh
* 3) Rahul Ojha
* 4) Yash M Sawant
****************************************************************************


First Compile This
1) gcc -g -c -Wall -o stack stack.c
2) gcc -g -c -Wall -o fsshelper fsshelper.c
3) gcc -g -lm -pthread -Wall -o fssserver fsserver.c stack fsshelper
4) gcc -g -Wall -o fssclient fssclient.c
5) gcc -g -lm -Wall -o sbwriter superblockwriter.c fsshelper

Then Run This to initialize Filesystem with with given filename as argument.
./sbwriter sdb

Now a sdb is created.
We had written a dummy code to changeToPartition in fsshelper which helps
to take care of partition issue within device(file) sdb.

Now we can run fssserver which is the server file
./fssserver sdb

Simutaneously on another terminal(locally) we can run
./fssclient

Sample Output
1) ls 2) mkfile 3) mkdir

For this we just have to input a choice number, for example if we want to do
ls then type 1.
Then it will ask for Parent path :
We can enter /

Example 2)
For mkfile
We have to enter first the Parent path where we are storing in our fss in this
case partition 0 in sdb.
Then we have to enter path in our original file system where this file path
can be treated as source
Then we have to enter file name as such we want to save in our fss.

Example 3)
For mkdir
We have to enter first the Parent path where we are storing in our fss.
Then we have to enter the name of the diretory.


fssclient can be run simultaneously with 5 parallel running.
