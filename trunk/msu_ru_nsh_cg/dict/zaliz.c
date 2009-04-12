#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *alph =
	"АаБбВвГгДдЕеЖжЗзИиЙйКкЛлМмНнОоПпРрСсТтУуФфХхЦцЧчШшЩщЪъЫыЬьЭэЮюЯяЁё";

int
cmp (const void *s1, const void *s2)
{
	char *a, *b;
	int i, j, i2, j2;

	a = *((char **) s1);
	b = *((char **) s2);

	for (i = j = 0; 1; i += 2, j += 2) {
		if (a[i] != '\xd0' && a[i] != '\xd1') {
			i2 = a[i];
			i--;
		} else {
			for (i2 = 0; i2 < 33 * 4; i2 += 4)
				if (alph[i2 + 2] == a[i]
				    && alph[i2 + 3] == a[i + 1])
					break;
			i2 = i2 / 4 + 500;
		}

		if (b[j] != '\xd0' && b[j] != '\xd1') {
			j2 = b[j];
			j--;
		} else {
			for (j2 = 0; j2 < 33 * 4; j2 += 4)
				if (alph[j2 + 2] == b[j]
				    && alph[j2 + 3] == b[j + 1])
					break;
			j2 = j2 / 4 + 500;
		}
		if (i2 != j2)
			return (i2 < j2) ? -1 : 1;
		if (i2 == 0 && j2 == 0)
			return 0;
	}
}


