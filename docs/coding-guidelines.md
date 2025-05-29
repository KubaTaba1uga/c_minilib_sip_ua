1. Each module internal functions follow this naming convention:
- cmsu_<Camelcase struct name>_<func name>
2. Each module public functions follow this naming convention:
- cmsu_<Snakecase struct name>_<func name>
3. Each module has this layout:
- <module name>.c
- <module name>.h
- _internals/...
These .c and .h file are just interface for module internals.
It is important to include internals only in .c file, not in 
.h file, so other modules that include .h file do not get
internals by acident.
