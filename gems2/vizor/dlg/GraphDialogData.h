/****************************************************************************
** Form interface generated from reading ui file 'GraphDialog.ui'
**
** Created: Fri Nov 16 14:53:57 2001
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#ifndef GRAPHDIALOGDATA_H
#define GRAPHDIALOGDATA_H

#include <qvariant.h>
#include <qdialog.h>
class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QGroupBox;
class QPushButton;

class GraphDialogData : public QDialog
{ 
    Q_OBJECT

public:
    GraphDialogData( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~GraphDialogData();

    QGroupBox* pGroupBox;
    QPushButton* pFragmentBtn;
    QPushButton* pBtn3;
    QPushButton* pBtn4;
    QPushButton* pBtn1;
    QGroupBox* pGrpLegend;


protected slots:
    virtual void CmFragment();
    virtual void CmLegend();
    virtual void CmPrint();

protected:
};

#endif // GRAPHDIALOGDATA_H
