/****************************************************************************
** Form interface generated from reading ui file 'ElementsDialog.ui'
**
** Created: Sun Dec 9 17:39:26 2001
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#ifndef ELEMENTSDIALOG_H
#define ELEMENTSDIALOG_H

#include <qvariant.h>
#include <qdialog.h>
class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QButtonGroup;
class QCheckBox;
class QLabel;
class QPushButton;
class QRadioButton;

class ElementsDialogData : public QDialog
{ 
    Q_OBJECT

public:
    ElementsDialogData( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~ElementsDialogData();

    QButtonGroup* bgOther;
    QPushButton* pbA_2;
    QPushButton* pbA_3;
    QPushButton* pbA_4;
    QPushButton* pbA_5;
    QPushButton* pbA_6;
    QPushButton* pbA_7;
    QPushButton* pbA_8;
    QPushButton* pbA_9;
    QPushButton* pbA_10;
    QPushButton* pbA_11;
    QPushButton* pbA_12;
    QPushButton* pbA_0;
    QPushButton* pbA_1;
    QButtonGroup* bgElem;
    QLabel* tlEg_2;
    QLabel* tlEg_3;
    QLabel* tlEg_5;
    QLabel* tlEg_6;
    QLabel* tlEg_7;
    QLabel* tlEp_4a;
    QLabel* tlEp_5;
    QLabel* tlEp_5a;
    QLabel* tlEp_6;
    QLabel* tlEp_6a;
    QLabel* tlEp_1;
    QLabel* tlEp_2;
    QLabel* tlEp_3;
    QLabel* tlEp_4;
    QPushButton* pbE_87;
    QPushButton* pbE_85;
    QPushButton* pbE_4;
    QPushButton* pbE_10;
    QPushButton* pbE_18;
    QPushButton* pbE_21;
    QPushButton* pbE_23;
    QPushButton* pbE_27;
    QPushButton* pbE_28;
    QPushButton* pbE_29;
    QPushButton* pbE_33;
    QPushButton* pbE_34;
    QPushButton* pbE_37;
    QPushButton* pbE_39;
    QPushButton* pbE_40;
    QPushButton* pbE_41;
    QPushButton* pbE_42;
    QPushButton* pbE_44;
    QPushButton* pbE_45;
    QPushButton* pbE_46;
    QPushButton* pbE_47;
    QPushButton* pbE_48;
    QPushButton* pbE_49;
    QPushButton* pbE_51;
    QPushButton* pbE_53;
    QPushButton* pbE_54;
    QPushButton* pbE_55;
    QPushButton* pbE_56;
    QPushButton* pbE_72;
    QPushButton* pbE_73;
    QPushButton* pbE_79;
    QPushButton* pbE_80;
    QPushButton* pbE_86;
    QPushButton* pbE_88;
    QPushButton* pbE_57;
    QPushButton* pbE_58;
    QPushButton* pbE_59;
    QPushButton* pbE_60;
    QPushButton* pbE_61;
    QPushButton* pbE_62;
    QPushButton* pbE_63;
    QPushButton* pbE_64;
    QPushButton* pbE_65;
    QPushButton* pbE_66;
    QLabel* tlEp_7;
    QLabel* tlEg_1;
    QLabel* tlEg_8;
    QPushButton* pbE_26;
    QPushButton* pbE_25;
    QPushButton* pbE_17;
    QPushButton* pbE_16;
    QPushButton* pbE_15;
    QPushButton* pbE_14;
    QPushButton* pbE_13;
    QPushButton* pbE_12;
    QPushButton* pbE_9;
    QPushButton* pbE_7;
    QPushButton* pbE_5;
    QPushButton* pbE_3;
    QPushButton* pbE_52;
    QPushButton* pbE_43;
    QPushButton* pbE_38;
    QPushButton* pbE_20;
    QPushButton* pbE_22;
    QPushButton* pbE_24;
    QPushButton* pbE_11;
    QPushButton* pbE_19;
    QPushButton* pbE_32;
    QPushButton* pbE_78;
    QPushButton* pbE_77;
    QPushButton* pbE_76;
    QPushButton* pbE_75;
    QPushButton* pbE_74;
    QPushButton* pbE_84;
    QPushButton* pbE_50;
    QPushButton* pbE_6;
    QPushButton* pbE_35;
    QPushButton* pbE_89;
    QPushButton* pbE_101;
    QPushButton* pbE_100;
    QLabel* tlEg_4;
    QPushButton* pbE_2;
    QPushButton* pbE_30;
    QPushButton* pbE_31;
    QPushButton* pbE_67;
    QPushButton* pbE_68;
    QPushButton* pbE_69;
    QPushButton* pbE_90;
    QPushButton* pbE_91;
    QPushButton* pbE_92;
    QPushButton* pbE_93;
    QPushButton* pbE_94;
    QPushButton* pbE_95;
    QPushButton* pbE_96;
    QPushButton* pbE_97;
    QPushButton* pbE_98;
    QPushButton* pbE_70;
    QPushButton* pbE_71;
    QPushButton* pbE_8;
    QPushButton* pbE_1;
    QPushButton* pbE_81;
    QPushButton* pbE_83;
    QPushButton* pbE_82;
    QPushButton* pbE_0;
    QLabel* pImg1;
    QLabel* tlGEMS;
    QCheckBox* cbIsotopes;
    QPushButton* pbE_36;
    QPushButton* bHelp;
    QRadioButton* rbUncertain;
    QRadioButton* rbKernel;
    QCheckBox* cbGaseous;
    QCheckBox* cbAqueous;
    QCheckBox* cbSorption;
    QRadioButton* rbSpecific;
    QPushButton* bReset;
    QPushButton* bNext;

protected:
    QHBoxLayout* Layout2;
    QGridLayout* Layout1;
    bool event( QEvent* );
};

#endif // ELEMENTSDIALOG_H
