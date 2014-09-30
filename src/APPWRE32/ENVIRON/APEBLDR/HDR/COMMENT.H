#ifndef COMMENT_H
#define COMMENT_H

#include "classdef.h"
#include "navitem.h"
#include "fwinobj.h"
#include "dbstring.h"

class COMMENT:public FUNCWINOBJ, public NAVIGATORITEM
	{
	private:
		static HFONT font;
		static int count;
		POINT	 viewOrigin;
		LPPOINT polyPts;


		void DrawSelection(
			HDC hdc);

		void BuildPoly();

		void Initialize();

		void LocationChanged();

		void Unitialize();

	public:

		DBSTRING briefComment;
		DBSTRING fullComment;

		COMMENT(
			SERUID theSubjectUID,
			POINT newLocation,
			LPCSTR commentName,
			LPCSTR commentBody);

		COMMENT(
			void * thisPtr);

		~COMMENT();

		short PostReadInit();

		void Draw(
			HDC hdc);

		void GetRect(
			LPRECT bounds);

		void ShowItem();

		void DrawDrag(
			HDC hdc,
			POINT offset);

		void Move(
			POINT offset);

		void ChangeCommentName(
			LPCSTR shortName,
			LPCSTR fullName);

		void Identify(
			HDC hdc);

		void SetViewOrigin();

		void SetLocation(POINT p)
			{
			location = p;
			LocationChanged();
			}

		WORD ExportUPSF();

		BOOL ImportUPSF();

		BOOL AddObjectToZlist(
			LPRECT area,
			LPZLIST zlistPtr);

	};

#endif
