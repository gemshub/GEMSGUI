/****************************************************************************
** Form implementation generated from reading ui file 'ColorDialog.ui'
**
** Created: Do 28. Feb 13:33:34 2002
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#include "ColorDialogData.h"

#include <qvariant.h>
#include <qbuttongroup.h>
#include <qframe.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qspinbox.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qimage.h>
#include <qpixmap.h>

static const char* const image0_data[] = { 
"18 18 2 1",
"# c #800080",
". c #c3c3c3",
"..................",
"..................",
"..................",
"........##........",
"...##...##...##...",
"....##..##..##....",
".....##.##.##.....",
"......######......",
".......####.......",
"..##############..",
".......####.......",
"......######......",
".....##.##.##.....",
"....##..##..##....",
"...##...##...##...",
"........##........",
"..................",
".................."};

static const char* const image1_data[] = { 
"18 18 2 1",
"# c #800080",
". c #c3c3c3",
"..................",
"..................",
"..................",
"..##..........##..",
"...##........##...",
"....##......##....",
".....##....##.....",
"......##..##......",
".......####.......",
"........##........",
".......####.......",
"......##..##......",
".....##....##.....",
"....##......##....",
"...##........##...",
"..##..........##..",
"..................",
".................."};

static const char* const image2_data[] = { 
"18 18 2 1",
"# c #800080",
". c #c3c3c3",
"..................",
"..................",
"..................",
"......######......",
".....##....##.....",
"....##......##....",
"...##........##...",
"..##..........##..",
"..##..........##..",
"..##..........##..",
"..##..........##..",
"..##..........##..",
"...##........##...",
"....##......##....",
".....##....##.....",
"......######......",
"..................",
".................."};

static const char* const image3_data[] = { 
"18 18 2 1",
"# c #800080",
". c #c3c3c3",
"..................",
"..................",
"........#.........",
".......###........",
"......##.##.......",
".....##...##......",
"....##.....##.....",
"...##.......##....",
"..##.........##...",
"..##.........##...",
"...##.......##....",
"....##.....##.....",
".....##...##......",
"......##.##.......",
".......###........",
"........#.........",
"..................",
".................."};

static const char* const image4_data[] = { 
"18 18 2 1",
"# c #800080",
". c #c3c3c3",
"..................",
"..................",
"...#############..",
"...#############..",
"...##.........##..",
"...##.........##..",
"...##.........##..",
"...##.........##..",
"...##.........##..",
"...##.........##..",
"...##.........##..",
"...##.........##..",
"...##.........##..",
"...#############..",
"...#############..",
"..................",
"..................",
".................."};

static const char* const image5_data[] = { 
"18 18 2 1",
"# c #800080",
". c #c3c3c3",
"..................",
"..................",
"..................",
"......######......",
".....########.....",
"....##########....",
"...############...",
"..##############..",
"..##############..",
"..##############..",
"..##############..",
"..##############..",
"...############...",
"....##########....",
".....########.....",
"......######......",
"..................",
".................."};

static const char* const image6_data[] = { 
"18 18 2 1",
"# c #800080",
". c #c3c3c3",
"..................",
"..................",
"...#############..",
"...#############..",
"...#############..",
"...#############..",
"...#############..",
"...#############..",
"...#############..",
"...#############..",
"...#############..",
"...#############..",
"...#############..",
"...#############..",
"...#############..",
"..................",
"..................",
".................."};

static const char* const image7_data[] = { 
"18 18 2 1",
"# c #800080",
". c #c3c3c3",
"..................",
"..................",
"..................",
"..##........##....",
"...##......##.....",
"....##....##......",
".....##..##.......",
"......####........",
".......##.........",
".......##.........",
".......##.........",
".......##.........",
".......##.........",
".......##.........",
".......##.........",
".......##.........",
"..................",
".................."};

static const char* const image8_data[] = { 
"18 18 2 1",
"# c #800080",
". c #c3c3c3",
"..................",
"..................",
"..................",
"..................",
"..................",
"..................",
".......###........",
"......#####.......",
"......#####.......",
"......#####.......",
".......###........",
"..................",
"..................",
"..................",
"..................",
"..................",
"..................",
".................."};

static const char* const image9_data[] = { 
"18 18 2 1",
"# c #800080",
". c #c3c3c3",
"..................",
"..................",
"........#.........",
".......###........",
"......#####.......",
".....#######......",
"....#########.....",
"...###########....",
"..#############...",
"..#############...",
"...###########....",
"....#########.....",
".....#######......",
"......#####.......",
".......###........",
"........#.........",
"..................",
".................."};


/* 
 *  Constructs a ColorDialogData which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
ColorDialogData::ColorDialogData( QWidget* parent,  const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    QPixmap image0( ( const char** ) image0_data );
    QPixmap image1( ( const char** ) image1_data );
    QPixmap image2( ( const char** ) image2_data );
    QPixmap image3( ( const char** ) image3_data );
    QPixmap image4( ( const char** ) image4_data );
    QPixmap image5( ( const char** ) image5_data );
    QPixmap image6( ( const char** ) image6_data );
    QPixmap image7( ( const char** ) image7_data );
    QPixmap image8( ( const char** ) image8_data );
    QPixmap image9( ( const char** ) image9_data );
    if ( !name )
	setName( "ColorDialogData" );
    resize( 484, 207 ); 
    QFont f( font() );
    f.setFamily( "Lucida Sans Unicode" );
    f.setPointSize( 8 );
    setFont( f ); 
    setCaption( trUtf8( "ColorDialogData" ) );

    pGroup = new QButtonGroup( this, "pGroup" );
    pGroup->setGeometry( QRect( 10, 10, 340, 180 ) ); 
    pGroup->setTitle( trUtf8( "Symbol Type" ) );
    pGroup->setExclusive( TRUE );
    pGroup->setRadioButtonExclusive( TRUE );

    p6 = new QRadioButton( pGroup, "p6" );
    p6->setGeometry( QRect( 133, 99, 54, 22 ) ); 
    p6->setText( trUtf8( "" ) );
    p6->setPixmap( image0 );

    p1 = new QRadioButton( pGroup, "p1" );
    p1->setGeometry( QRect( 72, 71, 55, 22 ) ); 
    p1->setText( trUtf8( "" ) );
    p1->setPixmap( image1 );

    p7 = new QRadioButton( pGroup, "p7" );
    p7->setGeometry( QRect( 193, 99, 55, 22 ) ); 
    p7->setText( trUtf8( "" ) );
    p7->setPixmap( image2 );

    p4 = new QRadioButton( pGroup, "p4" );
    p4->setGeometry( QRect( 11, 99, 55, 22 ) ); 
    p4->setText( trUtf8( "" ) );
    p4->setPixmap( image3 );

    p2 = new QRadioButton( pGroup, "p2" );
    p2->setGeometry( QRect( 193, 71, 55, 22 ) ); 
    p2->setText( trUtf8( "" ) );
    p2->setPixmap( image4 );

    p9 = new QRadioButton( pGroup, "p9" );
    p9->setGeometry( QRect( 254, 99, 55, 22 ) ); 
    p9->setText( trUtf8( "" ) );
    p9->setPixmap( image5 );

    p3 = new QRadioButton( pGroup, "p3" );
    p3->setGeometry( QRect( 254, 71, 55, 22 ) ); 
    p3->setText( trUtf8( "" ) );
    p3->setPixmap( image6 );

    p5 = new QRadioButton( pGroup, "p5" );
    p5->setGeometry( QRect( 133, 71, 54, 22 ) ); 
    p5->setText( trUtf8( "" ) );
    p5->setPixmap( image7 );

    p0 = new QRadioButton( pGroup, "p0" );
    p0->setGeometry( QRect( 11, 71, 55, 22 ) ); 
    p0->setText( trUtf8( "" ) );
    p0->setPixmap( image8 );
    p0->setChecked( TRUE );

    p8 = new QRadioButton( pGroup, "p8" );
    p8->setGeometry( QRect( 72, 99, 55, 22 ) ); 
    p8->setText( trUtf8( "" ) );
    p8->setPixmap( image9 );

    QWidget* privateLayoutWidget = new QWidget( pGroup, "hbox1" );
    privateLayoutWidget->setGeometry( QRect( 10, 30, 300, 25 ) ); 
    hbox1 = new QHBoxLayout( privateLayoutWidget, 0, 15, "hbox1"); 

    TextLabel1 = new QLabel( privateLayoutWidget, "TextLabel1" );
    TextLabel1->setText( trUtf8( "Line Size" ) );
    hbox1->addWidget( TextLabel1 );

    pLine = new QSpinBox( privateLayoutWidget, "pLine" );
    hbox1->addWidget( pLine );

    TextLabel2 = new QLabel( privateLayoutWidget, "TextLabel2" );
    TextLabel2->setText( trUtf8( "Symbol Size" ) );
    hbox1->addWidget( TextLabel2 );

    pSize = new QSpinBox( privateLayoutWidget, "pSize" );
    hbox1->addWidget( pSize );

    QWidget* privateLayoutWidget_2 = new QWidget( pGroup, "Layout6" );
    privateLayoutWidget_2->setGeometry( QRect( 11, 140, 300, 30 ) ); 
    Layout6 = new QHBoxLayout( privateLayoutWidget_2, 0, 6, "Layout6"); 

    pColor = new QFrame( privateLayoutWidget_2, "pColor" );
    pColor->setFrameShape( QFrame::StyledPanel );
    pColor->setFrameShadow( QFrame::Raised );
    Layout6->addWidget( pColor );

    PushButton12 = new QPushButton( privateLayoutWidget_2, "PushButton12" );
    PushButton12->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)5, 0, 0, PushButton12->sizePolicy().hasHeightForWidth() ) );
    PushButton12->setText( trUtf8( "C&hange Color..." ) );
    Layout6->addWidget( PushButton12 );

    QWidget* privateLayoutWidget_3 = new QWidget( this, "Layout7" );
    privateLayoutWidget_3->setGeometry( QRect( 361, 17, 110, 60 ) ); 
    Layout7 = new QVBoxLayout( privateLayoutWidget_3, 0, 6, "Layout7"); 

    PushButton10 = new QPushButton( privateLayoutWidget_3, "PushButton10" );
    PushButton10->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)5, 0, 0, PushButton10->sizePolicy().hasHeightForWidth() ) );
    PushButton10->setText( trUtf8( "&Ok" ) );
    PushButton10->setDefault( TRUE );
    Layout7->addWidget( PushButton10 );

    PushButton11 = new QPushButton( privateLayoutWidget_3, "PushButton11" );
    PushButton11->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)5, 0, 0, PushButton11->sizePolicy().hasHeightForWidth() ) );
    PushButton11->setText( trUtf8( "&Cancel" ) );
    Layout7->addWidget( PushButton11 );

    // signals and slots connections
    connect( PushButton10, SIGNAL( clicked() ), this, SLOT( accept() ) );
    connect( PushButton11, SIGNAL( clicked() ), this, SLOT( reject() ) );
    connect( PushButton12, SIGNAL( clicked() ), this, SLOT( CmSelectColor() ) );
}

/*  
 *  Destroys the object and frees any allocated resources
 */
ColorDialogData::~ColorDialogData()
{
    // no need to delete child widgets, Qt does it all for us
}

void ColorDialogData::CmSelectColor()
{
    qWarning( "ColorDialogData::CmSelectColor(): Not implemented yet!" );
}

