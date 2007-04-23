SOURCES = \
	lts_main.c phoneset.c cart.c cart_search.c dict.c

HEADERS = \
phoneset.h cart.h cart_search.h dict.h

lts: $(SOURCES) $(HEADERS)
	gcc -O0 -g -o lts $(SOURCES)

cart.c cart.h: make_cart.scm ../stresstrain/train.tree
	festival -b make_cart.scm '(carttoC)'

clean:
	rm -rf lts