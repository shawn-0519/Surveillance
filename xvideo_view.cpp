#include "xvideo_view.h"
#include "xsdl.h"
XVideoView* XVideoView::Create(Rendertype type)
{
	switch (type)
	{
	case XVideoView::SDL:
		return new XSDL();
		break;
	default:
		break;
	}

	return nullptr;
}
