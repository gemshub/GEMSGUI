//-------------------------------------------------------------------
// $Id: page_w.cpp 969 2007-12-13 15:41:25Z gems $
//
// Implementation of TCPage, TField, TCell, TCellInput
//  TCellCheck, TCellText and TQueryWindow classes
//
// Copyright (C) 1996-2001  A.Rysin
// Uses  gstring class (C) A.Rysin 1999
//
// This file is part of the GEM-Vizor library which uses the
// Qt v.2.x GUI Toolkit (Troll Tech AS, http://www.trolltech.com)
// according to the Qt Duo Commercial license
//
// This file may be distributed under the terms of the GEMS-PSI
// QA Licence (GEMSPSI.QAL)
//
// See http://gems.web.psi.ch/ for more information
// E-mail gems2.support@psi.ch
//-------------------------------------------------------------------
#include <stdio.h>

#include <qapplication.h>
#include <qpushbutton.h>
#include <qtooltip.h>
#include <qnamespace.h>
#include <q3accel.h>
#include <qvalidator.h>
#include <qclipboard.h>
#include <qscrollbar.h>
//Added by qt3to4:
#include <QKeyEvent>
#include <QLabel>
#include <Q3PopupMenu>
#include <QFocusEvent>
#include <QCloseEvent>

#include "units.h"
#include "page_i.h"
#include "visor.h"
#include "v_module.h"
#include "m_param.h"
#include "m_sdata.h"
#include "module_w.h"
#include "page_w.h"
#include "page_win_w.h"
#include "visor_w.h"
#include "service.h"
#include "query_w.h"
#include "help.h"
#include "CalcDialog.h"
#include "CalcCheckDialog.h"


const char *GEMS_OBJNDX_HTML = "g_objndx.html";

// temporary workaround to make empty value look better
const gstring emptiness("---");
const gstring short_emptiness("---");

inline
gstring visualizeEmpty(const gstring& text)
{
    return (text==S_EMPTY) ? emptiness : text;
}

// for QLineEdit with QValidator
static
const char* GetAllowedRegExp(TObject& obj)
{
    ObjType type = obj.GetType();
    switch ( type )
    {
    case I_: 		//<short>
    case L_: 		//<long>
    case H_: 		//<signed char>
        return	"(---)|([*`])|(-?\\d{0,9})";
    case U_: 		//<unsigned short>
    case X_: 		//<unsigned long>
    case B_: 		//<unsigned char>
        return	"(---)|([*`])|(\\d{0,9})";
    case F_: 		//<float>
    case D_: 		//<double>
        return	"(---)|([*`])|(-?\\d{0,9}(\\.\\d{1,9})?(e-?\\d{1,2})?)";
    case A_: 		//<char> - not used here
    case S_: 		//TValString
    default:		// 1..127
        return	".*";
    }
}

//inline
static
uint AllowedPos(TObject& obj)
{
    switch ( obj.GetType() )
    {
    case I_: 		//<short>
        return	5+1;
    case U_: 		//<unsigned short>
        return	5;
    case L_: 		//<long>
        return	10+1;
    case X_: 		//<unsigned long>
        return	10+1;
    case F_: 		//<float>
        return	13+1+2;  //SD 0ct 2005
    case D_: 		//<double>
        return	32+1;      // 32 chars for 'double'???
    case A_: 		//<char>
        return	1+1;
    case B_: 		//<unsigned char>
    case H_: 		//<signed char>
        return	3+1;
    case S_: 		//TValString
        return	32000+1; //obj.GetM();
    default:
        return	(unsigned char)obj.GetType();
    }

}


static const int wdWIN = 500;
static const int htWIN = 400;

static const int wdTab = 200;
static const int szScroll = 15;

static const int X0 = 10;	// position of the 1st Field
static const int Y0 = 10;	// on the Page



//------------------------------------------------
// TCell
//------------------------------------------------


