//-------------------------------------------------------------------
// $Id$
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
// See http://les.web.psi.ch/Software/GEMS-PSI/ for more information
// E-mail gems2.support@psi.ch
//-------------------------------------------------------------------
#include <stdio.h>

#include <qapplication.h>
#include <qpushbutton.h>
#include <qtooltip.h>
#include <qkeycode.h>
#include <qaccel.h>

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
#include "dlg/CalcDialog.h"
#include "dlg/CalcCheckDialog.h"


const char *GEMS_OBJNDX_HTML = "g_objndx.html";

bool IsCharAllowed(TObject& obj, uint ch)
{
    if( ch == '*' || ch == '`' )
        return true;

    ObjType type = obj.GetType();
    switch ( type )
    {
    case I_: 		//<short>
    case L_: 		//<long>
    case H_: 		//<signed char>
        return	(IsDigit(ch) || IsSign(ch));
    case U_: 		//<unsigned short>
    case X_: 		//<unsigned long>
    case B_: 		//<unsigned char>
        return	IsDigit(ch);
    case F_: 		//<float>
    case D_: 		//<double>
        return	(IsDigit(ch) || IsSign(ch) || IsEorP(ch));
    case A_: 		//<char> - not used here
    case S_: 		//TValString
    default:		// 1..127
        return	true;
    }
}

//inline
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
        return	13+1;
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
        //         p = new TValFixString(type, sz, Dynamic);
    }

}


const int wdWIN = 500;
const int htWIN = 400;

const int wdTab = 200;
const int szScroll = 15;

const int X0 = 10;	// position of the 1st Field
const int Y0 = 10;	// on the Page


//----------------------------------------------------------------
// TCPage
//----------------------------------------------------------------

TCPage::TCPage(PageInfo& r):
        QWidget(&r.GetWin()),
        rInfo(r),
        fRedraw(true),
	firstRedraw(true)
{
    r.pPage = this;

    for( int ii=0; ii<getFieldCnt(); ii++ )
    { // added Sveta 24/08/01
       rInfo.aFieldInfo[ii].pField = 0;
    }
    mwidth = x = X0;
    mheight = y = Y0;

    hide();
    move(0, szTab);
}

TCPage::~TCPage()
{}


void
TCPage::show()
{
    if( fRedraw )
        RedrawFields();

    QWidget::show();
}


/*!
    Updates the page
*/
void
TCPage::Update()
{
    for( int ii=0; ii<getFieldCnt(); ii++ )
    {
        bool appeared = false;
        if( !getField(ii) )
            if( !rInfo.aFieldInfo[ii].pObj->IsNull() )
                appeared = true;
            else
                continue;
        if( appeared || getField(ii)->SizeChanged() )
        {
            fRedraw = true;
            if( isVisible/*ToTLW*/() )
                RedrawFields();
            // if not - redrawn by EV_SHOW
            return;
        }
        getField(ii)->Update();
    }
}


void
TCPage::RedrawFields()
{
    fRedraw = false;

    if( !firstRedraw ) {
    for( int ii=0; ii<getFieldCnt(); ii++ )
        if( getField(ii) )
        {
            delete getField(ii);
            rInfo.aFieldInfo[ii].pField=0;
        }
    }
    else
	firstRedraw = false;

    AddFields();
}

