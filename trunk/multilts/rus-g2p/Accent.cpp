//================================
// ==========   Dialing Lemmatizer (www.aot.ru)
//================================

#pragma warning(disable:4786)
#pragma warning(disable:4530)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <locale.h>

#include "../common/utilit.h"
#include "../AgramtabLib/RusGramTab.h"
#include "../AgramtabLib/EngGramTab.h"
#include "../AgramtabLib/GerGramTab.h"
#include "../LemmatizerLib/Lemmatizers.h"
#include "../LemmatizerLib/Paradigm.h"

bool				bPrintIds = true;;
MorphLanguageEnum	Language;
CLemmatizer*		pLemmatizer;
CAgramtab*			pAgramtab;

string GetGrammems (const char* tab_str)
{
	QWORD G;
	pAgramtab->GetGrammems(tab_str, G);
	string s = pAgramtab->GrammemsToStr (G);
	if (!s.empty() && (s[s.length() -1] == ','))
		s.erase(s.length() - 1);
	return s;
}


void RussianConvertToDictionary (std::string& S)
{
	#ifdef WIN32
		OemToCharBuff((char*)S.c_str(), (char*)S.c_str(), S.length());		 
	#else
		KOI8ToWin(S);
	#endif

};


void RussianConvertToScreen (std::string& S)
{
	#ifdef WIN32
		CharToOemBuff((char*)S.c_str(), (char*)S.c_str(), S.length());		 
	#else
		WinToKOI8(S);
	#endif

};

char getLowChar(char c)
{
    char rusBig [34];    
    char rusSml [34];
    strcpy(rusBig, "АБВГДЕЁЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯ");
    strcpy(rusSml, "абвгдеёжзийклмнопрстуфхцчшщъыьэюя");
    
    for (int i=0; i < 33; i++)
	if (rusBig[i] == c)
	    return rusSml[i];	    

    return c;
}


void lowercase(std::string& S) 
{
    char* s = (char*)S.c_str();    

    for(unsigned int i = 0; i < S.length(); i++){
	s[i] = getLowChar(s[i]);
    }
}


std::string  GetMorphInfo (std::string  Form)
{
	
	std::string  Result;

	bool bCapital   = is_upper_alpha((BYTE)Form[0], Language);

	std::vector<CFormInfo> Paradigms;

	pLemmatizer->CreateParadigmCollection(false, Form, bCapital, true, Paradigms);

	for (int i =0; i < Paradigms.size(); i++)
	{
		const CFormInfo& F = Paradigms[i];

		BYTE Accent = F.GetSrcAccentedVowel();
		if (Accent != 0xff)
			Result = Format("%s+%s", Form.substr(0, Accent).c_str(), Form.substr(Accent).c_str());
   	};

	return Result;
};



void ProcessFile(std::string Filename)
{
	printf ("read %s\n", Filename.c_str());

	StringVector Forms;
	{
		FILE* fp = fopen (Filename.c_str(), "r");
		if (!fp)
		{
			printf ("cannot open file %s\n", Filename.c_str());
			return;
		};
		char buffer[256];
		while (fgets(buffer,256, fp))
		{
			std::string q =  buffer;
			Trim(q);
			if (q.empty()) continue;
			RmlMakeUpper(q,pLemmatizer->GetLanguage());
			Forms.push_back(q);
		};
		fclose (fp);
	};
	size_t Count = Forms.size();

	StringVector Results;
	Results.resize(Count, string (255,' ') );
	printf ("process %s\n", Filename.c_str());
	clock_t start = clock();
	const size_t OutBufferSize = 2000; 
	char OutBuffer[OutBufferSize];
	for (size_t i=0; i < Count; i++)
	{
		if (!pLemmatizer->GetAllAncodesAndLemmasQuick(Forms[i], true, OutBuffer, OutBufferSize, true))
		{
			printf ("Too many variants for %s\n", Forms[i].c_str());
		};
		if (Language == morphRussian)
		    RussianConvertToDictionary(Forms[i]);

		// Get results by dictionary.
		Results[i] = GetMorphInfo(Forms[i].c_str());
		
		if (Results[i].length() == 0)
		    Results[i] = Forms[i];
		    
		Results[i] += "\n";
		
		if (Language == morphRussian)
		    RussianConvertToScreen(Results[i]);	 
	
		// Convert to lower case.
		lowercase(Results[i]);		
	};

	std::string OutFileName = MakeFName(Filename,"lem");
	printf ("writing to %s\n",OutFileName.c_str());
	{
		FILE* out_fp = fopen (OutFileName.c_str(), "w");
		if (!out_fp)
		{
			printf ("cannot write file %s\n", OutFileName.c_str());
			return;
		};
		for (size_t i=0; i < Count; i++)
			fprintf (out_fp,"%s", Results[i].c_str());
		fclose (out_fp);
	};
};