TCell::TCell(TObject& ro, int n, int m, bool edit_, eShowType showType_, QWidget* p):
    rObj(ro), 
    N(n), M(m), 
    edit(edit_), 
    showType(showType_), 
    pw(p)
{
    backgroundColor = pw->paletteBackgroundColor();
}

void
TCell::SetDescription()
{
    QWidget* topw = pw->topLevelWidget();
    const gstring& desc = rObj.GetDescription(N,M);
    vstr s(desc.length()+45);
    if( rObj.GetN()!=1 || rObj.GetM()!=1 )
        sprintf(s, "%s [%u,%u] : %s", rObj.GetKeywd(), N, M, desc.c_str());
    else
        sprintf(s, "%s : %s", rObj.GetKeywd(), desc.c_str());

    if( topw->inherits("QDialog") )
        ((TQueryWindow*)(topw))->setHint( s );
    else
        ((TCModuleImp*)(topw))->SetStatus( s );
}

// Help on F1 pressed on data field
void
TCell::CmHelp()
{
    // Sveta 16/02/2005
    gstring item = rObj.GetFullName(N,M);
//    vstr v(15);
//    int Ndx = rObj.ndx( N, M );
//    gstring item = rObj.GetKeywd();

//    item += "_";
//    sprintf(v, "%u", Ndx );  // N);  fixed 16.01.01
//    item += v;  /// # of the cell
    try
    {
        gstring item1 = item;
        gstring docDir = pVisor->docDir();/*pVisor->sysGEMDir();
        docDir += "doc/html/"; oct 2005 SD */
        Helper hlp(docDir);
        // Get file URL to DOD item description
        hlp.checkForRef( GEMS_OBJNDX_HTML, item );

        if( item.empty() )
            throw TError("Help", item1 + ": No such item in HTML files!");

    	// Changed to return file locator in item   DAK 05.01.01
        //16/06/04  Sveta          pVisorImp->OpenHelp( item.c_str() );
        if( pw->topLevelWidget()->inherits("QDialog") )
            pVisorImp->OpenHelp( item.c_str(), NULL, pw->topLevelWidget());  // Fix 15.09.04
//            pVisorImp->OpenHelp( item.c_str(), rObj.GetKeywd(), pw->topLevelWidget());
        else
            pVisorImp->OpenHelp( item.c_str() );
        // end changes Sveta
        //          pVisorImp->OpenHelp("objind.html", item.c_str() );
    }
    catch(TError err) {
        vfMessage(pw->topLevelWidget(), "Help", err.mess, vfErr);
    }
}



void TCell::updateCellBackground()
{

    QColor col;
    switch( showType )
    {
    case stResult:
        col = QColor(Qt::cyan);
        break;
    case stIO:
        col = QColor(130, 160, 130);
        break;
    case stWork:
        col = QColor(130, 130, 160);
        break;
    case stAux:
        col = QColor(150, 150, 90);
        break;
    case stHelp:
        col = QColor(Qt::gray);
        break;
    }

    pw->setPalette(QPalette(col, backgroundColor));
}

void TCell::setGroupSelected(bool selected)
{
    pw->setPaletteBackgroundColor( selected ? Qt::yellow : backgroundColor );
}



//------------------------------------------------
// TCellInput
//------------------------------------------------