void
TCPage::AddFields()
{
    TField* prevField = 0;
    int LineH = pVisorImp->getCharHeight();

    for( int ii=0; ii<getFieldCnt(); ii++ )
    {
        FieldInfo& fi = rInfo.aFieldInfo[ii];
        if( fi.pObj->IsNull() )
        {   fi.pField = NULL;    //added Sveta 24/08/01
            continue; //return;
        }
        TField* pTied = 0;
        TField* pSticked = 0;

        // calculating position of the element

        int prevHeight;  // for 'case Under'
        if( prevField )
        {
            switch( fi.place )
            {
            case FieldInfo::First:
                x=10;
                y=10;
                break;

            case FieldInfo::Tied:
                pTied = prevField;
    int jj;
    for( jj = ii+1; jj<getFieldCnt() && rInfo.aFieldInfo[jj].pObj->IsNull(); jj++ );	// searching for next 'not null' field
    if( jj == getFieldCnt() || rInfo.aFieldInfo[jj].place != FieldInfo::Tied )
	prevField->indTied = -1;	// this is the last field in the chain
	
            case FieldInfo::Right:
                x += prevField->GetW();
                break;

            case FieldInfo::Fixed:
            case FieldInfo::NextP:
                x += prevField->GetW() + wdFSPACE;
                break;

            case FieldInfo::Sticked:
                pSticked = prevField;
            case FieldInfo::Down:
                y += LineH + htFSPACE;
                break;

            case FieldInfo::Under:
                y += prevField->GetH() + htFSPACE;
                break;

            case FieldInfo::SkipLine:
                y += LineH + pVisorImp->getCharHeight();
                x = 10;
                break;
            }
            prevHeight = prevField->GetH();
        }
        else
            x = X0, y = Y0, LineH = pVisorImp->getCharHeight(),
                                    prevHeight = 0;

        fi.pField = new TField(this, fi, x, y, pTied, pSticked);
        if( !prevField )
            fi.pField->SetFirstCellFocus();

        prevField = fi.pField;
        prevField->show();

        // size of the window
        mwidth = max( mwidth, x + prevField->GetW() );
        mheight = max( mheight, y + prevField->GetH() );

        // estimating the height of the line
        switch( fi.place )
        {
        case FieldInfo::First:
            LineH = fi.pField->GetH();
            break;
        case FieldInfo::Tied:
        case FieldInfo::Right:
        case FieldInfo::Fixed:
        case FieldInfo::NextP:
            LineH = max(LineH,fi.pField->GetH());
            break;
        case FieldInfo::Under:
            LineH = max(LineH,fi.pField->GetH()+prevHeight);	// first variant
            //; leaving the height of previous element		// second one
            break;
        case FieldInfo::Sticked:
        case FieldInfo::Down:
        case FieldInfo::SkipLine:
            LineH = fi.pField->GetH();
            break;
        }
    }

    resize(mwidth+5, mheight+5);
    // update();
}


void
TCPage::SetFirstCellFocus()
{
    if( rInfo.aFieldInfo[0].pField )
        rInfo.aFieldInfo[0].pField->SetFirstCellFocus();
}

//----------------------------------------------------------------
// TField
//----------------------------------------------------------------

