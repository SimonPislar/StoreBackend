Authors: Simon Pislar and Noah Wassberg

This program is a back-end system for a store. You can add item information, shopping carts, 
storage locations for different items, add items to carts, check out carts and more.

Compiling the program:
    To compile the program, make sure that you are positioned inside the inlupp2-folder.
    Compile the program by typing "make compile_program" in the terminal. This will compile all 
    necessary files  and create an executable file called "program". The compiler used is GCC.

Running the tests:
    To run the tests, make sure that you are positioned inside the inlupp2-folder.
    Type "make memtest_cart" to run all tests relating to the cart-functions and type 
    "make memtest" to run all tests relating to the db-functions. All tests are being run by Valgrind
    with the option --leak-check=full enabled.

Running the program:
    To run the program, make sure that you are positioned inside the inlupp2-folder.
    The program can be run by typing "make run_program" in the terminal. The program will be
    compiled with GCC and then ran by Valgrind with the option --leak-check=full enabled. No files
    other than the files located in the inlupp2-folder are necessary to run the program.

Code coverage (obtained using gcov):


mart_tests.c coverage report

------------------------------------------------------------------------------
                           GCC Code Coverage Report
Directory: .
------------------------------------------------------------------------------
File                                       Lines    Exec  Cover   
------------------------------------------------------------------------------
cart_tests.c                                 135     130    96%   
db.c                                         297     135    45%   
hash_table.c                                 184      89    48%   
iterator.c                                    21      19    90%   
linked_list.c                                120      71    59%   
merch.c                                       23      17    73%   
shopping_cart.c                              224     194    86%   
ui.c                                          83      10    12%   
utils.c                                       77      55    71%   
------------------------------------------------------------------------------
TOTAL                                       1164     720    61%
------------------------------------------------------------------------------


merch_tests.c coverage report


------------------------------------------------------------------------------
                           GCC Code Coverage Report
Directory: .
------------------------------------------------------------------------------
File                                       Lines    Exec  Cover 
------------------------------------------------------------------------------
db.c                                         305     213    69%  
hash_table.c                                 184      89    48%  
iterator.c                                    21      18    85%  
linked_list.c                                120      65    54%   
merch.c                                       24      24   100%   
merch_tests.c                                105     100    95%   
shopping_cart.c                              219       8     3%   
ui.c                                          84      30    35%   
utils.c                                       77      53    68%   
------------------------------------------------------------------------------
TOTAL                                       1139     600    52%
------------------------------------------------------------------------------


Code from different sources:
    Hash table: Noah Wassberg
    Linked list: Noah Wassberg
    Utils: Simon Pislar
