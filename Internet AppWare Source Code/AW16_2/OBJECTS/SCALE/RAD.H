#ifndef _RADHEADER_
#define _RADHEADER_


#define RADIO_ITEMS_INFO		0
#define NUM_RADIO_DATAS			1



class EDIT_RADIO
	{
	private:
		int						version;
		OBJECTID			editID;
		int						numItems;
		MEMBLOCKID		itemsMID;
		LPOBJECTID		items;
		HWND					hWndDlg;

	public:
		EDIT_RADIO(OBJECTID id);
		~EDIT_RADIO();
		void InitDialog();
		BOOL AddItem(OBJECTID id);
		BOOL RemoveItem(int idx);
		void Store();
		void FillObjectsList();
		void FillItemsList();
		void ListsChanged();
		friend LRESULT far pascal _export Rad_SetupDlg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	};
typedef EDIT_RADIO far * LPEDIT_RADIO;


extern "C" void pascal _export Rad_objectChanged(pAEvtInfo theEvent, pAObjMessage theSystem);
long EditRadioObject(OBJECTID id);
LRESULT far pascal _export Rad_SetupDlg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

extern "C" long pascal _export RadioObjectAdm(OBJECTID oid, pAObjMessage theSystem);


#endif