TCellInput::TCellInput(TField& rfield, int xx, int yy,
                       eFieldType ft, int npos, eShowType showType_,
                       TObject& rO, int n, int m, bool ed, int ht):
        QLineEdit(&rfield),
        TCell(rO, n, m, ed, showType_, this),
        fieldType(ft)
{
    move(xx, yy);
    setFixedSize(wdF(fieldType, npos), htF(fieldType, ht));
    setReadOnly(!edit);

    if( edit )
    {
        connect(this, SIGNAL(fieldUpdate()), &rfield, SLOT(Update()));
	setMaxLength(AllowedPos(rO)+1);
	setValidator(new QRegExpValidator( QRegExp(GetAllowedRegExp(rO)), this ) );
    }

    connect(this, SIGNAL(selectionChanged()), field(), SLOT(selectionChanged()));
/*
    QAccel* accel = new QAccel(this);
    accel->connectItem( accel->insertItem(Key_L + CTRL), this, SLOT(CmSelectColumn()) );
    accel->connectItem( accel->insertItem(Key_R + CTRL), this, SLOT(CmSelectRow()) );
//    accel->connectItem( accel->insertItem(Key_ + CTRL), this, SLOT(CmSelectObject()) );
//    accel->connectItem( accel->insertItem(Key_A + CTRL), this, SLOT(CmSelectObject()) );
*/
    switch( fieldType )
    {
    case ftNumeric:
    case ftFloat:
        setAlignment(Qt::AlignRight);
        break;
    default:
        ;
    }

    //    setFont( pVisorImp->getCellFont() );
    updateCellBackground();
    updateDisplay();
    QToolTip::add(this, (rObj.GetDescription(N,M)).c_str() );
}


void
TCellInput::closeEvent(QCloseEvent* e)
{
    if( isModified() )
        setValue();

    QLineEdit::closeEvent(e);
}


void
TCellInput::updateDisplay()
{
//Sveta 16/02/2005
  gstring str_sh = visualizeEmpty(rObj.GetString(N, M));
    if( rObj.GetType() > 0 )
           str_sh.strip();
  setText( str_sh.c_str() );
//    setText( visualizeEmpty(rObj.GetString(N,M)).c_str() );

// temporary fix for Qt bug on right-alighned QLineEdit with contents larger then display size
//    if( (fieldType == ftNumeric || fieldType == ftFloat) )
//		&& text().length()*pVisorImp->getCharWidth() > width()-5 )
//	repaint();

    setCursorPosition(0);
}


void
TCellInput::setIfChanged()
{
    if( isModified() )
        setValue();
}


void
TCellInput::setValue()
{
    gstring str_sh;
    clearModified();
    if( rObj.SetString(text(), N, M) )
    {
        field()->objectChanged();
//Sveta 16/02/2005
        str_sh = visualizeEmpty(rObj.GetString(N, M));
        if( rObj.GetType() > 0 )
             str_sh.strip();
        setText( str_sh.c_str() );
//      setText( visualizeEmpty(rObj.GetString(N,M)).c_str() );
    }
    else
    {
        if( text() == emptiness || text() == short_emptiness )
        {
            rObj.SetString(S_EMPTY, N, M);
//Sveta 16/02/2005
        str_sh = visualizeEmpty(rObj.GetString(N, M));
        if( rObj.GetType() > 0 )
             str_sh.strip();
        setText( str_sh.c_str() );
//            setText( visualizeEmpty(rObj.GetString(N,M)).c_str() );
        }
        else
        {
            vfMessage(topLevelWidget(), rObj.GetKeywd(), "Sorry! Wrong value typed!", vfErr );
//Sveta 16/02/2005
        str_sh = visualizeEmpty(rObj.GetString(N, M));
        if( rObj.GetType() > 0 )
             str_sh.strip();
        setText( str_sh.c_str() );
//            setText( visualizeEmpty(rObj.GetString(N,M)).c_str() );
	    // temporary fix for Qt bug on right-alighned QLineEdit with contents larger then display size
	    if( (fieldType == ftNumeric || fieldType == ftFloat) )
		repaint();
            setCursorPosition(0);
        }
    }
}


void
TCellInput::focusInEvent(QFocusEvent* e)
{
    QLineEdit::focusInEvent(e);

    clearModified();
    field()->setFocused(this);
}


void
TCellInput::focusOutEvent(QFocusEvent* e)
{
    if( isModified() )
    	setValue();
	
    if( QFocusEvent::reason() != QFocusEvent::Popup )
	field()->setFocused(NULL);

    QLineEdit::focusOutEvent(e);
}

