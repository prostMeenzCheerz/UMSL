This program reads through a file, or text entered on the command line, to determine whether the given grammar is correct.

"testScanner" is called from main, and is the driver of the program.  Once the program is inside "testScanner", "testScanner" will call
"scanner".

"scanner" will then iterate through an input file, one letter at a time, in an attempt to build a token based on certain criteria 
for the given grammar.
Once a token is found, it stored in a struct containing the instance of the token, the type of token, and the line number it is found on.
If "scanner" finds an error while build the token, the program will exit with an error message detailing the incorrect character or digit
and the line number the error ocurred.
"testScanner" will call "scanner" repeatedly until the EOF token is read.  It will then call "filter" to determine whether or not any
of the tokens found is a keyword.
