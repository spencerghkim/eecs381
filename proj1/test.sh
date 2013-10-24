#!/bin/bash

make clean
make
./p1Lexe < normal_in.txt > out.txt 
diff out.txt normal_out_list.txt 
./p1Aexe < normal_in.txt > out.txt 
diff out.txt normal_out_array.txt 
./p1Lexe < errors_in.txt > out.txt 
diff out.txt errors_out.txt 
./p1Aexe < errors_in.txt > out.txt 
diff out.txt errors_out.txt 
./p1Lexe < typeahead_in.txt > out.txt 
diff out.txt typeahead_out.txt 
./p1Aexe < typeahead_in.txt > out.txt 
diff out.txt typeahead_out.txt
 
