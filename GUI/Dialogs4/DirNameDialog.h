#ifndef DIRNAMEDIALOG_H
#define DIRNAMEDIALOG_H

#include <QDialog>
#include <string>
using namespace std;

namespace Ui {
class DirNameDialog;
}

class DirNameDialog : public QDialog
{
    Q_OBJECT

    string title;

public slots:
    void CmTaskDir();

public:
    explicit DirNameDialog( const string& name_, const string& path_,
                            const char *title_, QWidget *parent = 0);
    ~DirNameDialog();

    string Path();
    string Name();


private:
    Ui::DirNameDialog *ui;
};

#endif // DIRNAMEDIALOG_H
