//-------------------------------------------------------------------
// $Id: ProgressDialog.h 968 2007-12-13 13:23:32Z gems $
//
// Declaration of ProgressDialog class
//
// Copyright (C) 1996-2009  A.Rysin, S.Dmytriyeva
//
// This file is part of the GEM-Selektor GUI library which uses the
// Qt v.4 cross-platform App & UI framework (https://qt.io/download-open-source)
// under LGPL v.2.1 (http://www.gnu.org/licenses/lgpl-2.1.html)
//
// This file may be distributed under the GPL v.3 license

//
// See http://gems.web.psi.ch/ for more information
// E-mail gems2.support@psi.ch
//-------------------------------------------------------------------

#ifndef ProgressDialog_included
#define ProgressDialog_included

#include <QPaintEvent>
#include <QCloseEvent>
#include <QDialog>
#include <ctime>

namespace Ui {
class ProgressDialogData;
}

class CalcThread;
class QTimer;

/// \class IPNCalcObject object to run IPM calc in other thread
class IPNCalcObject: public QObject
{
    Q_OBJECT

signals:
    /// Finished process
    void IPM_finish();
    /// Finished with exception
    void IPM_exception(std::string err_mess);
    /// Finished OK
    void IPM_OK();

public slots:
    /// Start IPM calc
    void IPM_run();

public:
    explicit IPNCalcObject()  {}
    ~IPNCalcObject() {}
};

class ProgressDialog : public QDialog
{
    Q_OBJECT

    Ui::ProgressDialogData *ui;
    int ht_g;
    int ht_a;
    int ht_s;
    int ht_l;

#ifndef USE_GEMS3K_SERVER
    time_t last_update;
    CalcThread* calcThread=nullptr;
    QTimer* timer=nullptr;
    clock_t t_start;
#endif

    void switchToAccept(bool isAccept, bool stepwise);

public slots:
    virtual void CmClose();
    void CalcFinished();

protected slots:

    virtual void CmAccept();
#ifndef USE_GEMS3K_SERVER
    virtual void CmStep();
    virtual void CmStop();
    virtual void CmResume();
    virtual void Run();
#endif

protected:
    void closeEvent(QCloseEvent* ev);
    void paintEvent(QPaintEvent* ev);

public:
    static ProgressDialog* pDia;

    ProgressDialog(QWidget* parent);
    virtual ~ProgressDialog();

    void Update(bool force=false);
    void startThread(bool step);
};

#endif // ProgressDialog_included