void
TCellInput::setGroupSelected(bool selected)
{
    deselect();
    TCell::setGroupSelected(selected);
}

void
TCellInput::keyPressEvent(QKeyEvent* e)
{
    if ( e->state() & ControlButton ) {
	switch ( e->key() ) {
	case Qt::Key_C:
	    copy();
	    return;
	break;
	case Qt::Key_A:
	    CmSelectAll();
	    return;
	break;
	case Qt::Key_R:
	    CmSelectRow();
	    return;
	break;
	case Qt::Key_L:
	    CmSelectColumn();
	    return;
	break;
	case Qt::Key_T:
	    pasteTransposed();
	    return;
	break;
	}
    }
    else
    if ( e->state() & ShiftButton ) {
	int n1, n2, m1, m2;
	int vizN1, vizN2, vizM1, vizM2;
	
	if( field()->isSelected() )
	    field()->getSelectedArea(n1, n2, m1, m2);
	else {
	    n1 = GetN();
	    n2 = GetN() + 1;
	    m1 = GetM();
	    m2 = GetM() + 1;
	}
	
	field()->getVisibleArea(vizN1, vizN2, vizM1, vizM2);
	
	switch ( e->key() ) {
	case Qt::Key_Up:
	    if( n1 > 0 ) {
		field()->setSelectedArea(n1-1, n2, m1, m2);
		if( n1-1 < vizN1 )
		    field()->scrollVertically(-1);
	    }
	    return;
	break;
	case Qt::Key_Down:
	    if( n2 < rObj.GetN() ) {
		field()->setSelectedArea(n1, n2+1, m1, m2);
		if( n2 > vizN2 )
		    field()->scrollVertically(1);
	    }
	    return;
	break;
	case Qt::Key_Right:
	    if( m2 < rObj.GetM() ) {
		field()->setSelectedArea(n1, n2, m1, m2+1);
		if( m2 > vizM2 )
		    field()->scrollHorizontally(1);
	    }
	    return;
	break;
	case Qt::Key_Left:
	    if( m1 > 0 ) {
		field()->setSelectedArea(n1, n2, m1-1, m2);
		if( m1-1 < vizM1 )
		    field()->scrollHorizontally(-1);
	    }
	    return;
	break;
	}
    }

    switch( e->key() )
    {
    case Qt::Key_Enter:
    case Qt::Key_Return:
        if( isModified() )
            setValue();
        return;
    case Qt::Key_F1:
        CmHelp();
        return;
    case Qt::Key_F2:
        CmSDRef();
        return;
        //    case Key_F3:
        //        CmScript();
        //        return;
    case Qt::Key_F7:
        CmDComp();
        return;
    case Qt::Key_F8:
        CmCalc();
        return;
    }

    QLineEdit::keyPressEvent(e);
}

