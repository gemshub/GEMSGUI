/****************************************************************************
** Form implementation generated from reading ui file 'SetFiltersDialog.ui'
**
** Created: Sun Dec 9 17:42:47 2001
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#include "SetFiltersDialogData.h"

#include <qcheckbox.h>
#include <qcombobox.h>
#include <qframe.h>
#include <qheader.h>
#include <qlabel.h>
#include <qlistview.h>
#include <qmultilineedit.h>
#include <qpushbutton.h>
#include <qtabwidget.h>
#include <qtextview.h>
#include <qwidget.h>
#include <qlayout.h>
#include <qvariant.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qimage.h>
#include <qpixmap.h>


/* 
 *  Constructs a SetFiltersDialog which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
SetFiltersDialog::SetFiltersDialog( QWidget* parent,  const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "SetFiltersDialog" );
    resize( 601, 472 ); 
    QFont f( font() );
    f.setFamily( "Lucida Sans Unicode" );
    f.setPointSize( 9 );
    setFont( f ); 
    setCaption( tr( "Set filters to create an independent system profile" ) );

    twSetFilters = new QTabWidget( this, "twSetFilters" );
    twSetFilters->setGeometry( QRect( 10, 10, 580, 401 ) ); 
    QFont twSetFilters_font(  twSetFilters->font() );
    twSetFilters->setFont( twSetFilters_font ); 
    twSetFilters->setTabShape( QTabWidget::Rounded );

    tpFiles = new QWidget( twSetFilters, "tpFiles" );

    lvDefDBfiles = new QListView( tpFiles, "lvDefDBfiles" );
    lvDefDBfiles->addColumn( tr( "Record Type / File Name (click toggles selection)" ) );
    QListViewItem * item_2 = new QListViewItem( lvDefDBfiles, 0 );
    item_2->setOpen( TRUE );
    QListViewItem * item_3 = new QListViewItem( item_2, 0 );
    item_3->setOpen( TRUE );
    QListViewItem * item = new QListViewItem( item_3, 0 );
    item->setText( 0, tr( "SDref" ) );
    item_3->setOpen( TRUE );
    QListViewItem * item_4 = new QListViewItem( item_3, item );
    item_4->setOpen( TRUE );
    item = new QListViewItem( item_4, item );
    item->setText( 0, tr( "icomp.nagra.uncertain.pdb" ) );
    item_4->setOpen( TRUE );
    item = new QListViewItem( item_4, item );
    item->setText( 0, tr( "icomp.nagra.kernel.pdb" ) );
    item_4->setText( 0, tr( "IComp" ) );
    item_3->setOpen( TRUE );
    item = new QListViewItem( item_3, item_4 );
    item->setText( 0, tr( "Compos" ) );
    item_3->setOpen( TRUE );
    item = new QListViewItem( item_3, item );
    item->setText( 0, tr( "DComp" ) );
    item_3->setOpen( TRUE );
    item = new QListViewItem( item_3, item );
    item->setText( 0, tr( "ReacDC" ) );
    item_3->setOpen( TRUE );
    item = new QListViewItem( item_3, item );
    item->setText( 0, tr( "Phase" ) );
    item_3->setText( 0, tr( "Default" ) );
    item_2->setOpen( TRUE );
    QListViewItem * item_5 = new QListViewItem( item_2, item_3 );
    item_5->setOpen( TRUE );
    item = new QListViewItem( item_5, item_3 );
    item->setText( 0, tr( "IComp" ) );
    item_5->setOpen( TRUE );
    item = new QListViewItem( item_5, item );
    item->setText( 0, tr( "Compos" ) );
    item_5->setOpen( TRUE );
    item = new QListViewItem( item_5, item );
    item->setText( 0, tr( "DComp" ) );
    item_5->setOpen( TRUE );
    item = new QListViewItem( item_5, item );
    item->setText( 0, tr( "ReacDC" ) );
    item_5->setOpen( TRUE );
    item = new QListViewItem( item_5, item );
    item->setText( 0, tr( "Phase" ) );
    item_5->setText( 0, tr( "OldProfile" ) );
    item_2->setText( 0, tr( "Database" ) );

    lvDefDBfiles->setGeometry( QRect( 10, 10, 560, 320 ) ); 
    lvDefDBfiles->setSelectionMode( QListView::Extended );
    lvDefDBfiles->setRootIsDecorated( TRUE );

    cbCopyDef = new QCheckBox( tpFiles, "cbCopyDef" );
    cbCopyDef->setGeometry( QRect( 30, 330, 240, 30 ) ); 
    cbCopyDef->setText( tr( "Copy from default database" ) );
    cbCopyDef->setChecked( TRUE );

    cbCopyPar = new QCheckBox( tpFiles, "cbCopyPar" );
    cbCopyPar->setGeometry( QRect( 290, 330, 250, 30 ) ); 
    cbCopyPar->setText( tr( "Copy from the parent profile" ) );
    twSetFilters->insertTab( tpFiles, tr( "Database Files" ) );

    tpIComp = new QWidget( twSetFilters, "tpIComp" );

    Line1 = new QFrame( tpIComp, "Line1" );
    Line1->setGeometry( QRect( 10, 40, 550, 20 ) ); 
    Line1->setFrameStyle( QFrame::HLine | QFrame::Sunken );

    Line1_2 = new QFrame( tpIComp, "Line1_2" );
    Line1_2->setGeometry( QRect( 10, 130, 550, 20 ) ); 
    Line1_2->setFrameStyle( QFrame::HLine | QFrame::Sunken );

    tlICtab = new QLabel( tpIComp, "tlICtab" );
    tlICtab->setGeometry( QRect( 10, 10, 380, 31 ) ); 
    QFont tlICtab_font(  tlICtab->font() );
    tlICtab_font.setPointSize( 10 );
    tlICtab->setFont( tlICtab_font ); 
    tlICtab->setText( tr( "  IComp (Independent Component) records" ) );

    cbICsearch = new QComboBox( FALSE, tpIComp, "cbICsearch" );
    cbICsearch->insertItem( tr( "" ) );
    cbICsearch->insertItem( tr( "   space" ) );
    cbICsearch->insertItem( tr( "_  underscore" ) );
    cbICsearch->insertItem( tr( "*? last char." ) );
    cbICsearch->insertItem( tr( "*??" ) );
    cbICsearch->insertItem( tr( "?* first char." ) );
    cbICsearch->insertItem( tr( "??*" ) );
    cbICsearch->insertItem( tr( "*  all nonspace" ) );
    cbICsearch->setGeometry( QRect( 10, 60, 140, 31 ) ); 
    cbICsearch->setEditable( TRUE );
    cbICsearch->setCurrentItem( 3 );

    cbICreplace = new QComboBox( FALSE, tpIComp, "cbICreplace" );
    cbICreplace->insertItem( tr( "" ) );
    cbICreplace->insertItem( tr( "   space" ) );
    cbICreplace->insertItem( tr( "_  underscore" ) );
    cbICreplace->insertItem( tr( "nothing" ) );
    cbICreplace->setGeometry( QRect( 10, 100, 140, 31 ) ); 
    cbICreplace->setEditable( TRUE );
    cbICreplace->setCurrentItem( 3 );

    tlICsearch = new QLabel( tpIComp, "tlICsearch" );
    tlICsearch->setGeometry( QRect( 170, 60, 380, 31 ) ); 
    tlICsearch->setText( tr( "Characters to search in the last field of record key" ) );

    tlICreplace = new QLabel( tpIComp, "tlICreplace" );
    tlICreplace->setGeometry( QRect( 170, 100, 400, 31 ) ); 
    tlICreplace->setText( tr( "Characters to replace with during copying of records" ) );

    tlIClist1 = new QLabel( tpIComp, "tlIClist1" );
    tlIClist1->setGeometry( QRect( 20, 160, 130, 80 ) ); 
    tlIClist1->setText( tr( "Lists of currently \n"
"selected IComp \n"
"symbols: in  the \n"
"Default database" ) );
    tlIClist1->setAlignment( int( QLabel::AlignVCenter | QLabel::AlignRight ) );

    tlIClist2 = new QLabel( tpIComp, "tlIClist2" );
    tlIClist2->setGeometry( QRect( 10, 270, 140, 40 ) ); 
    tlIClist2->setText( tr( "In the parent (old)\n"
"profile database" ) );
    tlIClist2->setAlignment( int( QLabel::AlignVCenter | QLabel::AlignRight ) );

    tvIClistD = new QTextView( tpIComp, "tvIClistD" );
    tvIClistD->setGeometry( QRect( 161, 150, 400, 104 ) ); 
    tvIClistD->setResizePolicy( QTextView::Manual );

    tvIClistP = new QTextView( tpIComp, "tvIClistP" );
    tvIClistP->setGeometry( QRect( 161, 250, 400, 104 ) ); 
    tvIClistP->setResizePolicy( QTextView::Manual );
    twSetFilters->insertTab( tpIComp, tr( "IComp" ) );

    tpCompos = new QWidget( twSetFilters, "tpCompos" );

    Line1_3_2 = new QFrame( tpCompos, "Line1_3_2" );
    Line1_3_2->setGeometry( QRect( 10, 150, 550, 20 ) ); 
    Line1_3_2->setFrameStyle( QFrame::HLine | QFrame::Sunken );

    tlPCOfilter = new QLabel( tpCompos, "tlPCOfilter" );
    tlPCOfilter->setGeometry( QRect( 10, 210, 100, 80 ) ); 
    tlPCOfilter->setText( tr( "Filter script \n"
"for copying \n"
"records \n"
"(optional)" ) );
    tlPCOfilter->setAlignment( int( QLabel::AlignVCenter | QLabel::AlignRight ) );

    mlePCOfscript = new QMultiLineEdit( tpCompos, "mlePCOfscript" );
    mlePCOfscript->setGeometry( QRect( 120, 210, 440, 140 ) ); 

    tlPCOtab = new QLabel( tpCompos, "tlPCOtab" );
    tlPCOtab->setGeometry( QRect( 20, 10, 440, 31 ) ); 
    QFont tlPCOtab_font(  tlPCOtab->font() );
    tlPCOtab_font.setPointSize( 10 );
    tlPCOtab->setFont( tlPCOtab_font ); 
    tlPCOtab->setText( tr( "  Compos (Predefined Composition Object) records" ) );

    Line1_3 = new QFrame( tpCompos, "Line1_3" );
    Line1_3->setGeometry( QRect( 10, 40, 550, 20 ) ); 
    Line1_3->setFrameStyle( QFrame::HLine | QFrame::Sunken );

    cbPCOsearch = new QComboBox( FALSE, tpCompos, "cbPCOsearch" );
    cbPCOsearch->insertItem( tr( "" ) );
    cbPCOsearch->insertItem( tr( "   space" ) );
    cbPCOsearch->insertItem( tr( "_  underscore" ) );
    cbPCOsearch->insertItem( tr( "*? last char." ) );
    cbPCOsearch->insertItem( tr( "*??" ) );
    cbPCOsearch->insertItem( tr( "?* first char." ) );
    cbPCOsearch->insertItem( tr( "??*" ) );
    cbPCOsearch->insertItem( tr( "*  all nonspace" ) );
    cbPCOsearch->setGeometry( QRect( 20, 70, 140, 31 ) ); 
    cbPCOsearch->setEditable( TRUE );
    cbPCOsearch->setCurrentItem( 3 );

    cbPCOreplace = new QComboBox( FALSE, tpCompos, "cbPCOreplace" );
    cbPCOreplace->insertItem( tr( "" ) );
    cbPCOreplace->insertItem( tr( "   space" ) );
    cbPCOreplace->insertItem( tr( "_  underscore" ) );
    cbPCOreplace->insertItem( tr( "nothing" ) );
    cbPCOreplace->setGeometry( QRect( 20, 110, 140, 31 ) ); 
    cbPCOreplace->setEditable( TRUE );
    cbPCOreplace->setCurrentItem( 3 );

    tlPCOsearch = new QLabel( tpCompos, "tlPCOsearch" );
    tlPCOsearch->setGeometry( QRect( 170, 70, 380, 31 ) ); 
    tlPCOsearch->setText( tr( "Characters to search in the last field of record key" ) );

    tlPCOreplace = new QLabel( tpCompos, "tlPCOreplace" );
    tlPCOreplace->setGeometry( QRect( 170, 110, 400, 31 ) ); 
    tlPCOreplace->setText( tr( "Characters to replace with during copying of records" ) );

    chbPCOcopy = new QCheckBox( tpCompos, "chbPCOcopy" );
    chbPCOcopy->setGeometry( QRect( 20, 170, 530, 31 ) ); 
    chbPCOcopy->setText( tr( "Copy only PCO records that are fully conformable to the IComp list" ) );
    twSetFilters->insertTab( tpCompos, tr( "Compos" ) );

    tpDComp = new QWidget( twSetFilters, "tpDComp" );

    Line1_3_4 = new QFrame( tpDComp, "Line1_3_4" );
    Line1_3_4->setGeometry( QRect( 10, 150, 550, 20 ) ); 
    Line1_3_4->setFrameStyle( QFrame::HLine | QFrame::Sunken );

    tlDCtab = new QLabel( tpDComp, "tlDCtab" );
    tlDCtab->setGeometry( QRect( 20, 10, 440, 31 ) ); 
    QFont tlDCtab_font(  tlDCtab->font() );
    tlDCtab_font.setPointSize( 10 );
    tlDCtab->setFont( tlDCtab_font ); 
    tlDCtab->setText( tr( "  DComp (standard thermochemical data) records" ) );

    Line1_3_3 = new QFrame( tpDComp, "Line1_3_3" );
    Line1_3_3->setGeometry( QRect( 10, 40, 550, 20 ) ); 
    Line1_3_3->setFrameStyle( QFrame::HLine | QFrame::Sunken );

    cbDCsearch = new QComboBox( FALSE, tpDComp, "cbDCsearch" );
    cbDCsearch->insertItem( tr( "" ) );
    cbDCsearch->insertItem( tr( "   space" ) );
    cbDCsearch->insertItem( tr( "_  underscore" ) );
    cbDCsearch->insertItem( tr( "*? last char." ) );
    cbDCsearch->insertItem( tr( "*??" ) );
    cbDCsearch->insertItem( tr( "?* first char." ) );
    cbDCsearch->insertItem( tr( "??*" ) );
    cbDCsearch->insertItem( tr( "*  all nonspace" ) );
    cbDCsearch->setGeometry( QRect( 20, 70, 140, 31 ) ); 
    cbDCsearch->setEditable( TRUE );
    cbDCsearch->setCurrentItem( 3 );

    cbDCreplace = new QComboBox( FALSE, tpDComp, "cbDCreplace" );
    cbDCreplace->insertItem( tr( "" ) );
    cbDCreplace->insertItem( tr( "   space" ) );
    cbDCreplace->insertItem( tr( "_  underscore" ) );
    cbDCreplace->insertItem( tr( "nothing" ) );
    cbDCreplace->setGeometry( QRect( 20, 110, 140, 31 ) ); 
    cbDCreplace->setEditable( TRUE );
    cbDCreplace->setCurrentItem( 3 );

    tlDCsearch = new QLabel( tpDComp, "tlDCsearch" );
    tlDCsearch->setGeometry( QRect( 170, 70, 380, 31 ) ); 
    tlDCsearch->setText( tr( "Characters to search in the last field of record key" ) );

    tlDCreplace = new QLabel( tpDComp, "tlDCreplace" );
    tlDCreplace->setGeometry( QRect( 170, 110, 400, 31 ) ); 
    tlDCreplace->setText( tr( "Characters to replace with during copying of records" ) );

    tlDCfilter = new QLabel( tpDComp, "tlDCfilter" );
    tlDCfilter->setGeometry( QRect( 10, 220, 100, 80 ) ); 
    tlDCfilter->setText( tr( "Filter script \n"
"for copying \n"
"records \n"
"(optional)" ) );
    tlDCfilter->setAlignment( int( QLabel::AlignVCenter | QLabel::AlignRight ) );

    mleDCfscript = new QMultiLineEdit( tpDComp, "mleDCfscript" );
    mleDCfscript->setGeometry( QRect( 120, 180, 440, 170 ) ); 
    twSetFilters->insertTab( tpDComp, tr( "DComp" ) );

    tpReacDC = new QWidget( twSetFilters, "tpReacDC" );

    Line1_3_3_2 = new QFrame( tpReacDC, "Line1_3_3_2" );
    Line1_3_3_2->setGeometry( QRect( 10, 40, 550, 20 ) ); 
    Line1_3_3_2->setFrameStyle( QFrame::HLine | QFrame::Sunken );

    Line1_3_3_3 = new QFrame( tpReacDC, "Line1_3_3_3" );
    Line1_3_3_3->setGeometry( QRect( 10, 150, 550, 20 ) ); 
    Line1_3_3_3->setFrameStyle( QFrame::HLine | QFrame::Sunken );

    tlREtab = new QLabel( tpReacDC, "tlREtab" );
    tlREtab->setGeometry( QRect( 10, 10, 440, 31 ) ); 
    QFont tlREtab_font(  tlREtab->font() );
    tlREtab_font.setPointSize( 10 );
    tlREtab->setFont( tlREtab_font ); 
    tlREtab->setText( tr( "  ReacDC (reaction-defined standard data) records" ) );

    cbREsearch = new QComboBox( FALSE, tpReacDC, "cbREsearch" );
    cbREsearch->insertItem( tr( "" ) );
    cbREsearch->insertItem( tr( "   space" ) );
    cbREsearch->insertItem( tr( "_  underscore" ) );
    cbREsearch->insertItem( tr( "*? last char." ) );
    cbREsearch->insertItem( tr( "*??" ) );
    cbREsearch->insertItem( tr( "?* first char." ) );
    cbREsearch->insertItem( tr( "??*" ) );
    cbREsearch->insertItem( tr( "*  all nonspace" ) );
    cbREsearch->setGeometry( QRect( 20, 70, 140, 31 ) ); 
    cbREsearch->setEditable( TRUE );
    cbREsearch->setCurrentItem( 3 );

    cbREreplace = new QComboBox( FALSE, tpReacDC, "cbREreplace" );
    cbREreplace->insertItem( tr( "" ) );
    cbREreplace->insertItem( tr( "   space" ) );
    cbREreplace->insertItem( tr( "_  underscore" ) );
    cbREreplace->insertItem( tr( "nothing" ) );
    cbREreplace->setGeometry( QRect( 20, 110, 140, 31 ) ); 
    cbREreplace->setEditable( TRUE );
    cbREreplace->setCurrentItem( 3 );

    tlREsearch = new QLabel( tpReacDC, "tlREsearch" );
    tlREsearch->setGeometry( QRect( 170, 70, 380, 31 ) ); 
    tlREsearch->setText( tr( "Characters to search in the last field of record key" ) );

    tlREreplace = new QLabel( tpReacDC, "tlREreplace" );
    tlREreplace->setGeometry( QRect( 170, 110, 400, 31 ) ); 
    tlREreplace->setText( tr( "Characters to replace with during copying of records" ) );

    tlREfilter = new QLabel( tpReacDC, "tlREfilter" );
    tlREfilter->setGeometry( QRect( 10, 200, 100, 80 ) ); 
    tlREfilter->setText( tr( "Filter script \n"
"for copying \n"
"records \n"
"(optional)" ) );
    tlREfilter->setAlignment( int( QLabel::AlignVCenter | QLabel::AlignRight ) );

    mleREfscript = new QMultiLineEdit( tpReacDC, "mleREfscript" );
    mleREfscript->setGeometry( QRect( 120, 180, 440, 170 ) ); 
    twSetFilters->insertTab( tpReacDC, tr( "ReacDC" ) );

    tpPhase = new QWidget( twSetFilters, "tpPhase" );

    Line1_3_5 = new QFrame( tpPhase, "Line1_3_5" );
    Line1_3_5->setGeometry( QRect( 10, 40, 550, 20 ) ); 
    Line1_3_5->setFrameStyle( QFrame::HLine | QFrame::Sunken );

    Line1_3_6 = new QFrame( tpPhase, "Line1_3_6" );
    Line1_3_6->setGeometry( QRect( 10, 150, 550, 20 ) ); 
    Line1_3_6->setFrameStyle( QFrame::HLine | QFrame::Sunken );

    tlPHtab = new QLabel( tpPhase, "tlPHtab" );
    tlPHtab->setGeometry( QRect( 10, 10, 450, 31 ) ); 
    QFont tlPHtab_font(  tlPHtab->font() );
    tlPHtab_font.setPointSize( 10 );
    tlPHtab->setFont( tlPHtab_font ); 
    tlPHtab->setText( tr( "  Phase (Definition of Thermodynamic Phase) records" ) );

    cbPHsearch = new QComboBox( FALSE, tpPhase, "cbPHsearch" );
    cbPHsearch->insertItem( tr( "" ) );
    cbPHsearch->insertItem( tr( "   space" ) );
    cbPHsearch->insertItem( tr( "_  underscore" ) );
    cbPHsearch->insertItem( tr( "*? last char." ) );
    cbPHsearch->insertItem( tr( "*??" ) );
    cbPHsearch->insertItem( tr( "?* first char." ) );
    cbPHsearch->insertItem( tr( "??*" ) );
    cbPHsearch->insertItem( tr( "*  all nonspace" ) );
    cbPHsearch->setGeometry( QRect( 20, 70, 140, 31 ) ); 
    cbPHsearch->setEditable( TRUE );
    cbPHsearch->setCurrentItem( 3 );

    cbPHreplace = new QComboBox( FALSE, tpPhase, "cbPHreplace" );
    cbPHreplace->insertItem( tr( "" ) );
    cbPHreplace->insertItem( tr( "   space" ) );
    cbPHreplace->insertItem( tr( "_  underscore" ) );
    cbPHreplace->insertItem( tr( "nothing" ) );
    cbPHreplace->setGeometry( QRect( 20, 110, 140, 31 ) ); 
    cbPHreplace->setEditable( TRUE );
    cbPHreplace->setCurrentItem( 3 );

    tlPHsearch = new QLabel( tpPhase, "tlPHsearch" );
    tlPHsearch->setGeometry( QRect( 170, 70, 380, 31 ) ); 
    tlPHsearch->setText( tr( "Characters to search in the last field of record key" ) );

    tlPHreplace = new QLabel( tpPhase, "tlPHreplace" );
    tlPHreplace->setGeometry( QRect( 170, 110, 400, 31 ) ); 
    tlPHreplace->setText( tr( "Characters to replace with during copying of records" ) );

    chbPHcopyS = new QCheckBox( tpPhase, "chbPHcopyS" );
    chbPHcopyS->setGeometry( QRect( 20, 170, 480, 31 ) ); 
    chbPHcopyS->setText( tr( "Copy Phase records for single-component phases" ) );
    chbPHcopyS->setChecked( TRUE );

    chbPHcopyF = new QCheckBox( tpPhase, "chbPHcopyF" );
    chbPHcopyF->setGeometry( QRect( 20, 200, 480, 31 ) ); 
    chbPHcopyF->setText( tr( "Copy only Phase records that retain all species after filtering" ) );

    chbPHcopyL = new QCheckBox( tpPhase, "chbPHcopyL" );
    chbPHcopyL->setGeometry( QRect( 40, 230, 520, 31 ) ); 
    chbPHcopyL->setText( tr( "Show a list of discarded Phase records with some species retained" ) );

    chbPHcopyY = new QCheckBox( tpPhase, "chbPHcopyY" );
    chbPHcopyY->setGeometry( QRect( 20, 320, 300, 31 ) ); 
    chbPHcopyY->setText( tr( "Copy definitions of sorption phases" ) );

    chbPHcopyA = new QCheckBox( tpPhase, "chbPHcopyA" );
    chbPHcopyA->setGeometry( QRect( 20, 290, 380, 31 ) ); 
    chbPHcopyA->setText( tr( "Copy definitions of aqueous electrolyte" ) );

    chbPHcopyN = new QCheckBox( tpPhase, "chbPHcopyN" );
    chbPHcopyN->setGeometry( QRect( 20, 260, 410, 31 ) ); 
    chbPHcopyN->setText( tr( "Copy definitions of non-ideal solid-solution phases" ) );
    chbPHcopyN->setChecked( TRUE );
    twSetFilters->insertTab( tpPhase, tr( "Phases" ) );

    QWidget* privateLayoutWidget = new QWidget( this, "Layout7" );
    privateLayoutWidget->setGeometry( QRect( 10, 420, 570, 40 ) ); 
    Layout7 = new QHBoxLayout( privateLayoutWidget ); 
    Layout7->setSpacing( 6 );
    Layout7->setMargin( 0 );

    pbHelp = new QPushButton( privateLayoutWidget, "pbHelp" );
    pbHelp->setText( tr( "&Help" ) );
    Layout7->addWidget( pbHelp );
    QSpacerItem* spacer = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    Layout7->addItem( spacer );

    pbCancel = new QPushButton( privateLayoutWidget, "pbCancel" );
    pbCancel->setText( tr( "&Cancel" ) );
    Layout7->addWidget( pbCancel );

    pbReset = new QPushButton( privateLayoutWidget, "pbReset" );
    pbReset->setText( tr( "&Reset" ) );
    Layout7->addWidget( pbReset );

    pbApply = new QPushButton( privateLayoutWidget, "pbApply" );
    pbApply->setText( tr( "&Apply" ) );
    pbApply->setDefault( TRUE );
    Layout7->addWidget( pbApply );

    pbClose = new QPushButton( privateLayoutWidget, "pbClose" );
    pbClose->setText( tr( "C&lose" ) );
    Layout7->addWidget( pbClose );
}

/*  
 *  Destroys the object and frees any allocated resources
 */
