//-------------------------------------------------------------------
// $Id: KeyProfile.h 968 2007-12-13 13:23:32Z gems $
//
// Declaration of KeyProfile class
//
// Copyright (C) 1996-2008  A.Rysin, S.Dmytriyeva
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

#ifndef KeyProfile_included
#define KeyProfile_included

#include <QDialog>

namespace Ui {
class KeyProfileData;
}

class KeyProfile : public QDialog
{
    Q_OBJECT

    Ui::KeyProfileData *ui;
    uint iRt;
    bool newKey;

protected slots:
    virtual void CmNew();
    virtual void CmHelp();
    void CmReturnIA();
    void CmReturnSmart();

public:

    KeyProfile(QWidget* win, uint irt, const char* caption = nullptr);
    virtual ~KeyProfile();

    std::string getKey() const;
    bool   getAqGasState() const;
    bool   getFilesState() const;
    bool   getRemakeState() const;
    int   getCalcMode() const;
    std::string getTemplateKey() const;

    int   getDumpMode() const;
    int   getGEMSExportMode() const;

};


#endif // KeyProfile_included


