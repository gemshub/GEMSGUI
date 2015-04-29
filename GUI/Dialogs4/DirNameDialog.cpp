#include "DirNameDialog.h"
#include "ui_DirNameDialog.h"
#include "service.h"


DirNameDialog::DirNameDialog(const string& name_, const string& path_,
                             const char *title_, QWidget *parent) :
    QDialog(parent), title(title_),
    ui(new Ui::DirNameDialog)
{
    ui->setupUi(this);
    setWindowTitle( trUtf8(title_) );

    ui->nameEdit->setText(name_.c_str());
    ui->pathEdit->setText(path_.c_str());
    QObject::connect( ui->pathButton, SIGNAL(clicked()), this, SLOT(CmTaskDir()));
}

DirNameDialog::~DirNameDialog()
{
    delete ui;
}

void DirNameDialog::CmTaskDir()
{
   string dir = ui->pathEdit->text().toUtf8().data();

  if( vfChooseDirectory(this, dir, title.c_str() )  )
           ui->pathEdit->setText( dir.c_str() );
}

string DirNameDialog::Path()
{
  return ui->pathEdit->text().toUtf8().data();
}

string DirNameDialog::Name()
{
  return ui->nameEdit->text().toUtf8().data();
}
