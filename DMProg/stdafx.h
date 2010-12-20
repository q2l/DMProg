
// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#pragma once

#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // Exclude rarely-used stuff from Windows headers
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // some CString constructors will be explicit

// turns off MFC's hiding of some common and often safely ignored warning messages
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions


#include <afxdisp.h>        // MFC Automation classes



#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC support for Internet Explorer 4 Common Controls
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT


#import <msxml.dll>


/////////////////////////////////////////////////////////////////////////////////
// Start XML Archive support

#define DECLARE_XMLSERIAL(className) DECLARE_SERIAL(className)\
	CRuntimeClass* GetActualClass() { return className::GetRuntimeClass(); }; 
#define IMPLEMENT_XMLSERIAL(className, baseName) IMPLEMENT_SERIAL(className, baseName, 1)

//
// Example of serialization of CArray
//
#include <afxtempl.h>

template<class TYPE, class ARG_TYPE = const TYPE&>
class CArrayXML : public CArray<TYPE, ARG_TYPE>
{
public:
	virtual void Serialize(CArchive& ar)
	{
		CXMLArchive& xmlArchive = static_cast<CXMLArchive&>(ar);
		CXMLArchiveNode* curNodePtr = xmlArchive.GetCurrentNode();

		if (xmlArchive.IsStoring())
		{
			// Storing
			for (int arrayIndex = 0; arrayIndex < GetSize(); arrayIndex++)
			{
				GetAt(arrayIndex)->Serialize(xmlArchive);
			}
		}
		else
		{
			// Loading
			int numberObjects = curNodePtr->GetNoChildren();

			for (int i = 0; i < numberObjects; i++, curNodePtr->GetNextChildIndex())
			{
				CString childNodeName = curNodePtr->GetChildName(i);

				CObject* newItemPtr = CXMLArchiveNode::CreateObject(childNodeName);

				if (newItemPtr == NULL)
				{
					continue;
				}

				newItemPtr->Serialize(xmlArchive);

				Add(static_cast<TYPE>(newItemPtr));
			}
		}
	}
};

// End XML Archive support
///////////////////////////////////////////////////////////////////////////////////////







#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif


