

OBJS 	= main.o list.o functions.o core_functions.o
SOURCE	= main.c list.c functions.c core_functions.c
HEADER  = list.h functions.h core_functions.h
OUT  	= catalogs
CC	= gcc
FLAGS   = -c  
# -c flag generates object code for separate files

all: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT)

# create/compile the individual files >>separately<< 
main.o: main.c
	$(CC) $(FLAGS) main.c

list.o: list.c
	$(CC) $(FLAGS) list.c
function.o: function.c
	$(CC) $(FLAGS) function.c
core_function.o: core_function.c
	$(CC) $(FLAGS) core_function.c

# clean house
clean:
	rm -f $(OBJS) $(OUT)

# do a bit of accounting
count:
	wc $(SOURCE) $(HEADER)

