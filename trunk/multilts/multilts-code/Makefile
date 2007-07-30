SOURCES = \
	lts_main.c \
	cmu_cart.c \
	cmu_values.c

HEADERS = \
	cmu_cart.h \
	cst_lts.h

lts: $(SOURCES) $(HEADERS)
	gcc -O0 -g -o lts $(SOURCES)

clean:
	rm -rf lts