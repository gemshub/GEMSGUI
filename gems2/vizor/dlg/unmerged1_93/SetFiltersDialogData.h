/****************************************************************************
** Form interface generated from reading ui file 'SetFiltersDialog.ui'
**
** Created: Wed Jan 23 15:04:25 2002
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#ifndef SETFILTERSDIALOGDATA_H
#define SETFILTERSDIALOGDATA_H

#include <qvariant.h>
#include <qdialog.h>
class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QCheckBox;
class QComboBox;
class QFrame;
class QLabel;
class QListView;
class QListViewItem;
class QMultiLineEdit;
class QPushButton;
class QTabWidget;
class QTextView;
class QWidget;

class SetFiltersDialogData : public QDialog
{ 
    Q_OBJECT

public:
    SetFiltersDialogData( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~SetFiltersDialogData();

    QPushButton* pbHelp;
    QPushButton* pbCancel;
    QPushButton* pbReset;
    QPushButton* pbApply;
    QPushButton* pbOk;
    QTabWidget* twSetFilters;
    QWidget* tpFiles;
    QCheckBox* cbCopyDef;
    QCheckBox* cbCopyPar;
    QListView* lvDefDBfiles;
    QWidget* tpIComp;
    QFrame* Line1;
    QFrame* Line1_2;
    QLabel* tlICtab;
    QComboBox* cbICsearch;
    QComboBox* cbICreplace;
    QLabel* tlICsearch;
    QLabel* tlICreplace;
    QLabel* tlIClist1;
    QLabel* tlIClist2;
    QTextView* tvIClistD;
    QTextView* tvIClistP;
    QWidget* tpCompos;
    QFrame* Line1_3_2;
    QLabel* tlPCOfilter;
    QMultiLineEdit* mlePCOfscript;
    QLabel* tlPCOtab;
    QFrame* Line1_3;
    QComboBox* cbPCOsearch;
    QComboBox* cbPCOreplace;
    QLabel* tlPCOsearch;
    QLabel* tlPCOreplace;
    QCheckBox* chbPCOcopy;
    QWidget* tpDComp;
    QFrame* Line1_3_4;
    QLabel* tlDCtab;
    QFrame* Line1_3_3;
    QComboBox* cbDCsearch;
    QComboBox* cbDCreplace;
    QLabel* tlDCsearch;
    QLabel* tlDCreplace;
    QLabel* tlDCfilter;
    QMultiLineEdit* mleDCfscript;
    QWidget* tpReacDC;
    QFrame* Line1_3_3_2;
    QFrame* Line1_3_3_3;
    QLabel* tlREtab;
    QComboBox* cbREsearch;
    QComboBox* cbREreplace;
    QLabel* tlREsearch;
    QLabel* tlREreplace;
    QLabel* tlREfilter;
    QMultiLineEdit* mleREfscript;
    QWidget* tpPhase;
    QFrame* Line1_3_5;
    QFrame* Line1_3_6;
    QLabel* tlPHtab;
    QComboBox* cbPHsearch;
    QComboBox* cbPHreplace;
    QLabel* tlPHsearch;
    QLabel* tlPHreplace;
    QCheckBox* chbPHcopyS;
    QCheckBox* chbPHcopyF;
    QCheckBox* chbPHcopyL;
    QCheckBox* chbPHcopyY;
    QCheckBox* chbPHcopyA;
    QCheckBox* chbPHcopyN;

protected:
    QHBoxLayout* Layout7;
    bool event( QEvent* );
};

#endif // SETFILTERSDIALOGDATA_H
