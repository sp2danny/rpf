
CC = clang++ -std=c++17 -stdlib=libstdc++ -Wall -Wextra -Wpedantic -Werror -ferror-limit=3 -static
LINK = -lpthread -s
CFLAGS = -DNDEBUG -O3

compile: rpf2

clean:
	rm obj/*.o

src/common.h: src/stringtools.h
	touch src/common.h

obj/main.o: Makefile src/main.cpp src/common.h src/stringtools.h src/purgecomment.h src/operators.h
	$(CC) $(CFLAGS) -c src/main.cpp -o obj/main.o

obj/linux.o: Makefile src/linux.cpp src/common.h
	$(CC) $(CFLAGS) -c src/linux.cpp -o obj/linux.o

obj/match.o: Makefile src/match.cpp src/common.h
	$(CC) $(CFLAGS) -c src/match.cpp -o obj/match.o

obj/misc.o: Makefile src/misc.cpp src/common.h
	$(CC) $(CFLAGS) -c src/misc.cpp -o obj/misc.o

obj/stringtools.o: Makefile src/stringtools.cpp src/stringtools.h
	$(CC) $(CFLAGS) -c src/stringtools.cpp -o obj/stringtools.o

obj/purgecomment.o: Makefile src/purgecomment.cpp src/purgecomment.h
	$(CC) $(CFLAGS) -c src/purgecomment.cpp -o obj/purgecomment.o

# opers
obj/and.o: Makefile src/common.h src/operators.h src/opers/and.cpp
	$(CC) $(CFLAGS) -c src/opers/and.cpp -o obj/and.o
obj/file.o: Makefile src/common.h src/operators.h src/opers/file.cpp
	$(CC) $(CFLAGS) -c src/opers/file.cpp -o obj/file.o
obj/line.o: Makefile src/common.h src/operators.h src/opers/line.cpp
	$(CC) $(CFLAGS) -c src/opers/line.cpp -o obj/line.o
obj/or.o: Makefile src/common.h src/operators.h src/opers/or.cpp
	$(CC) $(CFLAGS) -c src/opers/or.cpp -o obj/or.o
obj/not.o: Makefile src/common.h src/operators.h src/opers/not.cpp
	$(CC) $(CFLAGS) -c src/opers/not.cpp -o obj/not.o
obj/dir.o: Makefile src/common.h src/operators.h src/opers/dir.cpp
	$(CC) $(CFLAGS) -c src/opers/dir.cpp -o obj/dir.o
obj/near.o: Makefile src/common.h src/operators.h src/opers/near.cpp
	$(CC) $(CFLAGS) -c src/opers/near.cpp -o obj/near.o
obj/bm.o: Makefile src/common.h src/operators.h src/opers/bm.cpp
	$(CC) $(CFLAGS) -c src/opers/bm.cpp -o obj/bm.o
obj/regex.o: Makefile src/common.h src/operators.h src/opers/regex.cpp
	$(CC) $(CFLAGS) -c src/opers/regex.cpp -o obj/regex.o
obj/sub.o: Makefile src/common.h src/operators.h src/opers/sub.cpp
	$(CC) $(CFLAGS) -c src/opers/sub.cpp -o obj/sub.o
obj/true.o: Makefile src/common.h src/operators.h src/opers/true.cpp
	$(CC) $(CFLAGS) -c src/opers/true.cpp -o obj/true.o
obj/inv.o: Makefile src/common.h src/operators.h src/opers/inv.cpp
	$(CC) $(CFLAGS) -c src/opers/inv.cpp -o obj/inv.o

OBJS = obj/main.o obj/stringtools.o obj/purgecomment.o obj/linux.o obj/match.o obj/misc.o \
obj/and.o obj/file.o obj/line.o obj/or.o obj/not.o obj/dir.o obj/near.o obj/bm.o obj/regex.o obj/sub.o obj/true.o obj/inv.o

rpf2: Makefile $(OBJS)
	$(CC) -o rpf2 -static-libstdc++ -static-libgcc $(OBJS) $(LINK)

