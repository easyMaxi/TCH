TCH is a tool aimed to be used to test cryptohardware.
Currently there are 25 testcases to cover the basics of PKCS #11.

building
--------
gcc -Wall -o "tch" "tch.c" -ldl -g

using
--------
Usage: tch [-o] [-i] [-v] [-m file] [-u User PIN] [-s SO PIN]

-o: overrides object on your token. Do NOT use is if there are ANY object that are important in any way.
-i: tells tch to try to initialise the token. Be carefull with this option. Obviously you need -s in this case
-v: more information
-m file: Path to the module
-u PIN: User PIN
-s PIN: Security Officer PIN




TCH is still in an early version. Do not use it with any token that is in use!
In some cases TCH uses the CKF_RW_SESSIOn flag for sessions, even if -o is not ommited.
