
#include "_LF.h"
#include "tinyxml.h"
#include <math.h>
#ifdef _BORLAND_
	#include <vcl.h>
	#pragma link "awpipl2b.lib"
	#pragma link "FVLCB.lib"
	#pragma link "awplflibb.lib"
	#pragma link "JPEGLIB.lib"
    #pragma link "TinyXML.lib"
	#pragma argsused
#endif


void Usage()
{
	printf ("csbuilder -key <param.xml> [out.jpg]\n");
	printf ("key:\n");
	printf("-i get information about detector in the <param.xml>\n");
	printf("-a add strong classifier.\n");
	printf("-b build new detector.\n");
}

int main(int argc, char* argv[])
{
	std::string name = "";
	std::string key  = "";
	std::string outName = "";
	if ( argc < 2 )
	{
		Usage();
		return 0;
	}  
	if (argc == 2)
	{
		key = argv[1];
		name = "CSBuild.xml";
	}
	else
	{
		key = argv[1];
		name = argv[2];
	}
    TCSBuildDetector  Builder;
	if (!Builder.LoadConfig(name))
	{
		printf("Failed to load config. \n");
		return 0;
	}

	if (key == "-i")
	{
		//do detector info.
		printf("detector info.\n");
		Builder.PrintDetectorInfo();
		
	}
	else if (key == "-a")
	{
		//do add new strong classifier
		printf("Add new strong classifier.\n");
		Builder.AddNewClassifier();
	}
	else if (key == "-b")
	{
		//do new detecor
		Builder.Build();
	}
	else 
	{
		Usage();
		return 0;
	}

	return 0;
}
//---------------------------------------------------------------------------
