C_COMPILER     = gcc
C_OPTIONS      = -Wall -pedantic -g 
CUNIT_LINK     = -lcunit
SOURCES        = hash_table.c linked_list.c iterator.c merch.c utils.c db.c shopping_cart.c ui.c
OBJECTS        = hash_table.o linked_list.o iterator.o db.o merch.o utils.o shopping_cart.o ui.o


%.o: %.c 
	$(C_COMPILER) $(C_OPTIONS) $^ -c

merch_tests: $(OBJECTS) merch_tests.c
	$(C_COMPILER) $(C_OPTIONS) $^ $(CUNIT_LINK) -o merch_tests

memtest_merch: merch_tests
	valgrind -s --leak-check=full ./merch_tests < testfile_merch.txt

cart_tests: $(OBJECTS) cart_tests.c
	$(C_COMPILER) $(C_OPTIONS) $^ $(CUNIT_LINK) -o cart_tests

memtest_cart: cart_tests
	valgrind -s --leak-check=full ./cart_tests < cart_tests.txt

memtest: merch_tests cart_tests
	valgrind -s --leak-check=full ./merch_tests < testfile_merch.txt
	valgrind -s --leak-check=full ./cart_tests < cart_tests.txt

compile_program: $(OBJECTS)	
	$(C_COMPILER) $(C_OPTIONS) $^ -o program

run_program: compile_program
	valgrind -s --leak-check=full ./program

clean: 
	rm -f *.o *.gcda *.gcno *.gcov *.out *.h.gch cart_tests merch_tests program