SetFiltersDialog::~SetFiltersDialog()
{
    // no need to delete child widgets, Qt does it all for us
}

/*  
 *  Main event handler. Reimplemented to handle application
 *  font changes
 */
bool SetFiltersDialog::event( QEvent* ev )
{
    bool ret = QDialog::event( ev ); 
    if ( ev->type() == QEvent::ApplicationFontChange ) {
	QFont twSetFilters_font(  twSetFilters->font() );
	twSetFilters->setFont( twSetFilters_font ); 
	QFont tlICtab_font(  tlICtab->font() );
	tlICtab_font.setPointSize( 10 );
	tlICtab->setFont( tlICtab_font ); 
	QFont tlPCOtab_font(  tlPCOtab->font() );
	tlPCOtab_font.setPointSize( 10 );
	tlPCOtab->setFont( tlPCOtab_font ); 
	QFont tlDCtab_font(  tlDCtab->font() );
	tlDCtab_font.setPointSize( 10 );
	tlDCtab->setFont( tlDCtab_font ); 
	QFont tlREtab_font(  tlREtab->font() );
	tlREtab_font.setPointSize( 10 );
	tlREtab->setFont( tlREtab_font ); 
	QFont tlPHtab_font(  tlPHtab->font() );
	tlPHtab_font.setPointSize( 10 );
	tlPHtab->setFont( tlPHtab_font ); 
    }
    return ret;
}

