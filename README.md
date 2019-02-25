
# Reverse Polish Find : Summary Help

### 1a. File Operands (operands produce results)
|operand|meaning|
| ----- | ----- |
|f(expr)|filename matches expr using *? matching, case insensitive|
|+(expr)|like file match above, but c++ only, removes line/block comments and string/character literals from search text|
|d(expr)|directory matches expr using *? matching, case insensitive|
|m(expr)|file modified at or after 'expr', formated as 'YYYY-MM-DD'|
|x      |file is a text file. slow, uses mime-types|

### 1b. Text Operands (produce results, and populates match-lines)
|operand|meaning|
| ----- | ----- |
|l(expr)|whole line matches expr using *? matching, case sensitive|
|L(expr)|whole line matches expr using *? matching, case insensitive|
|r(expr)|part of line matches expr using regex matching|
|b(expr)|part of line matches expr using BoyerMoore matching, no *? matching|
|B(expr)|part of line matches expr using BoyerMoore matching, no *? matching, case insesitive|
|i(expr)|identifyer matching, case sensitive, no *? matching|
|I(expr)|identifyer matching, case insensitive, no *? matching|

### 2. Operators (operators combine results)
|operator|meaning|
| ------ | ----- |
|a |and's 2 results, merges match-lines|
|o |or's 2 results, merges match-lines|
|n |near, ie and's 2 results, remove match-lines that are more apart than 5 lines|
|nN|near, ie and's 2 results, remove match-lines that are more apart than N lines|
|! |negates's 1 results, purge match-lines|
|- |subtracts later match-lines from former|
|~ |inverts match-lines from result|

### 3. Misc
|operand|meaning|
| ----- | ----- |
|t      |always true, does not populate match-lines|

### 4. Options (specify with --option-on or --option-off)
|option|meaning|
| ----- | ----- |
|sparse|produce output without separating blank lines. default on|
|color |produce output with colorcoded matches. default on if output is a tty|
|stats |summary statistic printed. default on|

### Notes
    Only l, r, i and b (and uppercase variants) loads the file-content  
    Many operators and operands need to be escaped or quoted  

