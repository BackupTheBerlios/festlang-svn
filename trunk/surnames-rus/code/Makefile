SOURCES = \
	lts.c phoneset.c cart.c cart_search.c

HEADERS = \
phoneset.h cart.h cart_search.h

lts: $(SOURCES) $(HEADERS)
	gcc -O0 -g -o lts $(SOURCES)

cart.c cart.h: make_cart.scm train.tree.scm
	festival -b make_cart.scm train.tree.scm '(carttoC my_stress_tree )'

clean:
	rm -rf lts