/*! mouse press event
    handles context menu
*/
Q3PopupMenu*
TCellInput::createPopupMenu()
{
    Q3PopupMenu* menu = new Q3PopupMenu();
    menu->insertItem( "&Help\tF1", this, SLOT(CmHelp()), Qt::Key_F1 );
    if( fieldType == ftRef )
        menu->insertItem( "&SDRef-Record\tF2", this, SLOT(CmSDRef()), Qt::Key_F2 );
    //    if( fieldType == ftText )
    //        menu->insertItem( "&SDRef-Script\tF3", this, SLOT(CmScript()), Key_F3 );
    if( fieldType == ftRecord )
        menu->insertItem( "&Show record\tF7", this, SLOT(CmDComp()), Qt::Key_F7 );
    if( fieldType == ftFloat || fieldType == ftNumeric )
    {
        menu->insertSeparator();
        menu->setItemEnabled(
            menu->insertItem( "&Calculator\tF8", this,
                              SLOT(CmCalc()), Qt::Key_F8 ), edit );
    }

    menu->insertSeparator();

    const bool clipboardEmpty = QApplication::clipboard()->text(QClipboard::Clipboard).isEmpty();
    if( field()->isSelected() ) {
//        menu->insertItem( "&Unselect object\tF12", this, SLOT(CmSelectObject()), Key_F12 );

        Q3PopupMenu* objectMenu = new Q3PopupMenu();
        objectMenu->insertItem( "Cu&t\tCtrl+X", field(), SLOT(cut()) );
        objectMenu->insertItem( "&Copy\tCtrl+C", field(), SLOT(copy()) );
        objectMenu->setItemEnabled(
            objectMenu->insertItem( "&Paste\tCtrl+V", field(),
                              SLOT(paste()) ), edit && !clipboardEmpty );
        objectMenu->setItemEnabled(
            objectMenu->insertItem( "Paste &Transposed\tCtrl+T", field(),
                              SLOT(pasteTransposed())), edit && !clipboardEmpty );
        objectMenu->insertItem( "Clear", field(), SLOT(clear()) );

    //    menu->insertSeparator();
        menu->insertItem( "&Edit", objectMenu );
    }
    else {
	Q3PopupMenu* editMenu = QLineEdit::createPopupMenu();
	if( rObj.GetN() > 1 || rObj.GetM() > 1 ) {
    	    menu->insertItem( "&Select object\tCtrl+A", this, SLOT(CmSelectObject()), Qt::Key_Alt + Qt::Key_S );
    	    menu->setItemEnabled(
		menu->insertItem( "Select co&lumn\tCtrl+L", this, SLOT(CmSelectColumn()), Qt::Key_Alt + Qt::Key_L ),
		    rObj.GetN() > 1 );
    	    menu->setItemEnabled(
    		menu->insertItem( "Select &row\tCtrl+R", this, SLOT(CmSelectRow()), Qt::Key_Alt + Qt::Key_R ),
		    rObj.GetM() > 1 );
    	    menu->setItemEnabled(
    		menu->insertItem( "Select &cells\tShift+Arrows", this, SLOT(CmSelectObject()) ), false );
	    menu->insertSeparator();
	    editMenu->setItemEnabled(
        	editMenu->insertItem( "Paste &Transposed\tCtrl+T", this,
                              SLOT(pasteTransposed()),
			        QKeySequence(Qt::CTRL + Qt::Key_T), -1, 5 ), edit && !clipboardEmpty );

	}
    //    menu->insertSeparator();
        menu->insertItem( "&Edit",  editMenu );
    }

    setIfChanged();

    return menu;
}


void
TCellInput::CmSDRef()
{
    if( fieldType != ftRef )
        return;

    gstring str = (const char*)text();

    try
    {
        bool patt = false;
        //save new SDrefs
        gstring sd_key;
        if( !strcmp(rObj.GetKeywd(), "SDrefs") )
            sd_key = gstring( TSData::pm->GetKey() );

        if( str.find_first_of("*?") != gstring::npos )  // pattern
        { // Sveta
            str = TSData::pm->GetKeyofRecord( str.c_str(), "Get key", KEY_NEW);
            if( str.empty() )
                return;
            patt = true;
        }
        if( patt )
        {
            setText(str.c_str());
            setValue();
            //save new SDrefs
            if( !strcmp(rObj.GetKeywd(), "SDrefs") )
            {
                // setValue();
                if( sd_key.find_first_of("*?") == gstring::npos )  // pattern
                    TSData::pm->RecSave( sd_key.c_str(), FALSE );
            }
        }

        if( rt[RT_SDATA].Find( str.c_str() ) >= 0)
            TSData::pm->RecInput( str.c_str() );

        pVisorImp->OpenModule(topLevelWidget(), RT_SDATA);
        TSData::pm->Update();
    }
    catch( TError& err )
    {
        vfMessage(topLevelWidget(), err.title, err.mess );
    }
}