TField::TField(QWidget* p, const FieldInfo& fi, int xx0, int yy0,
               TField* tied, TField* sticked):
        QWidget(p),
        rInfo(fi),
        aCtrl(rInfo.maxN*rInfo.maxM),
        N(rInfo.pObj->GetN()),
        M(rInfo.pObj->GetM()),
        Lab(rInfo.label),
        X(xx0), Y(yy0),
        pTied(0),
        pSticked(0)
{
    focused = 0;
    pSV = pSH = 0;
    indTied = 0;

    //--- Circular list section ---
    if( tied )
    {
        TField*& TiedToLast = tied->pTied;
        if( TiedToLast )	// previous field already in the chain
	{
            pTied = TiedToLast;
        }
	else {
            pTied = tied;	// second field in new chain
	    tied->indTied = 0;
	}
	indTied = tied->indTied + 1;
	    
        TiedToLast = this;
    }
    
    //--- Circular list section ---
    if( sticked )
    {
        TField*& StickedToLast = sticked->pSticked;
        if( StickedToLast )
            pSticked = StickedToLast;
        else
            pSticked = sticked;
        StickedToLast = this;
    }

    iN = iM = 0;
    xx = 0;
    yy = 0;

    ObjType t = GetObj().GetType();

    largeM = largeN = false;
    int n = 1;
    int m = 1;
    if( t != S_ )
    {
        if( N > rInfo.maxN )
            largeN = true;
        if( M > rInfo.maxM )
            largeM = true;
        n = (largeN) ? rInfo.maxN : N;
        m = (largeM) ? rInfo.maxM : M;
    }

    // --- Label section ---

    if( Lab )
    {
        QLabel* p = new QLabel(this);
        p->setGeometry(0, 0, pVisorImp->getLabelWidth(), pVisorImp->getCharHeight());
        p->setFont( pVisorImp->getCellFont() );
        p->setText(GetObj().GetKeywd());
        //    p->show();

        if( GetObj().GetN() > 1 )
        {
            p->setAlignment( AlignLeft );
            yy += pVisorImp->getCharHeight() + htSPACE;
        }
        else
        {
            p->setAlignment( AlignRight );
            xx += pVisorImp->getLabelWidth() + wdSPACE;
        }
    }
    else		// aligning with labels of tied fields
    {
        TField* t = pTied;
        if( tied )
            for( ; t!=this; t=t->pTied )
                if( t->Lab )
                {
                    Lab = true;	// creation of scroll bars
                    // depends on Lab
                    yy += pVisorImp->getCharHeight() + htSPACE;
                    break;
                }
    }

    int x0 = xx;	//position of element(0,0) (without label)

    bool fedit = ((fi.edit==eYes) ? true : false);

    const int& W = xx;
    const int& H = yy;

    // --- Cells section ---

    for( int ii=0; ii<n; ii++ )
    {
        xx = x0;		// left align
        switch( fi.fType )
        {
        case ftCheckBox:
            for( int jj=0; jj<m; jj++ )
            {
                aCtrl.Add( new TCellCheck(*this, xx, yy,
                                          fi.npos, fi.showType, GetObj(), ii, jj, fedit) );
                xx += wdF(fi.fType, fi.npos);// + wdSPACE;
            }

            break;
        case ftText:
            for( int jj=0; jj<m; jj++ )
            {
                aCtrl.Add( new TCellText(*this, xx, yy,
                                         fi.npos, fi.showType, GetObj(), ii, jj, fedit, rInfo.maxN) );
                xx += wdF(fi.fType, fi.npos);// + wdSPACE;
            }

            break;
        default:
            for( int jj=0; jj<m; jj++ )
            {
                aCtrl.Add( new TCellInput(*this, xx, yy,
                                          fi.fType, fi.npos, fi.showType, GetObj(), ii, jj, fedit, rInfo.maxN) );
                xx += wdF(fi.fType, fi.npos);// + wdSPACE;
            }

        }
        yy += htF(fi.fType, rInfo.maxN);// + htSPACE;
    }

    // --- Scroll bars section ---

    if( largeN  && indTied == 0 )
        xx += szSBAR;
    if( largeM )
        yy += szSBAR;

    if( largeM )
    {
        int labW = (Lab && GetObj().GetN()==1) ? pVisorImp->getLabelWidth()+wdSPACE : 0;
        pSH = new QScrollBar(
                  0, GetObj().GetM()-rInfo.maxM,
                  1, rInfo.maxM,
                  0, QScrollBar::Horizontal, this);
        connect( pSH, SIGNAL(valueChanged(int)), SLOT(EvHScroll(int)) );
        pSH->move(labW, H - szSBAR);
        pSH->setFixedSize(W - ((largeN)?szSBAR:0) - labW, szSBAR);
        //    pSH->show();
    }

    if( largeN && indTied == 0 )
    {
        int labH = (Lab && GetObj().GetN()>1) ? pVisorImp->getCharHeight()+htSPACE : 0;
        pSV = new QScrollBar(
                  0, GetObj().GetN()-rInfo.maxN,
                  1, rInfo.maxN,
                  0, QScrollBar::Vertical, this);
        connect( pSV, SIGNAL(valueChanged(int)), this, SLOT(EvVScroll(int)) );
        pSV->move(W - szSBAR, labH);
        pSV->setFixedSize(szSBAR, H - ((largeM)?szSBAR:0) - labH);/// false);
        //    pSV->show();
    }

    setGeometry(X, Y, xx, yy);
}

TField::~TField()
{}


void
TField::Update()
{
    if( focused )	// save value of focused cell -- This is possible ERROR
        focused->setIfChanged();

    uint minM = min(GetObj().GetM(), rInfo.maxM);
    for(uint ii=0; ii<aCtrl.GetCount(); ii++)
        aCtrl[ii]->SetNM((ii/minM)+iN, (ii%minM)+iM);

    if( focused )
        focused->SetDescription();
}

void
TField::setYPos(int pos)
{
    iN = pos;
    Update();
    if( pSV )
	pSV->setValue(iN);
}

void
TField::setXPos(int pos)
{
    //  if( !pSH ) return;		/// Possibly ERROR !
    pSH->setValue(iM=pos);
    Update();
}

void
TField::EvVScroll(int n)
{
    if( n == iN )		// to prevent recursion of tied lists
        return;

    iN = n;
    Update();

    if( !pTied )
        return;

    for(TField* t = pTied; t!=this; t=t->pTied)
        t->setYPos( n );
}

