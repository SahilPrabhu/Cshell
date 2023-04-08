# Cshell 

Cshell is a simple implementation of a shell in C, It demonstrates the basics of how a shell works.
That is: read, parse, fork, exec, and wait.  Since its purpose is demonstration
(not feature completeness or even fitness for casual use), it has many
limitations, including:

* Commands must be on a single line.
* Arguments must be separated by whitespace.
* No quoting arguments or escaping whitespace.
* No piping or redirection.
* Only builtins are: `cd`, `help`, `exit`.

Made this with the approach of project based learning. Learnt a lot about how shell works and about functions in C in general.

Use `gcc -o main src/main.c` to compile, and then `./main` to run.