void
TCellInput::CmCalc()
{
    if( !edit || ( fieldType != ftFloat && fieldType != ftNumeric) )
        return;

    // check selection
    Selection sel;
    sel.N1 = N;
    sel.N2 = N;
    sel.M1 = M;
    sel.M2 = M;
    
    if( field()->isSelected() ) {
	field()->getSelectedArea(sel.N1, sel.N2, sel.M1, sel.M2);
	sel.N2--;
	sel.M2--;
    }
    
//    cerr << "n1: " << n1 << " n2: " << n2 << " m1: " << m1 << " m2: " << m2 << endl;
    
//    QRect sel(n, m, ncells, mcells);
    CalcDialog calc(topLevelWidget(), rObj, sel);

    if( calc.exec() )
    {
        for(int nn=sel.N1; nn<=sel.N2; nn++)
            for(int mm=sel.M1; mm<=sel.M2; mm++ )
                rObj.Put(calc.fun(rObj.Get(nn, mm)), nn, mm);

//	field()->setSelectedArea(sel.N1, sel.N2+1, sel.M1, sel.M2+1);
        field()->objectChanged();
        emit fieldUpdate();
    }
}

void
TCellInput::CmDComp()
{
    if( fieldType == ftRecord )
        TProfil::pm->ShowDBWindow(rObj.GetKeywd(), N);
}

void
TCellInput::CmSelectObject()
{
    QLineEdit::deselect();
    field()->setSelected(!field()->isSelected());
}

void
TCellInput::CmSelectColumn()
{
    QLineEdit::deselect();
    field()->setSelectedArea(0, rObj.GetN(), M, M+1);
}

void
TCellInput::CmSelectRow()
{
    QLineEdit::deselect();
    field()->setSelectedArea(N, N+1, 0, rObj.GetM());
}

void
TCellInput::CmSelectAll()
{
    if( rObj.GetN() > 1 || rObj.GetM() > 1 ) {
	deselect();
	field()->setSelected(true);
    }
    else
	QLineEdit::selectAll();
}

void
TCellInput::copy()
{
    if( field()->isSelected() )
	field()->copy();
    else
	QLineEdit::copy();
}

void
TCellInput::cut()
{
    if( field()->isSelected() ) {
	field()->copy();
	field()->clear();
    }
    else {
	QLineEdit::copy();
	clear();
    }
}

void
TCellInput::clear()
{
    setText("0");
    setValue();
}

void
TCellInput::paste()
{
//cerr << "paste: field sel: " << field()->isSelected() << endl;
//    TField* selectedField = field()->getPage()->getSelectedObject();
    if( field()->isSelected() )
	field()->paste();
    else
	if( QApplication::clipboard()->text().find('\t') != -1 || 
		QApplication::clipboard()->text().find('\n') != -1 ) {
	// clipboard contents seems to be table - try to paste from current cell
	    field()->pasteIntoArea(N, rObj.GetN(), M, rObj.GetM(), false);
	} 
	else
	    QLineEdit::paste();
}

void
TCellInput::pasteTransposed()
{
//cerr << "paste: field sel: " << field()->isSelected() << endl;
//    TField* selectedField = field()->getPage()->getSelectedObject();
    if( field()->isSelected() )
	field()->pasteTransposed();
    else
	if( QApplication::clipboard()->text().find('\t') != -1 || 
		QApplication::clipboard()->text().find('\n') != -1 ) {
	// clipboard contents seems to be table - try to paste from current cell
	    field()->pasteIntoArea(N, rObj.GetN(), M, rObj.GetM(), true);
	} 
}

//==========================================
// TCellCheck
//==========================================