void
TField::EvHScroll(int m)
{
    if( m == iM )		// to prevent recursion of tied lists
        return;

    iM = m;
    Update();

    if( !pSticked )
        return;

    for(TField* t = pSticked; t!=this; t=t->pSticked)
        t->setXPos( m );
}

bool
TField::SizeChanged()
{
    if( GetObj().IsDynamic() &&
            (GetObj().GetN()!=N || GetObj().GetM()!=M) )
        return true;
    return false;
}

void
TField::SetFirstCellFocus()
{
    focused = aCtrl[0];
    if( focused )
    {
        focused->pw->setFocus();
        focused->SetDescription();
    }
}

//------------------------------------------------
// TCell
//------------------------------------------------

void
TCell::SetDescription()
{
    QWidget* topw = pw->topLevelWidget();
    const gstring& desc = rObj.GetDescription(rObj.ndx(N,M));
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
    gstring item(rObj.GetKeywd());
    item += "_";
    vstr v(15);
    int Ndx = rObj.ndx( N, M );
    sprintf(v, "%u", Ndx );  // N);  fixed 16.01.01
    item += v;  /// # of the cell
    gstring item1 = item;
    try
    {
        gstring docDir = pVisor->sysGEMDir();
        docDir += "doc/html/";
        Helper hlp(docDir);
// Get file URL to DOD item description
        hlp.checkForRef( GEMS_OBJNDX_HTML, item );

        if( item.empty() )
            vfMessage(pw, "Help",(item1+=": No such item in HTML files!").c_str(), vfErr);
        else
// Changed to return file locator in item   DAK 05.01.01
            pVisorImp->OpenHelp( item.c_str() );
//          pVisorImp->OpenHelp("objind.html", item.c_str() );
    }
    catch(...)
    {
        vfMessage(pw, "Helper","File browsing error");
    }
}


void
TCell::ObjectChanged()
{
    QWidget* topw = pw->topLevelWidget();
    if( topw->inherits("QWidget") &&  !topw->inherits("QDialog") )
        ((TCModuleImp*)(topw))->CellChanged();
}


void TCell::updateCellBackground()
{
    
    switch( showType )
    {
    case stResult:
	pw->setPalette(QPalette(Qt::cyan, pw->backgroundColor()));
    break;
    case stIO:
	pw->setPalette(QPalette(Qt::green, pw->backgroundColor()));
    break;
    case stWork:
	pw->setPalette(QPalette(Qt::blue, pw->backgroundColor()));
    break;
    case stAux:
	pw->setPalette(QPalette(Qt::yellow, pw->backgroundColor()));
    break;
    case stHelp:
	pw->setPalette(QPalette(Qt::gray, pw->backgroundColor()));
    break;
    }
}


// Critical function for visual speed
// gone to page_w.h to be 'inline'
/*
void
TCell::SetNM(int n, int m)
{
    // Sveta 12/05/99 saveIfChanged();
    N = n, M = m;
    Update();
    if( pw->hasFocus() )
	SetDescription();
}
*/


/*! temporary workaround to make empty value look better

*/

const gstring emptiness("---");
const gstring short_emptiness("---");

gstring visualizeEmpty(const gstring& text)
{
    return (text==S_EMPTY) ? emptiness : text;
}


//------------------------------------------------
// TCellInput
//------------------------------------------------

TCellInput::TCellInput(TField& rfield, int xx, int yy,
                       eFieldType ft, int npos, eShowType showType_,
                       TObject& rO, int n, int m, bool ed, int ht):
        QLineEdit(&rfield),
        TCell(rO, n, m, ed, showType_, this),
        fieldType(ft),
        changed(false)
{
    move(xx, yy);
    setFixedSize(wdF(fieldType, npos), htF(fieldType, ht));
    if( !edit )
        setReadOnly(true);
    else
    {
	connect(this, SIGNAL(textChanged(const QString&)), this, SLOT(EvChange()) );
        connect(this, SIGNAL(fieldUpdate()), &rfield, SLOT(Update()));
    }
    setMaxLength(AllowedPos(rO)+1);

    switch( fieldType )
    {
    case ftNumeric:
    case ftFloat:
        setAlignment(Qt::AlignRight);
        break;
    default:
        ;
    }

    setFont( pVisorImp->getCellFont() );
    updateCellBackground();
    Update();
}


