
CC = clang++ -std=c++17 -stdlib=libstdc++ -Wall -Wextra -Wpedantic -Werror -ferror-limit=3 -static
LINK = -lpthread -s
CFLAGS = -DNDEBUG -O3

compile: rpf2

clean:
	rm obj/*.o

src/common.h: src/stringtools.h
	touch src/common.h

obj/main.o: Makefile src/main.cpp src/common.h src/stringtools.h src/purgecomment.h src/execute.h
	$(CC) $(CFLAGS) -c src/main.cpp -o obj/main.o

obj/linux.o: Makefile src/linux.cpp src/common.h
	$(CC) $(CFLAGS) -c src/linux.cpp -o obj/linux.o

obj/execute.o: Makefile src/execute.cpp src/common.h src/execute.h
	$(CC) $(CFLAGS) -c src/execute.cpp -o obj/execute.o

obj/stringtools.o: Makefile src/stringtools.cpp src/stringtools.h
	$(CC) $(CFLAGS) -c src/stringtools.cpp -o obj/stringtools.o

obj/purgecomment.o: Makefile src/purgecomment.cpp src/purgecomment.h
	$(CC) $(CFLAGS) -c src/purgecomment.cpp -o obj/purgecomment.o

OBJS = obj/main.o obj/stringtools.o obj/purgecomment.o obj/linux.o obj/execute.o

rpf2: Makefile $(OBJS)
	$(CC) -o rpf2 -static-libstdc++ -static-libgcc $(OBJS) $(LINK)

