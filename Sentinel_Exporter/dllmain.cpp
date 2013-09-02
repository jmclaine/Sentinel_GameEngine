#include "stdafx.h"
#include "Sentinel_Exporter.h"

//----------------------------------------------------------------
// Macros
//----------------------------------------------------------------

#define MAXEXPORT_API extern "C" __declspec(dllexport) 

//----------------------------------------------------------------
// Global variables
//----------------------------------------------------------------

HINSTANCE	g_instance;

//----------------------------------------------------------------
// Local declarations
//----------------------------------------------------------------

class MyExporterClassDesc : public ClassDesc2
{
public:
	int 			IsPublic()						{return TRUE; }
	void*			Create( BOOL loading=FALSE )	{return new MaxExporter();}
	const TCHAR*	ClassName()						{return _T("Custom Max Exporter");}
	SClass_ID		SuperClassID()					{return SCENE_EXPORT_CLASS_ID;}
	Class_ID		ClassID()						{return Class_ID(0x33a262ac, 0x6fa25f72);}
	const TCHAR* 	Category()						{return _T("Exporters");}
	const TCHAR*	InternalName()					{return _T("Custom Max Exporter");}
	HINSTANCE		HInstance()						{return g_instance;}
};

//----------------------------------------------------------------
// DLL functions
//----------------------------------------------------------------

BOOL WINAPI DllMain( HINSTANCE inst, ULONG reason, LPVOID )
{
	if ( DLL_PROCESS_ATTACH == reason )
	{
		g_instance = inst;
		DisableThreadLibraryCalls( g_instance );
		return TRUE;
	}

	return TRUE;
}

MAXEXPORT_API const TCHAR* LibDescription()
{
	return _T("Custom 3dsmax Exporter");
}

MAXEXPORT_API int LibNumberClasses()
{
	return 1;
}

MAXEXPORT_API ClassDesc* LibClassDesc( int i )
{
	switch( i ) 
	{
		case 0:{
			static MyExporterClassDesc s_classDesc;
			return &s_classDesc;}

		default:
			return 0;
	}
}

MAXEXPORT_API ULONG LibVersion()
{
	return VERSION_3DSMAX;
}

MAXEXPORT_API ULONG CanAutoDefer()
{
	return 1;
}