void
TCellInput::closeEvent(QCloseEvent* e)
{
    if( hasFocus() && changed )
        setValue();

    QLineEdit::closeEvent(e);
}


void
TCellInput::Update()
{
    setText( visualizeEmpty(rObj.GetString(N,M)).c_str() );
    setCursorPosition(0);

    /// probably ERROR/leak !!!!!!!!
    /// Should we call
    /// QToolTip::remove(this); ??
    QToolTip::add(this, (rObj.GetDescription(rObj.ndx(N,M))).c_str() );
}

void
TCellInput::EvChange()
{
    if( edited() )	// it reacts now only on keyboard changes
	changed = true;
}


void
TCellInput::setIfChanged()
{
    if( changed )
        setValue();
}


void
TCellInput::setValue()
{
    changed = false;
    if( !rObj.SetString(text(), N, M) )
    {
	if( text() == emptiness || text() == short_emptiness )
	{
	    rObj.SetString(S_EMPTY, N, M);
	    setText( visualizeEmpty(rObj.GetString(N,M)).c_str() );
	}
	else
	{
    	    vfMessage(this, rObj.GetKeywd(), "Sorry! Wrong value typed!" );
    	    setText( visualizeEmpty(rObj.GetString(N,M)).c_str() );
    	    setCursorPosition(0);
	}
    }
    else
    {
        ObjectChanged();
	setText( visualizeEmpty(rObj.GetString(N,M)).c_str() );
    }
}


void
TCellInput::focusInEvent(QFocusEvent* e)
{
    QLineEdit::focusInEvent(e);
    field()->focused = this;
    SetDescription();
    changed = false;
//    if( rObj.IsEmpty(N, M) )
//	selectAll();
}


void
TCellInput::focusOutEvent(QFocusEvent* e)
{
    field()->focused = 0;
    QLineEdit::focusOutEvent(e);

    if( changed )
        setValue();
}

void
TCellInput::keyPressEvent(QKeyEvent* e)
{
    switch( e->key() )
    {
    case Key_Enter:
    case Key_Return:
        if( changed )
    	    setValue();
	return;
    case Key_F1:
        CmHelp();
        return;
    case Key_F2:
        CmSDRef();
        return;
    case Key_F7:
        CmDComp();
        return;
    case Key_F8:
        CmCalc();
        return;
    }

/*    if( !edit )
        return;
test Sveta*/
    if( e->key()==Key_Delete || e->ascii()=='\0' )
    {
        QLineEdit::keyPressEvent(e);
        return;
    }
   if( !edit )
        return;
/*test Sveta*/

    if( e->ascii()=='<' && !IsCharAllowed(rObj, e->ascii()) )
    {
	setText("<empty>");
	setValue();
    }
    else
        if( e->ascii()==0x08 ||
            IsCharAllowed(rObj, e->ascii()) )
	{
	    if( text() == emptiness )
		clear();
            QLineEdit::keyPressEvent(e);
	}
}

/*! mouse press event
    handles context menu
*/
void
TCellInput::mousePressEvent(QMouseEvent* e)
{
    if( e->button() != RightButton )
    {
        QLineEdit::mousePressEvent(e);
        return;
    }
    //  if( !edit )
    //    return;

    QPopupMenu* menu = new QPopupMenu;
    CHECK_PTR( menu );
    menu->insertItem( "&Help\tF1", this, SLOT(CmHelp()), Key_F1 );
    if( fieldType == ftRef )
        menu->insertItem( "&SDReference\tF2", this, SLOT(CmSDRef()), Key_F2 );
    if( fieldType == ftRecord )
        menu->insertItem( "&Show record\tF7", this, SLOT(CmDComp()), Key_F7 );
    if( fieldType == ftFloat )
    {
        menu->insertSeparator();
        menu->setItemEnabled(
            menu->insertItem( "&Calculator\tF8", this,
                              SLOT(CmCalc()), Key_F8 ), edit );
    }

    menu->exec(e->globalPos());
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
                    TSData::pm->RecSave( sd_key.c_str() );
            }
        }

        if( rt[RT_SDATA].Find( str.c_str() )>= 0)
            TSData::pm->RecInput( str.c_str() );
        pVisorImp->OpenModule(topLevelWidget(), RT_SDATA);
        TSData::pm->Update();
    }
    catch( TError& xcpt )
    {
        vfMessage(this, xcpt.title, xcpt.mess );
        return;
    }
}

