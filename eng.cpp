#define TIXML_USE_STL
#include <string>
#include "_LF.h"

/*начало, поехали*/
int main()
{
	printf("Hello Locate Framework!\n");			
	TLFImage image;
	TLFDetectEngine engine;
	printf("Loadng engine....");
	if (!engine.Load("engine.xml"))
	{
		printf("cannot load engine!\n");
		return -1;
	}
	printf("done.\n");
	printf("Loadng image....");
	if (!image.LoadFromFile("test.jpg"))
	{
		printf("cannot load image\n");
		return -1;
	}
	printf("done.\n");
	printf("Process image....");
	if (!engine.SetSourceImage(&image, true))
	{
		printf("cannot process image\n");
		return -1;
	}
	printf("done.\n");

	if (engine.GetItemsCount() > 0)
	{
		printf("number detectec objects = %i\n", engine.GetItemsCount());
		TLFSemanticImageDescriptor* si = engine.GetSemantic();
		if (si != NULL)
			si->SaveXML("faces.xml");
	}
	else 
printf("!!!");

	printf("Work done.\n");			

	return 0;
}
/*вот и все*/
