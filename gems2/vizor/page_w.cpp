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
#include <qvalidator.h>
#include <qclipboard.h>
#include <qscrollbar.h>

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

// temporary workaround to make empty value look better
const gstring emptiness("---");
const gstring short_emptiness("---");

inline
gstring visualizeEmpty(const gstring& text)
{
    return (text==S_EMPTY) ? emptiness : text;
}

/*
static
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
*/

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
                rInfo.aFieldInfo[ii].pField = NULL;
            }
    }
    else
        firstRedraw = false;

    AddFields();
}

void
TCPage::AddFields()
{
    TField* prevField = NULL;
    int LineH = pVisorImp->getCharHeight();

    for( int ii=0; ii<getFieldCnt(); ii++ )
    {
        FieldInfo& fi = rInfo.aFieldInfo[ii];
        if( fi.pObj->IsNull() )
        {   fi.pField = NULL;    //added Sveta 24/08/01
            continue; //return;
        }
        TField* pTied = NULL;
        TField* pSticked = NULL;

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

void
TCPage::clearSelectedObjects()
{
    QWidget* topw = topLevelWidget();
    if( !topw->inherits("QDialog") ) {
	for( int ii=0; ii<getFieldCnt(); ii++ )
    	    if( getField(ii) )
        	getField(ii)->setSelected(false);
    }
}

TField*
TCPage::getSelectedObject()
{
    if( ! topLevelWidget()->inherits("QDialog") ) {
	for( int ii=0; ii<getFieldCnt(); ii++ )
    	    if( getField(ii) && getField(ii)->isSelected() )
        	return getField(ii);
    }
    
    return NULL;
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
        selected(false),
        pTied(NULL),
        pSticked(NULL)
{
    focused = NULL;
    pSV = pSH = NULL;
    indTied = 0;

    selectN1 = 
    selectN2 = 
    selectM1 = 
    selectM2 = 0;

    p->setFont( pVisorImp->getCellFont() );

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
        //        p->setFont( pVisorImp->getCellFont() );
        p->setText(GetObj().GetKeywd());
        //    p->show();

        if( GetObj().GetN() > 1 )
        {
            p->setAlignment( AlignLeft );
            yy += pVisorImp->getCharHeight(); /// + htSPACE;
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
                    yy += pVisorImp->getCharHeight(); /// + htSPACE;
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
        int labH = (Lab && GetObj().GetN()>1) ? pVisorImp->getCharHeight()/*///+htSPACE*/ : 0;
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
    for(uint ii=0; ii<aCtrl.GetCount(); ii++) {
	const int n = (ii/minM)+iN;
	const int m = (ii%minM)+iM;
	bool cellSelected = (selected && n >= selectN1 && n < selectN2 
					&& m >= selectM1 && m < selectM2);
        aCtrl[ii]->changeCellNM(n, m, cellSelected);
    }

    if( focused )
        focused->SetDescription();
}

void
TField::getVisibleArea(int& n1, int& n2, int& m1, int& m2)
{
    if( pSV )
        n1 = pSV->value();
    else
	n1 = 0;
    n2 = n1 + (largeN ? rInfo.maxN : N) - 1;

    if( pSH )
        m1 = pSH->value();
    else
	m1 = 0;
    m2 = m1 + (largeM ? rInfo.maxM : M) - 1;
}

void
TField::scrollVertically(int shift)
{
    if( pSV ) {
	int val = pSV->value() + shift;
	if( val < pSV->minValue() ) val = pSV->minValue();
	if( val > pSV->maxValue() ) val = pSV->maxValue();
	setYPos(val);
    }
}

void
TField::scrollHorizontally(int shift)
{
    if( pSH ) {
	int val = pSH->value() + shift;
	if( val < pSH->minValue() ) val = pSH->minValue();
	if( val > pSH->maxValue() ) val = pSH->maxValue();
	setXPos(val);
    }
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
    iM = pos;
    Update();
    if( pSH )
	pSH->setValue(iM);
}

void
TField::EvVScroll(int n)
{
    if( n == iN )		// to prevent recursion of tied lists
        return;

    iN = n;
    Update();

    for(TField* t = pTied; t && t!=this; t=t->pTied)
        t->setYPos( n );
}


void
TField::EvHScroll(int m)
{
    if( m == iM )		// to prevent recursion of tied lists
        return;

    iM = m;
    Update();

    for(TField* t = pSticked; t && t!=this; t=t->pSticked)
        t->setXPos( m );
}

bool
TField::SizeChanged()
{
    return GetObj().IsDynamic() &&
           (GetObj().GetN() != N || GetObj().GetM() != M);
}

void
TField::objectChanged()
{
    QWidget* topw = topLevelWidget();
    if( topw->inherits("QWidget") && !topw->inherits("QDialog") )
        ((TCModuleImp*)(topw))->CellChanged();
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

void
TField::setFocused(TCell* cell)
{
    if( cell != focused ) {
	if( selected )
	    getPage()->clearSelectedObjects();
	focused = cell;
	if( focused )
	    focused->SetDescription();
    }
}

void
TField::setSelected(bool selected_)
{
    if( selected_ != selected ) {
	if( selected_ ) {
	    getPage()->clearSelectedObjects();
	    selectN1 = 0;
	    selectN2 = GetObj().GetN();
	    selectM1 = 0;
	    selectM2 = GetObj().GetM();
	}
	else {
	    selectN1 = 
	    selectN2 = 
	    selectM1 = 
	    selectM2 = 0;
	}

    	selected = selected_;
    	for(uint ii=0; ii<aCtrl.GetCount(); ii++)
    	    aCtrl[ii]->setGroupSelected(selected);
    }
}

void 
TField::setSelectedArea(int N1, int N2, int M1, int M2)
{
    getPage()->clearSelectedObjects();
    selected = true;
    selectN1 = N1;
    selectN2 = N2;
    selectM1 = M1;
    selectM2 = M2;

//cerr << "selectedArea " << N1 << " " << N2 << " " << M1 << " " << M2 << endl;
    for(uint ii=0; ii<aCtrl.GetCount(); ii++)
	if( aCtrl[ii]->GetN() >= selectN1 && aCtrl[ii]->GetN() < selectN2 
		&&  aCtrl[ii]->GetM() >= selectM1 && aCtrl[ii]->GetM() < selectM2 )
    	    aCtrl[ii]->setGroupSelected(selected);
}


QString
TField::createString(int N1, int N2, int M1, int M2)
{
    const TObject& object = GetObj();
    QString clipText;

//cerr << "copy " << N1 << " " << N2 << " " << M1 << " " << M2 << endl;

    for(int nn=N1; nn<N2; nn++) {
        for(int mm=M1; mm<M2; mm++) {
            //        if( aCtrl[ii]->isSelected() )
            //Sveta 16/02/2005
            gstring str_sh = visualizeEmpty(object.GetString(nn, mm));
            if( object.GetType() > 0 )
                     str_sh.strip();
            clipText += str_sh.c_str();
//            clipText += visualizeEmpty(object.GetString(nn, mm)).c_str();
            if( mm < M2 - 1 )
                clipText += "\t";
        }
        if( nn < N2 - 1 )
            clipText += "\n";
    }
    
    return clipText;
}

void
TField::copy()
{
    QString clipText = createString(selectN1, selectN2, selectM1, selectM2);
    QApplication::clipboard()->setText(clipText, QClipboard::Clipboard);
}

void
TField::setFromString(const QString& str, int N1, int N2, int M1, int M2, bool transpose) throw(TError)
{
	if( str.isEmpty() )
	    return;

	const QStringList rows = QStringList::split('\n', str, true);

	int rowNum = N1;
	const int nLimit = (transpose) ? (N1 + M2-M1) : N2;
	QStringList::const_iterator it;
	for( it = rows.begin(); it != rows.end() && rowNum < nLimit; ++it, rowNum++) {
	    if( (*it).isEmpty() )
		continue;
	
	    const QStringList cells = QStringList::split('\t', *it, true);

	    int cellNum = M1;
	    const int mLimit = (transpose) ? (M1 + N2-N1) : M2;
	    QStringList::const_iterator cellIt;
	    for( cellIt = cells.begin(); 
			cellIt != cells.end() && cellNum < mLimit; ++cellIt, cellNum++) {
		QString value(*cellIt);

		if( value.isEmpty() )
		    value = S_EMPTY;
		//cerr << "pasting row " << rowNum << " cell " << cellNum << ": '" << value << "'" << endl;

		if( transpose ) {
		if( ! GetObj().SetString( value, (cellNum-M1)+N1, (rowNum-N1)+M1 ) ) {
		    vstr err(200);
		    sprintf(err, "Invalid value for cell [%d, %d]: '%.100s'!",
				rowNum, cellNum, (const char*)value);
		    throw TError("Object paste", err.p);
		}
		}
		else {
		if( ! GetObj().SetString( value, rowNum, cellNum ) ) {
		    vstr err(200);
		    sprintf(err, "Invalid value for cell [%d, %d]: '%.100s'!",
				rowNum, cellNum, (const char*)value);
		    throw TError("Object paste", err.p);
		}
		}
	    }
	}
}

void
TField::clear()
{
  // Sveta oct 2005
 TObject& object = GetObj();
 for(int nn=selectN1; nn<selectN2; nn++)
            for(int mm=selectM1; mm<selectM2; mm++ )
               object.SetString(S_EMPTY, nn, mm);
 objectChanged();
 Update();
/*
    for(uint ii=0; ii<aCtrl.GetCount(); ii++)
	if( aCtrl[ii]->GetN() >= selectN1 && aCtrl[ii]->GetN() < selectN2
		&&  aCtrl[ii]->GetM() >= selectM1 && aCtrl[ii]->GetM() < selectM2 )
    	    ((TCellInput*)aCtrl[ii])->clear();
*/
}

void
TField::cut()
{
    copy();
    clear();
}

void
TField::paste()
{
    pasteIntoArea(selectN1, selectN2, selectM1, selectM2, false);
}

void
TField::pasteTransposed()
{
    pasteIntoArea(selectN1, selectN2, selectM1, selectM2, true);
}

void 
TField::pasteIntoArea(int N1, int N2, int M1, int M2, bool transpose)
{
    QString clipboard = QApplication::clipboard()->text(QClipboard::Clipboard);

//cerr << "pasteInto " << N1 << " " << N2 << " " << M1 << " " << M2 << " transpose: " << transpose << endl;
    
    uint lastCR = clipboard.findRev('\n');
    if( lastCR == clipboard.length() - 1 )
	clipboard.remove(lastCR, 1);
    
    const int clipN = clipboard.contains('\n') + 1;
    QString undoString;

    try {
	const bool largerN = transpose ? (clipN > (M2 - M1)) : (clipN > (N2 - N1));
    
	const QStringList rows = QStringList::split('\n', clipboard, true);
	int rowNum = N1;
	
	bool largerM = false;
	for(QStringList::const_iterator it = rows.begin(); it != rows.end(); ++it, rowNum++) {
	    int clipM = (*it).contains('\t') + 1;
	    largerM = transpose ? (clipM > (N2 - N1)) : (clipM > (M2 - M1));
	    if( largerM )
		break;
	    
///	    const QStringList cells = QStringList::split('\t', *it, true);
///	    for(QStringList::const_iterator cellIt = cells.begin(); cellIt != cells.end(); ++cellIt) {
		///checkCellValue();
///	    }
	}
    
	if( largerN || largerM ) {
	    if( vfQuestion3(topLevelWidget(), "Object paste", 
		    "Pasting contents has larger dimensions then the object!", 
		    "Discard extra cells", "Cancel", "") != VF3_1 )
		return;
	}
    
	undoString = createString(N1, N2, M1, M2);
    
	setFromString(clipboard, N1, N2, M1, M2, transpose);
	
	objectChanged();
	Update();
	
	groupUndoContents = undoString;	// for possible group undo
    }
    catch(TError& ex) {
	vfMessage(topLevelWidget(), "Object paste error", ex.mess , vfErr);
	if( !undoString.isEmpty() )
	    try { setFromString(undoString, N1, N2, M1, M2, false); } catch(...) {}
	    
    }
}

void
TField::selectionChanged()
{
    //    setSelected(false);
    getPage()->clearSelectedObjects();
}

void 
TField::getSelectedArea(int& N1, int& N2, int& M1, int& M2) const
{
    N1 = selectN1;
    N2 = selectN2;
    M1 = selectM1;
    M2 = selectM2;
}

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
	case Key_C:
	    copy();
	    return;
	break;
	case Key_A:
	    CmSelectAll();
	    return;
	break;
	case Key_R:
	    CmSelectRow();
	    return;
	break;
	case Key_L:
	    CmSelectColumn();
	    return;
	break;
	case Key_T:
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
	case Key_Up:
	    if( n1 > 0 ) {
		field()->setSelectedArea(n1-1, n2, m1, m2);
		if( n1-1 < vizN1 )
		    field()->scrollVertically(-1);
	    }
	    return;
	break;
	case Key_Down:
	    if( n2 < rObj.GetN() ) {
		field()->setSelectedArea(n1, n2+1, m1, m2);
		if( n2 > vizN2 )
		    field()->scrollVertically(1);
	    }
	    return;
	break;
	case Key_Right:
	    if( m2 < rObj.GetM() ) {
		field()->setSelectedArea(n1, n2, m1, m2+1);
		if( m2 > vizM2 )
		    field()->scrollHorizontally(1);
	    }
	    return;
	break;
	case Key_Left:
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
    case Key_Enter:
    case Key_Return:
        if( isModified() )
            setValue();
        return;
    case Key_F1:
        CmHelp();
        return;
    case Key_F2:
        CmSDRef();
        return;
        //    case Key_F3:
        //        CmScript();
        //        return;
    case Key_F7:
        CmDComp();
        return;
    case Key_F8:
        CmCalc();
        return;
    }

    QLineEdit::keyPressEvent(e);
}

/*! mouse press event
    handles context menu
*/
QPopupMenu*
TCellInput::createPopupMenu()
{
    QPopupMenu* menu = new QPopupMenu();
    menu->insertItem( "&Help\tF1", this, SLOT(CmHelp()), Key_F1 );
    if( fieldType == ftRef )
        menu->insertItem( "&SDRef-Record\tF2", this, SLOT(CmSDRef()), Key_F2 );
    //    if( fieldType == ftText )
    //        menu->insertItem( "&SDRef-Script\tF3", this, SLOT(CmScript()), Key_F3 );
    if( fieldType == ftRecord )
        menu->insertItem( "&Show record\tF7", this, SLOT(CmDComp()), Key_F7 );
    if( fieldType == ftFloat || fieldType == ftNumeric )
    {
        menu->insertSeparator();
        menu->setItemEnabled(
            menu->insertItem( "&Calculator\tF8", this,
                              SLOT(CmCalc()), Key_F8 ), edit );
    }

    menu->insertSeparator();

    const bool clipboardEmpty = QApplication::clipboard()->text(QClipboard::Clipboard).isEmpty();
    if( field()->isSelected() ) {
//        menu->insertItem( "&Unselect object\tF12", this, SLOT(CmSelectObject()), Key_F12 );

        QPopupMenu* objectMenu = new QPopupMenu();
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
	QPopupMenu* editMenu = QLineEdit::createPopupMenu();
	if( rObj.GetN() > 1 || rObj.GetM() > 1 ) {
    	    menu->insertItem( "&Select object\tCtrl+A", this, SLOT(CmSelectObject()), ALT + Key_S );
    	    menu->setItemEnabled(
		menu->insertItem( "Select co&lumn\tCtrl+L", this, SLOT(CmSelectColumn()), ALT + Key_L ),
		    rObj.GetN() > 1 );
    	    menu->setItemEnabled(
    		menu->insertItem( "Select &row\tCtrl+R", this, SLOT(CmSelectRow()), ALT + Key_R ),
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


QPopupMenu*
TCellCheck::createPopupMenu()
{
    QPopupMenu* menu = new QPopupMenu();
    menu->insertItem("&Help\tF1", this, SLOT(CmHelp()),
                     Key_F1, Vals.length());
    menu->insertSeparator();
    menu->setItemEnabled(
        menu->insertItem("&Calculator\tF8", this, SLOT(CmCalc()),
                         Key_F8, Vals.length()+1 ), edit );

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
        QTextEdit(&rfield),
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
    setTextFormat(QTextEdit::PlainText);

    updateCellBackground();
    setText( visualizeEmpty(rObj.GetString(N,M)).c_str() );

    QToolTip::add(this, (rObj.GetDescription(N,M)).c_str() );
}


void
TCellText::closeEvent(QCloseEvent* e)
{
    if( isModified() )
        setValue();

    QTextEdit::closeEvent(e);
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
    QTextEdit::focusInEvent(e);

    setModified(false);
    field()->setFocused(this);
}

void
TCellText::focusOutEvent(QFocusEvent* e)
{
    if( isModified() )
        setValue();
    field()->setFocused(NULL);

    QTextEdit::focusOutEvent(e);
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
    QTextEdit::keyPressEvent(e);

//    if( text().length() >= AllowedPos(rObj)+1 
//	    ||  text().section(QRegExp(GetAllowedRegExp(rObj)), 0) != text() )
//	undo();
}

/*! mouse press event
    handles context menu
*/
QPopupMenu*
TCellText::createPopupMenu(const QPoint& pos)
{
    QPopupMenu* menu = new QPopupMenu();
    menu->insertItem( "&Help\tF1", this, SLOT(CmHelp()), Key_F1 );
    menu->insertSeparator();
    menu->insertItem( "&Edit", QTextEdit::createPopupMenu(pos) );
    return menu;
}


//==========================================
// TQueryWindow
//==========================================


TQueryWindow::TQueryWindow(CWinInfo& w):
        QDialog(w.pWin, 0, true),
        rInfo(w)
{
    gstring s = w.rM.GetName();
    s += ": Remake dialog (press Ok to use default parameters)";
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
    TField* prevField = NULL;	// previous cell
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

        TField* pSticked = NULL;
        TField* pTied = NULL;

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

    if( mwidth < 200 )  // Bugfix gems 19.07.02
        mwidth = 200;

    setFixedSize(mwidth+10, mheight+15+40);

    hint = new QLabel(this);
    hint->setGeometry(3, mheight+4, mwidth, 12);
    hint->setText("Select fields, look at tooltips!");
    hint->show();

    QPushButton* btn = new QPushButton(this);
    btn->setGeometry(3, mheight+21, mwidth, 30);
    btn->setText("&Ok");
    btn->setDefault(true);
    btn->show();

    connect( btn, SIGNAL(clicked()), this, SLOT(accept()) );
}

//---------------------- End of file  page_w.cpp ---------------------------
