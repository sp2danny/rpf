
CC = clang++ -std=c++17 -stdlib=libstdc++ -Wall -Wextra -Wpedantic -Werror -ferror-limit=3 -static
LINK = -lpthread -s
CFLAGS = -DNDEBUG -O3
#CFLAGS = -D_DEBUG -g

compile: rpf

clean:
	rm obj/*.o

src/common.h: src/stringtools.h src/clone_ptr.hpp
	touch src/common.h

obj/main.o: src/main.cpp src/common.h src/purgecomment.h src/platform.h src/OFM.h src/container_operations.hpp
	$(CC) $(CFLAGS) -c src/main.cpp -o obj/main.o

obj/linux.o: src/linux.cpp src/common.h src/platform.h
	$(CC) $(CFLAGS) -c src/linux.cpp -o obj/linux.o

obj/match.o: src/match.cpp src/common.h src/OFM.h
	$(CC) $(CFLAGS) -c src/match.cpp -o obj/match.o

obj/misc.o: src/misc.cpp src/common.h src/OFM.h
	$(CC) $(CFLAGS) -c src/misc.cpp -o obj/misc.o

obj/stringtools.o: src/stringtools.cpp src/stringtools.h
	$(CC) $(CFLAGS) -c src/stringtools.cpp -o obj/stringtools.o

obj/purgecomment.o: src/purgecomment.cpp src/purgecomment.h
	$(CC) $(CFLAGS) -c src/purgecomment.cpp -o obj/purgecomment.o

OPERDEP = src/common.h src/operators.h src/OFM.h

# opers
obj/and.o: $(OPERDEP) src/opers/and.cpp
	$(CC) $(CFLAGS) -c src/opers/and.cpp -o obj/and.o
obj/file.o: $(OPERDEP) src/opers/file.cpp
	$(CC) $(CFLAGS) -c src/opers/file.cpp -o obj/file.o
obj/line.o: $(OPERDEP) src/opers/line.cpp
	$(CC) $(CFLAGS) -c src/opers/line.cpp -o obj/line.o
obj/or.o: $(OPERDEP) src/opers/or.cpp
	$(CC) $(CFLAGS) -c src/opers/or.cpp -o obj/or.o
obj/not.o: $(OPERDEP) src/opers/not.cpp
	$(CC) $(CFLAGS) -c src/opers/not.cpp -o obj/not.o
obj/dir.o: $(OPERDEP) src/opers/dir.cpp
	$(CC) $(CFLAGS) -c src/opers/dir.cpp -o obj/dir.o
obj/near.o: $(OPERDEP) src/opers/near.cpp
	$(CC) $(CFLAGS) -c src/opers/near.cpp -o obj/near.o
obj/bm.o: $(OPERDEP) src/opers/bm.cpp
	$(CC) $(CFLAGS) -c src/opers/bm.cpp -o obj/bm.o
obj/regex.o: $(OPERDEP) src/opers/regex.cpp
	$(CC) $(CFLAGS) -c src/opers/regex.cpp -o obj/regex.o
obj/sub.o: $(OPERDEP) src/opers/sub.cpp
	$(CC) $(CFLAGS) -c src/opers/sub.cpp -o obj/sub.o
obj/true.o: $(OPERDEP) src/opers/true.cpp
	$(CC) $(CFLAGS) -c src/opers/true.cpp -o obj/true.o
obj/inv.o: $(OPERDEP) src/opers/inv.cpp
	$(CC) $(CFLAGS) -c src/opers/inv.cpp -o obj/inv.o
obj/mod.o: $(OPERDEP) src/opers/mod.cpp src/platform.h
	$(CC) $(CFLAGS) -c src/opers/mod.cpp -o obj/mod.o
obj/ident.o: $(OPERDEP) src/opers/ident.cpp
	$(CC) $(CFLAGS) -c src/opers/ident.cpp -o obj/ident.o
obj/range.o: $(OPERDEP) src/opers/range.cpp
	$(CC) $(CFLAGS) -c src/opers/range.cpp -o obj/range.o
obj/misc_oper.o: $(OPERDEP) src/opers/misc.cpp
	$(CC) $(CFLAGS) -c src/opers/misc.cpp -o obj/misc_oper.o

OBJS = obj/main.o obj/stringtools.o obj/purgecomment.o obj/linux.o obj/match.o obj/misc.o \
obj/and.o obj/file.o obj/line.o obj/or.o obj/not.o obj/dir.o obj/near.o obj/bm.o \
obj/regex.o obj/sub.o obj/true.o obj/inv.o obj/mod.o obj/ident.o obj/misc_oper.o \
obj/range.o

rpf: $(OBJS)
	$(CC) -o rpf -static-libstdc++ -static-libgcc $(OBJS) $(LINK)