TCellCheck::TCellCheck(TField& rfield, int x1, int y1, int npos, eShowType showType_,
                       TObject& rO, int n, int m, bool ed1):
        QLineEdit(&rfield),
        TCell(rO, n, m, ed1, showType_, this),
        Vals(aUnits[npos].getVals(M))
{
    move(x1, y1);
    int size = wdF(ftCheckBox, 0);
    setFixedSize(size, size);
    setMaxLength(2);

    if( edit )
        connect(this, SIGNAL(fieldUpdate()), &rfield, SLOT(Update()));

    //    setFont( pVisorImp->getCellFont() );
    updateCellBackground();

    SetString( rObj.GetString(N,M) );

    QToolTip::add(this, (rObj.GetDescription(N,M)).c_str() );
}

void
TCellCheck::SetString(const gstring& s)
{
    size_t ind = Vals.find(s);
    if( ind == gstring::npos )
    {
        //        vstr str(80);
        //        sprintf( str, "%s[%d:%d] ", rObj.GetKeywd(), N, M );
        //        vfMessage(this, str.p, "CellCheck: Bad value, reset to <empty>!" );
        currentIndex = 4;
        setText( "`" /*defVALS[4]*/ );
        //     throw TError( "CellCheck validator: Bad value!", str.p );
    }
    else {
        currentIndex = ind;
        setText( s.c_str() );
    }
}

void
TCellCheck::SetIndex(int ii)
{
    if( ii >= (int)Vals.length() )		// 'Help' or 'Calc' menu
        return;

    gstring val(Vals, ii, 1);
    rObj.SetString(val.c_str(), N, M);
    setText(val.c_str());

    field()->objectChanged();
}


void
TCellCheck::focusInEvent(QFocusEvent* e)
{
    QLineEdit::focusInEvent(e);

    field()->setFocused(this);
}


void
TCellCheck::focusOutEvent(QFocusEvent* e)
{
    field()->setFocused(NULL);

    QLineEdit::focusOutEvent(e);
}


void
TCellCheck::updateDisplay()
{
    SetString(rObj.GetString(N,M));
}

void
TCellCheck::CmCalc()
{
    if( !edit )
        return;

    // check selection
    QRect sel(M,N,1,1);
    CalcCheckDialog calc(topLevelWidget(), rObj, Vals, sel);

    if( calc.exec() )
    {
        int ii = calc.fun();
        for(int nn=sel.top(); nn<=sel.bottom(); nn++)
            rObj.SetString( gstring(Vals, ii, 1).c_str(),
                            nn, M);
        field()->objectChanged();
        emit fieldUpdate();
    }
}

void
TCellCheck::keyPressEvent(QKeyEvent* e)
{
    switch( e->key() )
    {
    case Qt::Key_F1:
        CmHelp();
        return;
    case Qt::Key_F8:
        CmCalc();
        return;
    }

    if( !edit )
        return;

    int key = e->ascii();

    if( key == ' ' )
    {
        uint newIndex = ++currentIndex;
        if( newIndex == Vals.length() )
            newIndex = 0;

        SetIndex(newIndex);
    }
    else
    if( key >= 0x21 && key < 0xFF )
    {
        size_t newIndex = Vals.find(char(key));
        if( newIndex == gstring::npos )
            return;

        SetIndex(newIndex);
    }
}


Q3PopupMenu*
TCellCheck::createPopupMenu()
{
    Q3PopupMenu* menu = new Q3PopupMenu();
    menu->insertItem("&Help\tF1", this, SLOT(CmHelp()),
                     Qt::Key_F1, Vals.length());
    menu->insertSeparator();
    menu->setItemEnabled(
        menu->insertItem("&Calculator\tF8", this, SLOT(CmCalc()),
                         Qt::Key_F8, Vals.length()+1 ), edit );

    menu->insertSeparator();
    for(uint ii=0; ii<Vals.length(); ii++)
    {
        gstring s(Vals, ii, 1);
        int id = menu->insertItem(s.c_str(), ii);
        menu->setItemEnabled(id, edit);
    }
    connect(menu, SIGNAL(activated(int)), this, SLOT(SetIndex(int)));

    return menu;
}

