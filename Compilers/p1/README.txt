This program reads through a file, or text entered on the command line, to determine whether the given grammar is correct.

"testScanner" is called from main, and is the driver of the program.  Once the program is inside "testScanner", "testScanner" will call
"scanner".

"scanner" will then iterate through a file, grabbing one character at a time.  It will then call filter to determine whether or not the 
the given character can be used to properly build a token.

Once a token is found, it is stored in a struct containing the instance of the token, the type of token, and the line number it is found on.
If "scanner" finds an error while attempting to build the token, the program will exit with an error message detailing the incorrect character or digit and the line number the error ocurred.

"testScanner" will call "scanner" repeatedly until the EOF token is read.  It will then call "filter2" to determine whether or not any
of the tokens found is a keyword.

Finally, all of the tokens, and the token information stored in the struct, will be printed to an output file.
