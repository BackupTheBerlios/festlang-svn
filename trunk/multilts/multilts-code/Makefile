SOURCES = \
	lts_main.c \
	cmu_lts_model_english.c \
	cmu_lts_rules_english.c \
	cmu_lts_model_spanish.c \
	cmu_lts_rules_spanish.c \
	cmu_lts_model_russian.c \
	cmu_lts_rules_russian.c

HEADERS = \
	cmu_lts_model_english.h \
	cmu_lts_model_spanish.h \
	cmu_lts_model_russian.h \
	cst_lts.h

lts: $(SOURCES) $(HEADERS)
	gcc -O0 -g -o lts $(SOURCES)

clean:
	rm -rf lts