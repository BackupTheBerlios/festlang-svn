SOURCES = \
	lts_main.c cmu_lts_model.c cmu_lts_rules.c

HEADERS = \
cmu_lts_model.h cst_lts.h

lts: $(SOURCES) $(HEADERS)
	gcc -O0 -g -o lts $(SOURCES)

clean:
	rm -rf lts