void
TCellInput::CmCalc()
{
    if( !edit || fieldType!=ftFloat )
        return;

    // check selection
    QRect sel(M,N,1,1);
    CalcDialog calc(pw->topLevelWidget(), rObj, sel);

    if( calc.exec() )
    {
        for(int nn=sel.top(); nn<=sel.bottom(); nn++)
            for(int mm=sel.left(); mm<=sel.right(); mm++ )
                rObj.Put(calc.fun(rObj.Get(nn, mm)), nn, mm);
        ObjectChanged();
        emit fieldUpdate();
    }
}

void
TCellInput::CmDComp()
{
    if( fieldType != ftRecord )
        return;

    TProfil::pm->ShowDBWindow(rObj.GetKeywd(), N);
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

    setFont( pVisorImp->getCellFont() );
    updateCellBackground();

    SetString( rObj.GetString(N,M) );

    QToolTip::add(this, (rObj.GetDescription(rObj.ndx(N,M))).c_str() );
}

void
TCellCheck::SetString(const gstring& s)
{
    size_t ind1 = Vals.find(s);
    if( ind1 == gstring::npos )
    {
//        vstr str(80);
//        sprintf( str, "%s[%d:%d] ", rObj.GetKeywd(), N, M );
//        vfMessage(this, str.p, "CellCheck: Bad value, reset to <empty>!" );
        ind = 4;
        setText( "`" /*defVALS[4]*/ );
//     throw TError( "CellCheck validator: Bad value!", str.p );
    }
    else {
        ind = ind1;
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
    ObjectChanged();
}


void
TCellCheck::focusInEvent(QFocusEvent* e)
{
    QLineEdit::focusInEvent(e);
    field()->focused = this;
    SetDescription();
}


void
TCellCheck::focusOutEvent(QFocusEvent* e)
{
    QLineEdit::focusOutEvent(e);
    field()->focused = 0;
}


void
TCellCheck::Update()
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
    CalcCheckDialog calc(pw->topLevelWidget(), rObj, Vals, sel);

    if( calc.exec() )
    {
        int ii = calc.fun();
        for(int nn=sel.top(); nn<=sel.bottom(); nn++)
            rObj.SetString( gstring(Vals, ii, 1).c_str(),
                            nn, M);
        ObjectChanged();
        emit fieldUpdate();
    }
}

void
TCellCheck::keyPressEvent(QKeyEvent* e)
{
    switch( e->key() )
    {
    case Key_F1:
        CmHelp();
        return;
    case Key_F8:
        CmCalc();
        return;
    }

    if( !edit )
        return;

    int key = e->ascii();

    if( key == ' ' )
    {
        if( ++ind == Vals.length() )
            ind = 0;

        gstring val(Vals, ind, 1);
        rObj.SetString(val.c_str(), N, M);
        setText(val.c_str());
        ObjectChanged();
        return;
    }

    if( key >= 0x21 && key < 0xFF )
    {
        size_t ind1 = Vals.find(char(key));
        if( ind1 == gstring::npos )
            return;

        ind = ind1;
        gstring val(Vals, ind, 1);
        rObj.SetString(val.c_str(), N, M);
        setText(val.c_str());
        ObjectChanged();
    }
}


void
TCellCheck::mousePressEvent(QMouseEvent* e)
{
    if( e->button() != RightButton )
    {
        QLineEdit::mousePressEvent(e);
        return;
    }
    //  if( !edit )
    //    return;

    QPopupMenu* menu = new QPopupMenu;
    CHECK_PTR(menu);

    menu->insertItem("&Help\tF1", this, SLOT(CmHelp()),
                     Key_F1, Vals.length());
    menu->insertSeparator();
    menu->setItemEnabled(
        menu->insertItem("&Calculator\tF8", this, SLOT(CmCalc()),
                         Key_F8, Vals.length()+1 ),
        edit );
    menu->insertSeparator();

    for(uint ii=0; ii<Vals.length(); ii++)
    {
        gstring s(Vals, ii, 1);
        int id = menu->insertItem(s.c_str(), ii);
        menu->setItemEnabled(id, edit);
    }
    connect(menu, SIGNAL(activated(int)),
            this, SLOT(SetIndex(int)));
    menu->exec(e->globalPos());
}


