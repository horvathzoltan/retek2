#include "zerror.h"

#include <QMessageBox>

zError::zError()
{

}


void zError::ShowDialog(QString str) {
    QMessageBox messageBox;
    messageBox.critical(0, "Error", str);
    messageBox.setFixedSize(500, 200);
}
