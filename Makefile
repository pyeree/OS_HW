CC = gcc
CFLAGS = -Wall -g
TARGET = program
SRCS = main.c cat.c cd.c clear.c diff.c ls.c touch.c tree_io.c whereis.c
# SRCS = main.c cat.c cd.c chmod.c clear.c diff.c ls.c mkdir.c mv.c pwd.c touch.c tree_io.c whereis.c # 모든 소스 파일
OBJS = $(SRCS:.c=.o)
DEPS = header.h tree_io.h cat.h cd.h clear.h diff.h ls.h touch.h whereis.h
# DEPS = header.h tree_io.h cat.h cd.h chmod.h clear.h diff.h ls.h mkdir.h mv.h pwd.h touch.h whereis.h # 모든 헤더 파일

#### Notice ####
# main에 기능 구현 후 여기서 해당하는 object 파일 부분을 주석 해제해야 합니다!
# ex. main.c에서 cd 명령어 호출 부분 작성 후 cd.o 주석 해제
# 이후 make 명령어를 통해 컴파일하면 program 이라는 파일이 생성되며, ./program을 통해 실행할 수 있습니다.

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

main.o: main.c header.h tree_io.h cat.h cd.h clear.h diff.h ls.h mv.h pwd.h touch.h whereis.h
# main.o: main.c header.h tree_io.h cat.h cd.h chmod.h clear.h diff.h ls.h mkdir.h mv.h pwd.h touch.h whereis.h
	$(CC) $(CFLAGS) -c $< -o $@

cat.o: cat.c cat.h header.h tree_io.h
	$(CC) $(CFLAGS) -c $< -o $@

cd.o: cd.c cd.h header.h tree_io.h
	$(CC) $(CFLAGS) -c $< -o $@

# chmod.o: chmod.c chmod.h header.h
# 	$(CC) $(CFLAGS) -c $< -o $@

clear.o: clear.c clear.h header.h
	$(CC) $(CFLAGS) -c $< -o $@

diff.o: diff.c diff.h header.h
	$(CC) $(CFLAGS) -c $< -o $@

ls.o: ls.c ls.h header.h tree_io.h
	$(CC) $(CFLAGS) -c $< -o $@

# mkdir.o: mkdir.c mkdir.h header.h tree_io.h
# 	$(CC) $(CFLAGS) -c $< -o $@

# mv.o: mv.c mv.h header.h cd.h tree_io.h
# 	$(CC) $(CFLAGS) -c $< -o $@

# pwd.o: pwd.c pwd.h header.h tree_io.h
# 	$(CC) $(CFLAGS) -c $< -o $@

touch.o: touch.c touch.h header.h
	$(CC) $(CFLAGS) -c $< -o $@

tree_io.o: tree_io.c tree_io.h header.h
	$(CC) $(CFLAGS) -c $< -o $@

whereis.o: whereis.c whereis.h header.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJS)