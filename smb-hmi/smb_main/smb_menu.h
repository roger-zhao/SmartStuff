/*
 * smb_menu.h
 *
 */

#ifndef SMB_MENU_H 
#define SMB_MENU_H 

#if defined(ARDUINO) && ARDUINO >= 100
  #include <Arduino.h>
#else
  #include <WProgram.h>
#endif


#define MENU_TIMEOUT 30000
#define MAX_BUTTONS_MENU 4

/*#ifdef __cplusplus
extern "C" {
#endif */
// extern doesn't like templates

typedef int SMBTabId;
typedef int SMButtonId;



class Menu {
public:
	enum OutputType {SEROUT, LCDOUT};
	enum InputType  {SERIN, KEYIN};

public:
	const char *name;
	unsigned char  id;
	SMBTabId tabId; // menu's tab id (UTFT_Button id)
	SMButtonId buttons[MAX_BUTTONS_MENU];
	// Variables that hold the data structure of menu connections
	Menu *parent;
	Menu *child;  // the first of the linked list of children
	Menu *next;   // next sibling
	Menu *prev;   // previous sibling

	enum displayFlg {REDRAW, REFRESH} disFlg;

	static OutputType output;
	static InputType input;

public:

	void  (*display)(Menu &m, Menu &l);
	void  (*processKey)(Menu &m, char *key);

	Menu() {this->tabId = -1;};

	Menu(const char *_n, unsigned char  _id, OutputType _outType = LCDOUT, InputType _inType = KEYIN, displayFlg _flg = REDRAW);

	virtual ~Menu();

};



#if 0

template <typename typeT>
class DisplayData {
public:
	typeT *pdata;
	DisplayData();
	DisplayData(typeT *_pdata);
};

template <typename typeT>
DisplayData<typeT>::DisplayData(typeT *_pdata){
	this->pdata = _pdata;
};


class DisplayPos {
public:
	US topLeftPos_x; // less than 65536 pixels in x label
	US topLeftPos_y; // same as above
	US bottomRightPos_x;
	US bottomRightPos_y;
	DisplayPos() {};
	DisplayPos(US _x0, US _y0,  US _x1, US _y1) {
		this->topLeftPos_x = _x0; this->topLeftPos_y = _y0; this->bottomRightPos_x = _x1; this->bottomRightPos_y = _y1;};
	DisplayPos(US _x0, US _y0) {
			this->topLeftPos_x = _x0; this->topLeftPos_y = _y0;};

};

class RGBColor {
public:
	UC redVal;
	UC greenVal;
	UC blueVal;
	RGBColor() {};
	RGBColor(UC _r, UC _g, UC _b)
	{this->redVal = _r; this->greenVal = _g; this->blueVal = _b;};
};

class DisplayColor {
public:
	RGBColor frontColor;
	RGBColor backColor;
	DisplayColor() {};
	DisplayColor(RGBColor _f, RGBColor _b)
	{this->frontColor = _f; this->backColor = _b;};
};

class DisplayControl {
public:
	enum disCtrlType {ORIGINAL, REVERSE, REFRESH, NA} ctrlType;
	// enum dsFlg {NOCHANGE, REFRESHED, NA} ;
	DisplayControl();
	DisplayControl(disCtrlType _type) {this->ctrlType = _type;};
};

template <typename T>
class DataSource {
public:

	T *pVal;
	enum dsFlg {NOCHANGE, REFRESHED, NA} flg;
	enum dsType {TEXT, NUMBER, CURRSTATION, NEXTSTATION, YJCD, ALLSTATION, WEATHER, NA} type;

	DataSource();
	DataSource(dsFlg _flg, T *_pv);
	DataSource(dsFlg _flg, T *_pv, dsType _type);
public:
	void setFlg(UC _flg) {this->flg = _flg;};
	UC getFlg() {return this->flg;};

	void setVal(T *_pv) {this->flg = _pv;};
	void *getVal() {return this->pVal;};

private:

};

template <typename T>
DataSource<T>::DataSource(DataSource::dsFlg _flg, T *_pv) {
	this->flg = _flg; this->pVal = _pv; this->type = TEXT;
}

template <typename T>
DataSource<T>::DataSource(DataSource::dsFlg _flg, T *_pv, dsType _type) {
	this->flg = _flg; this->pVal = _pv; this->type = _type;
}




class MenuItem {
public:
	enum itemType {TEXT, NUMBER, SYMBOL, LINE, CIRCLE, RECT, ROUNDRECT, DATETIME, BITMAP} type;
	UC Id;
// private:

	DataSource *pSrc;

	DisplayData *pData;
	uint8_t *pFont;
	DisplayPos *pPos;
	DisplayColor *pColor;
	DisplayControl *pControl;

	// menu pair: static + dynamic
	MenuItem *pair;

	// list
	MenuItem *prev;
	MenuItem *next;

	// display func ptr
	void  (*display)(MenuItem &mi);
public:
	MenuItem();
	MenuItem(UC _id, itemType _type) {this->type = _type; this->id = _id;};
	MenuItem(UC _id, itemType _type, uint8_t *_pFont) {this->type = _type; this->id = _id; this->pFont = _pFont;};
	void refresh();

};
#endif

#endif /* YAMS_H_ */