template <class  T, class Y>
bool InitMorphologySystem()
{
	std::string langua_str = GetStringByLanguage(Language);
	pLemmatizer = new T;
	string strError;
	if (!pLemmatizer->LoadDictionariesRegistry(strError))
	{
   		fprintf (stderr, "Cannot load %s  morphological dictionary\n", langua_str.c_str());
		fprintf (stderr, "Return error: %s\n", strError.c_str());
		return false;
	};
	pAgramtab = new Y;
	if (!pAgramtab->LoadFromRegistry())
	{
   		fprintf (stderr, "Cannot load %s  gramtab\n", langua_str.c_str() );
		return false;
	};
	return true;
};


void PrintUsage()
{
		printf ("Dialing Command Line Morphology (www.aot.ru && pavel zernov)\n");
		printf ("Usage: Accent (RUSSIAN|ENGLISH|GERMAN) [file]\n");
		printf ("if [file] is specified then Accent builds a accent-file by [file] and exits\n");
		printf ("otherwise it reads from standard input\n");
		printf ("Example: Accent Russian\n");
		printf ("Example: Accent English test.txt\n");
		exit(-1);	
};


int main(int argc, char **argv)
{
	if (argc < 2)
		PrintUsage();

	if (!GetLanguageByString(argv[1], Language))
	{
		PrintUsage();
	};

	if (		!strcmp (argv[1],  "-h")
			||  !strcmp (argv[1],  "-help")
			||  !strcmp (argv[1],  "/h")
			||  !strcmp (argv[1],  "/help")
	)
		PrintUsage();


	std::string FileName;
	if (argc > 2)
		FileName = argv[2];

	// ===============  LOADING DICTS ================
	printf ("Loading..\n");
	bool bResult = false;
	switch (Language)
	{
		case morphRussian: 
					bResult = InitMorphologySystem<CLemmatizerRussian, CRusGramTab>();
					break;
		case morphEnglish : 
					bResult = InitMorphologySystem<CLemmatizerEnglish, CEngGramTab>();
					break;
		case morphGerman: 
					bResult = InitMorphologySystem<CLemmatizerGerman, CGerGramTab>();
					break;
		default : PrintUsage();
		
	};
	if (!bResult)
		return 1;

	if (!FileName.empty())
	{
		ProcessFile(FileName);
		delete pLemmatizer;
		delete pAgramtab;
		return 0;
	};
	

	// ===============  WORKING ===============
	
	printf ("Input a word..\n");
	for (;;)
	{
		printf (">");
		char buffer[1000] = "";
		char Form[100];	
		fgets(Form, 1000, stdin);
		std::string s = Form;
		Trim(s);
		if (s.empty()) break;
		if (s == "noids")
		{
			bPrintIds  = false;
			continue;
		};

		if (Language == morphRussian) 	
			RussianConvertToDictionary(s);
		
		string Result = GetMorphInfo(s);

		if  ( morphRussian == Language)    
			RussianConvertToScreen(Result);

		printf ("%s\n", Result.c_str());
		 
	};
	delete pLemmatizer;
	delete pAgramtab;
	return 0;
}
