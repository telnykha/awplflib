#define TIXML_USE_STL
#include <string>
#include "_LF.h"

/*начало, поехали*/
int main()
{
	TLFImage image;
	TLFFGEngine engine;
	if (!engine.Load("/home/nastya/Документы/faces_engine_2016_1.xml"))
	{
		printf("1");
		return -1;
	}
	if (!image.LoadFromFile("/home/nastya/Документы/test456/00002_931230_fa.jpg"))
	{
		printf("2");
		return -1;
	}
	if (!engine.SetSourceImage(&image, true))
	{
		printf("3");
		return -1;
	}
	if (engine.GetItemsCount() > 0)
	{
		printf("4");
		TLFSemanticImageDescriptor* si = engine.GetSemantic();
		if (si != NULL)
			si->SaveXML("faces.xml");
	}
	else printf("!!!");
	return 0;
}
/*вот и все*/
