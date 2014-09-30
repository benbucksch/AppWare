#ifndef VI_H
#define VI_H

enum TRACKING_MODE {I_TRACKING_NOT=0, I_TRACKING_OUTSIDE, I_TRACKING_INSIDE};

class POLY_VI:public OpLiteWndItem
	{
	private:
			USHORT							m_usTrackingButtonIndex;
			TRACKING_MODE				m_tracking;

	public:
		POLY_VI(AOBJECTID oiItem, AOBJECTID oiParent, RECT* rcItem, long status);
	
		void ALMAPI Destroy();
		void ALMAPI DataChanged(long details, long modifiers, long variation);
	 	void ALMAPI ParentOpened(void);
	  void ALMAPI ParentClosing(void);

		long ALMAPI AdjustNewRect(LPRECT rcNew, long adjustFlag);
	  void ALMAPI Draw(HDC hdc, LPRECT rcInvalid);
	  void ALMAPI MouseDown(POINT* where, int keysDown, int button);
		void ALMAPI DoubleClicked(POINT* where, int keysDown, int button) { MouseDown(where, keysDown, button); }
		void ALMAPI MouseMove(POINT* where, int keysDown);
	  void ALMAPI MouseUp(POINT* where, int keysDown, int button);
	  void ALMAPI CancelMode(void);

		BOOL SetTrackingMode(TRACKING_MODE track);
		TRACKING_MODE GetTrackingMode(void) {return m_tracking;}

		void SetTransparent(void);

	};
typedef POLY_VI  * LPPOLY_VI;

extern "C" OpWndItemD* ALMAPI	CreateVisualInstance(AOBJECTID oiItem, AOBJECTID oiParent, RECT* rcItem, long status, long special);



#endif
