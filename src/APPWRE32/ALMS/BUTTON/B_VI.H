#ifndef B_VI_H
#define B_VI_H

extern "C" LRESULT CALLBACK AppwareButtonObjectProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

class HWND_VI:public OpHwndWndItem
	{
	public:
		BOOL									m_bDoubleClicked;

	public:
		HWND_VI(AOBJECTID oiItem, AOBJECTID oiParent, RECT* rcItem, long status)
			:OpHwndWndItem(oiItem, oiParent, rcItem, status)
			{
			}
		
	};

class PUSH_VI:public HWND_VI
	{
	private:
		ATTRIBUTES    				m_attributes;   			//Font, foreground, background
		
	public:
		PUSH_VI(AOBJECTID oiItem, AOBJECTID oiParent, RECT* rcItem, long status);

		void ALMAPI Destroy();
		void ALMAPI DataChanged(long details, long modifiers, long variation);
  	void ALMAPI ParentOpened(void);
	  void ALMAPI ParentClosing(void);
  	void ALMAPI AccelKeyHit(void);
  	void ALMAPI SetDefaultItem(BOOL set);
	  long ALMAPI Activate(int causedBy, BOOL mustKeep);
	  void ALMAPI Deactivated();
  	LRESULT ALMAPI ParentNotification(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

		static BOOL ALMAPI AppearanceChanged(PUSH_VI *vi, long fwWhatChanged, long);
		BOOL SetAccelKey();
 	};

class RADIO_VI:public HWND_VI
	{
	private:
		ATTRIBUTES    				m_attributes;   			//Font, foreground, background
		
	public:
		RADIO_VI(AOBJECTID oiItem, AOBJECTID oiParent, RECT* rcItem, long status);

		void ALMAPI Destroy();
		void ALMAPI DataChanged(long details, long modifiers, long variation);
  	void ALMAPI ParentOpened(void);
	  void ALMAPI ParentClosing(void);
  	void ALMAPI AccelKeyHit(void);
	  long ALMAPI Activate(int causedBy, BOOL mustKeep);
 		LRESULT ALMAPI ParentNotification(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
		void ALMAPI Print(HDC hdc, LPRECT rcPrint, int pclPrinter);

		static BOOL ALMAPI AppearanceChanged(RADIO_VI *vi, long fwWhatChanged, long);
		BOOL SetAccelKey();
	};

class CHECK_VI:public HWND_VI
	{
	private:
		ATTRIBUTES    				m_attributes;   			//Font, foreground, background
		
	public:
		CHECK_VI(AOBJECTID oiItem, AOBJECTID oiParent, RECT* rcItem, long status);

		void ALMAPI Destroy();
		void ALMAPI DataChanged(long details, long modifiers, long variation);
  	void ALMAPI ParentOpened(void);
	  void ALMAPI ParentClosing(void);
  	void ALMAPI AccelKeyHit(void);
	 	LRESULT ALMAPI ParentNotification(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
		void ALMAPI Print(HDC hdc, LPRECT rcPrint, int pclPrinter);

		static BOOL ALMAPI AppearanceChanged(CHECK_VI *vi, long fwWhatChanged, long);
		BOOL SetAccelKey();
	};


//////////////////////////////

class BITMAP_VI:public OpLiteWndItem
	{
	private:
		HBITMAP									m_hbmpNormal, m_hbmpFocused, m_hbmpPressed, m_hbmpGrayed;
		BITMAP_STATE 						m_state;
		BOOL										m_bTracking;
		BOOL										m_bDoubleClicked;

	public:
		BITMAP_VI(AOBJECTID oiItem, AOBJECTID oiParent, RECT* rcItem, long status);
	
		void ALMAPI Destroy();
		void ALMAPI DataChanged(long details, long modifiers, long variation);
	 	void ALMAPI ParentOpened(void);
	  void ALMAPI ParentClosing(void);
	 	void ALMAPI AccelKeyHit(void);
	  long ALMAPI Activate(int causedBy, BOOL mustKeep);
	  void ALMAPI Deactivated();
	  void ALMAPI Enable(BOOL bEnable);

		BOOL ALMAPI QueryDesiredPalette(AMEMBLOCKID miPalette);

		void ALMAPI Move(LPRECT rcNew);
		long ALMAPI AdjustNewRect(LPRECT rcNew, long adjustFlag);
	  void ALMAPI Draw(HDC hdc, LPRECT rcInvalid);
	  void ALMAPI KeyDown(int vKey, char ascii, long keyData);
	  void ALMAPI KeyUp(int vKey, char ascii, long keyData);
		void ALMAPI MouseMove(POINT* where, int keysDown);
	  void ALMAPI MouseDown(POINT* where, int keysDown, int button);
	  void ALMAPI DoubleClicked(POINT* where, int keysDown, int button);
	  void ALMAPI MouseUp(POINT* where, int keysDown, int button);
	  void ALMAPI CancelMode(void);

		void GetBitmapSize(LPRECT rcRequested, LPPOINT ptBmpSize, LPPOINT ptSize);
		BOOL ChangeState(BITMAP_STATE newState);

		void CreateBitmapHandles();
		void DestroyBitmapHandles();

		static BOOL ALMAPI AppearanceChanged(BITMAP_VI *vi, long fwWhatChanged, long);
		BOOL SetAccelKey();
	};

class INVISIBLE_VI:public OpLiteWndItem
	{
	protected:
		INVISIBLE_STATE 				m_state;
		BOOL										m_bTracking;
		BOOL										m_bDoubleClicked;

	public:
		INVISIBLE_VI(AOBJECTID oiItem, AOBJECTID oiParent, RECT* rcItem, long status);

		void ALMAPI Destroy();
		void ALMAPI AccelKeyHit(void);
	  virtual void ALMAPI Draw(HDC hdc, LPRECT rcInvalid);
		void ALMAPI MouseMove(POINT* where, int keysDown);
		void ALMAPI MouseDown(POINT* where, int keysDown, int button);
	  void ALMAPI DoubleClicked(POINT* where, int keysDown, int button);
		void ALMAPI MouseUp(POINT* where, int keysDown, int button);
		void ALMAPI CancelMode(void);

		static BOOL ALMAPI AppearanceChanged(INVISIBLE_VI *vi, long fwWhatChanged, long);

		virtual BOOL ChangeState(INVISIBLE_STATE newState);
		BOOL SetAccelKey();
	};

class INVISIBLE_INVERTS_VI:public INVISIBLE_VI
	{
	public:
		INVISIBLE_INVERTS_VI(AOBJECTID oiItem, AOBJECTID oiParent, RECT* rcItem, long status);

	  void ALMAPI Draw(HDC hdc, LPRECT rcInvalid);

		BOOL ChangeState(INVISIBLE_STATE newState);
	};




#endif //B_VI_H
