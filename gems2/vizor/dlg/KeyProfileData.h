/****************************************************************************
** Form interface generated from reading ui file 'KeyProfile.ui'
**
** Created: Втр Лют 5 12:33:43 2002
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#ifndef KEYPROFILEDATA_H
#define KEYPROFILEDATA_H

#include <qvariant.h>
#include <qdialog.h>
class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QCheckBox;
class QLabel;
class QListBox;
class QListBoxItem;
class QPushButton;

class KeyProfileData : public QDialog
{ 
    Q_OBJECT

public:
    KeyProfileData( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~KeyProfileData();

    QLabel* pLabel;
    QListBox* pList;
    QPushButton* pGO_OKButton;
    QPushButton* pNewProfBtn;
    QPushButton* pHelpButton;
    QPushButton* pCancelButton;
    QCheckBox* pAqGas;
    QCheckBox* pFiles;


public slots:
    virtual void CmNew();
    virtual void CmHelp();

protected:
    QHBoxLayout* Layout3;
};

#endif // KEYPROFILEDATA_H
