# LazyAttractor

This program provides two approximation algoirthms for string attractors (lazy.out and greedy.out), and verification algorithm whether the given positions are string attractors for the given text (verify.out).

## Download
The source codes in 'module' directory are maintained in different repositories. 
So, to download all the necessary source codes, do the following:

> git clone https://github.com/TNishimoto/lazy_attractor.git  
> cd lazy_attractor  
> git submodule init  
> git submodule update 

# compile
> mkdir build  
> cd build  
> cmake -DCMAKE_BUILD_TYPE=Release ..  
> make  

You need sdsl-lite(https://github.com/simongog/sdsl-lite) to excecute this program. 
Please edit CMakeLists.txt to set SDSL library and include directory paths.

# preliminaries

hoghoge. $O(n)$ time.

# executions


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
  -?, --help            print this message  

## verify.out
Verify whether the input attractors is attractors for the input text file.  

usage: ./verify.out --input_file=string --attractor_file=string [options] ...  
options:  
  -i, --input_file        Input text file name (string)  
  -a, --attractor_file    Attractors file name (string)  
  -m, --msubstr_file      (option) Minimal substrings file name(the default minimal substrings filename is 'input_file.msub') (string [=])  
  -t, --attractor_file_type    (option) Input attractor file type(binary or text) (string [=binary])  
  -o, --output_file       (option) Error log file name (the default output name is 'input_file.verify.log') (string [=])  
  -?, --help              print this message  

## examples
$ mkdir sample  
$ echo -n abbababababababbababa > sample/a.txt  
  
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
$ ./verify.out -i sample/a.txt -a sample/a.txt.greedy.attrs  
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
$ ./verify.out -i sample/a.txt -a sample/a.attrs.txt -t text
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