//------------------------------------------------
// TCellText
//------------------------------------------------

TCellText::TCellText(TField& rfield, int xx, int yy,
                     int npos, eShowType showType_,
                     TObject& rO, int n, int m, bool ed, int ht):
        Q3TextEdit(&rfield),
        TCell(rO, n, m, ed, showType_, this)
{
    move(xx, yy);
    setFixedSize(wdF(ftText, npos), htF(ftText, ht));

    if( edit ) {
        connect(this, SIGNAL(textChanged()), this, SLOT(EvChange()) );
        connect(this, SIGNAL(fieldUpdate()), &rfield, SLOT(Update()));
    }

    //    setMaxLength(AllowedPos(rO)+1);
    //    setFont( pVisorImp->getCellFont() );
    setReadOnly( !edit );
    setTextFormat(Qt::PlainText);

    updateCellBackground();
    setText( visualizeEmpty(rObj.GetString(N,M)).c_str() );

    QToolTip::add(this, (rObj.GetDescription(N,M)).c_str() );
}


void
TCellText::closeEvent(QCloseEvent* e)
{
    if( isModified() )
        setValue();

    Q3TextEdit::closeEvent(e);
}


void
TCellText::updateDisplay()
{
    setText(visualizeEmpty(rObj.GetString(N,M)).c_str());
}


void
TCellText::EvChange()
{
    if( text().length() >= AllowedPos(rObj)+1 ) {
        vfMessage(topLevelWidget(), "Text object", "String is too long for this object", vfErr);
	undo();
    }
}

void
TCellText::setIfChanged()
{
    if( isModified() )
        setValue();
}


void
TCellText::setValue()
{
    /*
        gstring s;
        for(int ii=0; ii<lines()-1; ii++ )
        {
            s += textLine(ii);
            s += '\n';
        }
        s += textLine(ii);
    */
    setModified(false);
    if( !rObj.SetString(text().remove('\r'), N, M) )
    {
        vfMessage(topLevelWidget(), rObj.GetKeywd(), "Sorry! Wrong value typed!" );
        setText( visualizeEmpty(rObj.GetString(N,M)).c_str() );
    }
    else
        field()->objectChanged();
}


void
TCellText::focusInEvent(QFocusEvent* e)
{
    Q3TextEdit::focusInEvent(e);

    setModified(false);
    field()->setFocused(this);
}

void
TCellText::focusOutEvent(QFocusEvent* e)
{
    if( isModified() )
        setValue();
    field()->setFocused(NULL);

    Q3TextEdit::focusOutEvent(e);
}

void
TCellText::keyPressEvent(QKeyEvent* e)
{
    switch( e->key() )
    {
    case Qt::Key_F1:
        CmHelp();
        return;
    }
/*
    if( !edit || e->key()==Key_Delete || e->ascii()=='\0' )
    {
        QTextEdit::keyPressEvent(e);
        return;
    }

    if( e->ascii()==0x08 ||
            ( IsCharAllowed(rObj,e->ascii()) &&
              text().length() < AllowedPos(rObj)+1 )    )
*/
    Q3TextEdit::keyPressEvent(e);

//    if( text().length() >= AllowedPos(rObj)+1 
//	    ||  text().section(QRegExp(GetAllowedRegExp(rObj)), 0) != text() )
//	undo();
}

/*! mouse press event
    handles context menu
*/
Q3PopupMenu*
TCellText::createPopupMenu(const QPoint& pos)
{
    Q3PopupMenu* menu = new Q3PopupMenu();
    menu->insertItem( "&Help\tF1", this, SLOT(CmHelp()), Qt::Key_F1 );
    menu->insertSeparator();
    menu->insertItem( "&Edit", Q3TextEdit::createPopupMenu(pos) );
    return menu;
}



//---------------------- End of file  page_w.cpp ---------------------------