//------------------------------------------------
// TCellText
//------------------------------------------------

TCellText::TCellText(TField& rfield, int xx, int yy,
                     int npos, eShowType showType_,
                     TObject& rO, int n, int m, bool ed, int ht):
        QMultiLineEdit(&rfield),
        TCell(rO, n, m, ed, showType_, this),
        changed(false)
{
    move(xx, yy);
    setFixedSize(wdF(ftText, npos), htF(ftText, ht));
    if( !edit )
        setReadOnly(true);
    else
    {
        connect(this, SIGNAL(textChanged()), this, SLOT(EvChange()) );
        connect(this, SIGNAL(fieldUpdate()), &rfield, SLOT(Update()));
    }

    ///  setMaxLength(AllowedPos(rO)+1);
    //  setReadOnly( !edit );
    setFont( pVisorImp->getCellFont() );
    updateCellBackground();

    setText( visualizeEmpty(rObj.GetString(N,M)).c_str() );

    //  setTableFlags( Tbl_smoothScrolling /*| Tbl_autoVScrollBar*/ );
    //  clearTableFlags( /*Tbl_autoHScrollBar*/ );
    QToolTip::add(this, (rObj.GetDescription(rObj.ndx(N,M))).c_str() );
}


void
TCellText::closeEvent(QCloseEvent* e)
{
    if( hasFocus() && changed )
        setValue();

    QMultiLineEdit::closeEvent(e);
}


void
TCellText::Update()
{
    setText(visualizeEmpty(rObj.GetString(N,M)).c_str());
    //  if( hasFocus() )
    //      SetDescription();
}


void
TCellText::EvChange()
{
    if( edited() )	// it reacts now only on keyboard changes
	changed = true;
}

void
TCellText::setIfChanged()
{
    if( changed )
        setValue();
    SetDescription();
}


void
TCellText::setValue()
{
    gstring s;
    int ii;
    for( ii=0; ii<numLines()-1; ii++ )
    {
        s += textLine(ii);
        s += '\n';
    }
    s += textLine(ii);

    changed = false;
    if( !rObj.SetString(s.c_str(), N, M) )
    {
        vfMessage(this, rObj.GetKeywd(), "Sorry! Wrong value typed!" );
        setText( visualizeEmpty(rObj.GetString(N,M)).c_str() );
    }
    else
        ObjectChanged();
}


void
TCellText::focusInEvent(QFocusEvent* e)
{
    QMultiLineEdit::focusInEvent(e);
    field()->focused = this;
    SetDescription();
    changed = false;
}

void
TCellText::focusOutEvent(QFocusEvent* e)
{
    QMultiLineEdit::focusOutEvent(e);
    field()->focused = 0;

    if( changed )
        setValue();
}

void
TCellText::keyPressEvent(QKeyEvent* e)
{
    switch( e->key() )
    {
    case Key_F1:
        CmHelp();
        return;
    }

    if( !edit || e->key()==Key_Delete || e->ascii()=='\0' )
    {
        QMultiLineEdit::keyPressEvent(e);
        return;
    }

    if( e->ascii()==0x08 ||
            ( IsCharAllowed(rObj,e->ascii()) &&
              text().length() < AllowedPos(rObj)+1 )    )
        QMultiLineEdit::keyPressEvent(e);
}

/*! mouse press event
    handles context menu
*/
void
TCellText::mousePressEvent(QMouseEvent* e)
{
    if( e->button() != RightButton )
    {
        QMultiLineEdit::mousePressEvent(e);
        return;
    }
    //  if( !edit )
    //    return;

    QPopupMenu* menu = new QPopupMenu;
    CHECK_PTR( menu );
    menu->insertItem( "&Help\tF1", this, SLOT(CmHelp()), Key_F1 );
    menu->exec(e->globalPos());
}


//==========================================
// TQueryWindow
//==========================================


TQueryWindow::TQueryWindow(CWinInfo& w):
        QDialog(w.pWin, 0, true),
        rInfo(w)
{
    gstring s = w.rM.GetName();
    s += ": Remake parameters";
    setCaption(s.c_str());

    AddFields();
}

