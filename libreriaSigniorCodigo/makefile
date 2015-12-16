RM=rm -rf
CC=gcc

C_SRCS=$(shell find . -iname "*.c" | tr '\n' ' ')
H_SRCS=$(shell find . -iname "*.h" | tr '\n' ' ')
OBJS=$(C_SRCS:./%.c=%.o)

# Clean and compile .so
all: build/libsigniorCodigo.so

build/libsigniorCodigo.so: $(OBJS)
	$(CC) -shared -o "libsigniorCodigo.so" $(OBJS)

%.o: ./%.c
	$(CC) -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<" -lcommons -lpthread

clean:
	$(RM) ./*.o
	$(RM) ./*.d
	$(RM) ./libsigniorCodigo.so
	

install: all
	cp -u ./libsigniorCodigo.so /usr/lib
	mkdir -p /usr/include/signiorCodigo/
	cp -u $(H_SRCS) /usr/include/signiorCodigo/

uninstall: clean
	rm -f /usr/lib/libsigniorCodigo.so
	rm -rf /usr/include/signiorCodigo

.PHONY: all clean install uninstall