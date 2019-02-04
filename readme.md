# LazyAttractor
hogehoge

# download and compile
    $ git clone https://github.com/TNishimoto/lazy_attractor
    $ cd lazy_attractor
    $ cmake -DCMAKE_BUILD_TYPE=Release .

You need libdivsufsort(https://github.com/y-256/libdivsufsort) and sdsl-lite(https://github.com/simongog/sdsl-lite) to excecute this program.

# executions

## msubstr.out
Output minimum strings in the input text file.  

usage: ./msubstr.out --input_file=string [options] ...  
options:  
  -i, --input_file     Input text file name (string)  
  -o, --output_file    (option) Output attractor file name(the default output name is 'input_file.msub') (string [=])  
  -t, --output_type    (option) Output mode(binary or text) (string [=binary])  
  -?, --help           print this message  

## lazy.out
Output attractors for the input text file by lazy algorithm.  

usage: ./lazy.out --input_file=string [options] ...  
options:  
  -i, --input_file      Input text file name (string)  
  -o, --output_file     (option) Output attractor file name(the default output name is 'input_file.lazy.attrs') (string [=])  
  -t, --output_type     (option) Output mode(binary or text) (string [=binary])  
  -m, --msubstr_file    (option) Minimal substrings file name(the default minimal substrings filename is 'input_file.msub') (string [=])  
  -?, --help            print this message  
## greedy.out
Output attractors for the input text file by greedy algorithm.  

usage: ./greedy.out --input_file=string [options] ...  
options:  
  -i, --input_file      Input text file name (string)  
  -o, --output_file     (option) Output attractor file name(the default output name is 'input_file.greedy.attrs') (string [=])  
  -t, --output_type     (option) Output mode(binary or text) (string [=binary])  
  -m, --msubstr_file    (option) Minimal substrings file name(the default minimal substrings filename is 'input_file.msub') (string [=])  
  -b, --block_size      (option) block size (unsigned int [=1000])  
  -?, --help            print this message  

## verify.out
Verify whether the input attractors is attractors for the input text file.  

usage: ./verify.out --input_file=string --attractor_file=string [options] ...  
options:  
  -i, --input_file        Input text file name (string)  
  -a, --attractor_file    Attractors file name (string)  
  -m, --msubstr_file      (option) Minimal substrings file name(the default minimal substrings filename is 'input_file.msub') (string [=])  
  -o, --output_file       (option) Error log file name (the default output name is 'input_file.verify.log') (string [=])  
  -?, --help              print this message  

## examples
  $ mkdir sample  
  $ echo -n abbababababababbababa > sample/a.txt  
  $ ./msubstr.out -i sample/a.txt -t text  
  
  >___________RESULT___________  
  >File : sample/a.txt  
  >Output1 : sample/a.txt.msub.txt  
  >Output2 : sample/a.txt.msub.txt.parents  
  >The length of the input text : 21  
  >The number of minimal substrings : 15  
  >Excecution time : 1ms[21chars/ms]  
  >_________________________________ 

$ less -e sample/a.txt.msub.txt
>"a" SA[0, 9] occ: [0..0][3..3][5..5][7..7][9..9][11..11][13..13][16..16][18..18][20..20]  
>"ab" SA[1, 9] occ: [0..1][3..4][5..6][7..8][9..10][11..12][13..14][16..17][18..19]  
>"aba" SA[1, 7] occ: [3..5][5..7][7..9][9..11][11..13][16..18][18..20]  
>"abab" SA[2, 7] occ: [3..6][5..8][7..10][9..12][11..14][16..19]  
>"ababa" SA[2, 6] occ: [3..7][5..9][7..11][9..13][16..20]  
>"ababab" SA[3, 6] occ: [3..8][5..10][7..12][9..14]  
>"abababa" SA[3, 5] occ: [3..9][5..11][7..13]  
>"ababababa" SA[3, 4] occ: [3..11][5..13]  
>"abababababa" SA[3, 3] occ: [3..13]  
>"b" SA[10, 20] occ: [1..1][2..2][4..4][6..6][8..8][10..10][12..12][14..14][15..15][17..17][19..19]  
>"ba" SA[10, 18] occ: [2..3][4..5][6..7][8..9][10..11][12..13][15..16][17..18][19..20]  
>"bab" SA[11, 18] occ: [2..4][4..6][6..8][8..10][10..12][12..14][15..17][17..19]  
>"babb" SA[18, 18] occ: [12..15]  
>"bb" SA[19, 20] occ: [1..2][14..15]  
>"bbababab" SA[20, 20] occ: [1..8]  
 
$ ./lazy.out -i sample/a.txt -t text
>___________RESULT___________  
>File : sample/a.txt  
>Output : sample/a.txt.lazy.attrs.txt  
>The length of the input text : 21  
>The number of minimal substrings : 15  
>The number of attractors : 3  
>Excecution time : 0ms[infchars/ms]  
>_________________________________  

$ less -e sample/a.txt.lazy.attrs.txt
>0,1,12  

$ ./greedy.out -i sample/a.txt -t text
>___________RESULT___________ 
>File : sample/a.txt  
>Output : sample/a.txt.greedy.attrs.txt  
>The length of the input text : 21  
>The number of minimal substrings : 15  
>The number of attractors : 2  
>Block size : 1000  
>Excecution time : 0ms[infchars/ms]  
>_________________________________  

$ less -e sample/a.txt.greedy.attrs.txt
>3,14  

$ ./lazy.out -i sample/a.txt -t binary  
$ ./verify.out -i sample/a.txt -a sample/a.txt.lazy.attrs  
>___________RESULT___________ 
>File : sample/a.txt  
>Attractor File : sample/a.txt.lazy.attrs  
>The length of the input text : 21  
>The number of minimal substrings : 15  
>The number of attractors : 3  
>Excecution time : 1ms[21chars/ms]  
>Attractor? : YES  
>_________________________________  

$ ./greedy.out -i sample/a.txt -t binary  
>___________RESULT___________ 
>File : sample/a.txt  
>Attractor File : sample/a.txt.greedy.attrs  
>The length of the input text : 21  
>The number of minimal substrings : 15  
>The number of attractors : 2  
>Excecution time : 1ms[21chars/ms]  
>Attractor? : YES  
>_________________________________ 

$ echo -n 1, 6, 10, 18 > sample/a.attrs.txt  
$ ./verify.out -i sample/a.txt -a sample/a.attrs.txt  
>___________RESULT___________ 
>File : sample/a.txt  
>Attractor File : sample/a.attrs.txt  
>The length of the input text : 21  
>The number of minimal substrings : 15  
>The number of attractors : 4  
>Excecution time : 0ms[infchars/ms]  
>Attractor? : NO  
>Output minimal substrings not containing the input positions.  
>See also sample/a.txt.verify.log  
>_________________________________ 

$ less -e sample/a.txt.verify.log  
>"babb" occs:[12..15]