int
main (void)
{
	FILE *file1, *file2;
	char t[10000];
	char s[10000];
	char **flex, **acc, **pre, *p, *p2, *p3;
	long i, j, k, m, np, nflex, nacc, nlemm, nlemm2, npre, vow, yo;
	long iflex, iacc, ipre;
	char an[10], type[30], cur[100], prev[100];
	char **pairs;
	char **lem;

	printf ("MNCL\n");
	file1 = fopen ("rgramtab.tab.modified", "rt");

	pairs = (char **) malloc (sizeof (char *) * 4);

	for (i = 0; fgets (t, 10000, file1);) {
		if ((*t > 0 && *t < 33) || *t == '/')
			continue;
		pairs[i + 1] = strdup (strtok (t, " "));
		pairs[i] = strdup (strtok (0, " "));
		i += 2;
		pairs = (char **) realloc (pairs, sizeof (char *) * (i + 6));
	}

	np = i / 2;
	fclose (file1);

	file1 = fopen ("morphs.mrd", "rt");
	fgets (t, 10000, file1);
	nflex = atol (t);

	flex = (char **) malloc (sizeof (char *) * nflex);
	for (i = 0; i < nflex; i++) {
		fgets (t, 10000, file1);
		flex[i] = malloc (strlen (t) + 1);
		strcpy (flex[i], t);
	}

	fgets (t, 10000, file1);
	nacc = atol (t);

	acc = (char **) malloc (sizeof (char *) * nacc);
	for (i = 0; i < nacc; i++) {
		fgets (t, 10000, file1);
		acc[i] = malloc (strlen (t) + 1);
		strcpy (acc[i], t);
	}

	fgets (t, 10000, file1);
	npre = atol (t);
	for (i = 0; i < npre; i++) {
		fgets (t, 10000, file1);
	}

	fgets (t, 10000, file1);
	npre = atol (t);

	pre = (char **) malloc (sizeof (char *) * npre);
	for (i = 0; i < npre; i++) {
		fgets (t, 10000, file1);
		pre[i] = malloc (strlen (t) + 1);
		strcpy (pre[i], t);
	}

	fgets (t, 10000, file1);
	nlemm = atol (t);

	lem = (char **) malloc (sizeof (char *) * 5000000);
	for (nlemm2 = 0; fgets (t, 10000, file1);) {
		strtok (t, " ");
		if (*t == '#')
			memmove (t, t + 1, strlen (t) + 1);
		iflex = atol (strtok (0, " "));
		iacc = atol (strtok (0, " "));
		strtok (0, " ");
		strtok (0, " ");
		ipre = atol (strtok (0, " \r\n"));

		if (*t == '-')
			memmove (t, t + 1, strlen (t) + 1);

		p = strdup (flex[iflex] + 1);
		p3 = acc[iacc];
		strtok (p, "%\n\r");
		do {
			p2 = strchr (p, '*');
			*p2 = 0;

			*s = 0;
			if (strchr (p2 + 1, '*')) {
				strcpy (s, strchr (p2 + 1, '*') + 1);
				*strchr (p2 + 1, '*') = 0;
			}
			strcpy (an, p2 + 1);
			strcat (s, t);
			strcat (s, p);
			for (j = vow = yo = 0; s[j]; j += 2) {
				if (s[j] != '\xd0' && s[j] != '\xd1') {
					j--;
					continue;
				}
				if (s[j] == '\xd0' && (s[j + 1] == '\x90'	//а
						       || s[j + 1] == '\xaf'	//я
						       || s[j + 1] == '\x9e'	//о
						       || s[j + 1] == '\x81'	//ё
						       || s[j + 1] == '\xa3'	//у
						       || s[j + 1] == '\xae'	//ю
						       || s[j + 1] == '\xab'	//ы
						       || s[j + 1] == '\x98'	//и
						       || s[j + 1] == '\xad'	//э
						       || s[j + 1] == '\x95'	//е
				    ))
					vow++;
				if (s[j] == '\xd0' && s[j + 1] == '\x81')
					yo = j + 1;
				for (k = 0; k < 33 * 4; k += 4)
					if (alph[k] == s[j]
					    && alph[k + 1] == s[j + 1]) {
						s[j] = alph[k + 2];
						s[j + 1] = alph[k + 3];
					}
			}

			if (atoi (p3) != 255 && *s != 'I') {
				sprintf (cur, "(\"%s\" ", s);
				strcpy (type, "v");
				for (j = 0; j < np; j++) {
					if (strcmp (an, pairs[j * 2]) == 0) {
						strcpy (type,
							pairs[j * 2 + 1]);
					}
				}
				sprintf (strchr (cur, 0), "%s (%li))\n", type,
					 vow - atoi (p3));
				lem[nlemm2++] = strdup (cur);
				if (yo) {
					cur[yo + 1] = '\xd0';
					cur[yo + 2] = '\xb5';
					strcpy (strstr (cur, "))") + 1,
						" fix_yo)\n");
					lem[nlemm2++] = strdup (cur);
				}
			}
			if (strchr (p3, ';'))
				p3 = strchr (p3, ';') + 1;
		}
		while ((p = strtok (0, "%\n\r")));
		free (p);
	}
	fclose (file1);

	file1 = fopen ("missing", "rt");
	while (fgets (cur, 10000, file1))
		lem[nlemm2++] = strdup (cur);
	fclose (file1);

	qsort (lem, nlemm2, sizeof (char *), cmp);
	file2 = fopen ("doubles", "wt");
	for (k = m = 0; k < nlemm2; k++) {
		i = strchr (lem[k], ' ') - lem[k];
		j = strchr (prev, ' ') - prev;
		if (i == j)
			if (strncmp (lem[k], prev, i) == 0)
				if (atol (strchr (lem[k] + 1, '(') + 1) !=
				    atol (strchr (prev + 1, '(') + 1)
				    || (strstr (lem[k], "fix_yo") != NULL) !=
				    (strstr (prev, "fix_yo") != NULL)) {
					fputs (prev, file2);
					fputs (lem[k], file2);
				}
		if (strncmp (lem[k], prev, i) != 0) {
			printf ("%s", lem[k]);
			m++;
		}
		strcpy (prev, lem[k]);
	}
	fprintf (stderr, "%li entries written\n", m);
	fclose (file2);

	return 0;
}
