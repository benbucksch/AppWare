#ifndef _RADHEADER_
#define _RADHEADER_


#define RADIO_ITEMS_INFO		0
#define NUM_RADIO_DATAS			1



class EDIT_RADIO
	{
	private:
		int						version;
		AOBJECTID			editID;
		int						numItems;
		AMEMBLOCKID		itemsMID;
		LPOBJECTID		items;
		HWND					hWndDlg;

	public:
		EDIT_RADIO(AOBJECTID id);
		~EDIT_RADIO();
		void InitDialog();
		BOOL AddItem(AOBJECTID id);
		BOOL RemoveItem(int idx);
		void Store();
		void FillObjectsList();
		void FillItemsList();
		void ListsChanged();
		friend LRESULT CALLBACK Rad_SetupDlg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	};
typedef EDIT_RADIO  * LPEDIT_RADIO;

extern "C" void ALMAPI Rad_objectChanged(pAEvtInfo theEvent, pAObjMessage theSystem);

long EditRadioObject(AOBJECTID id);
LRESULT CALLBACK Rad_SetupDlg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

extern "C" long ALMAPI RadioObjectAdm(AOBJECTID oid, pAObjMessage theSystem);


#endif
