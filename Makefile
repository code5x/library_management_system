CC = gcc
CFLAGS = -Wall -Wextra -std=c99

SRC = src/main.c \
      src/login.c \
      src/menu.c \
      src/book.c \
      src/member.c \
      src/issue.c \
      src/date_util.c \
      src/file_handler.c \
      src/logger.c \
      src/validator.c \
      src/id_generator.c \
      src/ui_util.c

INC = -Iinclude

OUT = library

$(OUT): $(SRC)
	$(CC) $(CFLAGS) $(SRC) $(INC) -lcrypt -o $(OUT)

clean:
	rm -f $(OUT)
