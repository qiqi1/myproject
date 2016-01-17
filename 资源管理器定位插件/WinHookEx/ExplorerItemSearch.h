#pragma once
#include <list>
#include <OleAcc.h>
#include <atlstr.h>

#pragma comment(lib,"Oleacc.lib")
using namespace std;

typedef struct tagSEARCH_RESULT_NODE
{
	CString      strItemName;
	IAccessible* paccParent;
	IAccessible* paccChild;
	int          nChildIndex;
}SEARCH_RESULT_NODE,*PSEARCH_RESULT_NODE;
typedef list<SEARCH_RESULT_NODE> LIST_SEARCH_RESULT;
typedef LIST_SEARCH_RESULT::iterator LIST_SEARCH_RESULT_PTR;

BOOL SearchExplorerItem(HWND hWnd,LIST_SEARCH_RESULT *pListRes);
BOOL SetSelectExplorerItem( SEARCH_RESULT_NODE *pNode );
VOID ReleaseSearchResult(LIST_SEARCH_RESULT *pListRes);