// !!! TQueryWindow does not have 'Cancel' button
// since it modifies cells right in their memory
// workaround would be pretty time-consuming
void
TQueryWindow::done(int result)
{
    // focus out for last focused widget
    if( qApp->focusWidget() )
        qApp->focusWidget()->clearFocus();
    // save state for user window

    rInfo.pWin->Update();
    QDialog::done(result);
}

// adds all fields from given module window
// (only from page 0) now try from the last page
// that have 'param' flag set to the Query dialog
//
void
TQueryWindow::AddFields()
{
    TField* prevField = 0;	// previous cell
    int LineH = pVisorImp->getCharHeight();
//  PageInfo& pginfo = rInfo.aPageInfo[0];
    int LastPage = rInfo.aPageInfo.GetCount()-1;
    PageInfo& pginfo = rInfo.aPageInfo[LastPage];
//  Fixed for collecting flags from the last page  DAK

    int x = X0, y = Y0;
    int mwidth = X0, mheight = Y0;

    for( uint ii=0; ii<pginfo.aFieldInfo.GetCount(); ii++ )
    {
        FieldInfo& fi = pginfo.aFieldInfo[ii];
        if( fi.edit != eParam )
            continue;
        if( fi.pObj->IsNull() )
        {    fi.pField = NULL;    //added Sveta 24/08/01
             continue; /// may be error?
        }

        TField* pSticked = 0;
        TField* pTied = 0;

        if( prevField )
            switch( fi.place )
            {
            case FieldInfo::First:
                x=X0;
                y=Y0;
                break;
            case FieldInfo::Tied:
                pTied = prevField;
            case FieldInfo::Right:
                x += prevField->GetW();
                break;
            case FieldInfo::Fixed:
            case FieldInfo::NextP:
                x += prevField->GetW() + wdFSPACE;
                break;
            case FieldInfo::Sticked:
                pSticked = prevField;
            case FieldInfo::Down:
                y += LineH + htFSPACE;
                break;
            case FieldInfo::Under:
                y += prevField->GetH() + htFSPACE;
                break;
            case FieldInfo::SkipLine:
                y += LineH + pVisorImp->getCharHeight();
                x = 10;
                break;
            }
        else
            x = X0, y = Y0, LineH = pVisorImp->getCharHeight();

        FieldInfo nfi = fi;
        nfi.edit = eYes;
        nfi.label = false; /// maybe leave it?

        nfi.pField = new TField(this, nfi, x, y, pTied, pSticked);
        if( !prevField )
            nfi.pField->setFocus();
        prevField = nfi.pField;
        prevField->show();

        mwidth = max( mwidth, x + prevField->GetW() );
        mheight = max( mheight, y + prevField->GetH() );

        switch( fi.place )
        {
        case FieldInfo::First:
            if( fi.pField == NULL )    // Bugfix  DAK 19.12.00
                LineH = pVisorImp->getCharHeight();
            else LineH = fi.pField->GetH();
            break;
        case FieldInfo::Tied:
        case FieldInfo::Right:
        case FieldInfo::Fixed:
        case FieldInfo::NextP:
            if( fi.pField == NULL )    // Bugfix  DAK 19.12.00
                LineH = pVisorImp->getCharHeight();
            else LineH = max(LineH,fi.pField->GetH());
            break;
        case FieldInfo::Under:
            //; leaving the height of previous element	// second one
            break;
        case FieldInfo::Sticked:
        case FieldInfo::Down:
        case FieldInfo::SkipLine:
            if( fi.pField == NULL )    // Bugfix  DAK 19.12.00
                LineH = pVisorImp->getCharHeight();
            else LineH = fi.pField->GetH();
            break;
        }
    }

    //  if( prevField == 0 )
    //    accept();

    if( mwidth < 40 )
        mwidth = 40;

    setFixedSize(mwidth+10, mheight+15+40);

    hint = new QLabel(this);
    hint->setGeometry(3, mheight+4, mwidth, 12);
    hint->setText("Please select any field");
    hint->show();

    QPushButton* btn = new QPushButton(this);
    btn->setGeometry(3, mheight+21, mwidth, 30);
    btn->setText("&Ok");
    btn->setDefault(true);
    btn->show();
    connect( btn, SIGNAL(clicked()), this, SLOT(accept()) );
}

//---------------------- End of file  page_w.cpp ---------------------------
