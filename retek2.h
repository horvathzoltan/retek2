#ifndef RETEK2_H
#define RETEK2_H

#include <QtWidgets/QMainWindow>
#include "ui_retek2.h"
#include <QSqlQuery>
#include "ztable.h"

class retek2 : public QMainWindow
{
	Q_OBJECT

public:
	retek2(QWidget *parent = 0);
	~retek2();
	void init(void);	       

private slots:
	void GenerateAll();
    void GenerateByText();
	void TableSelect(QListWidgetItem*);
    void on_pushButton_clicked();

    void on_pushButton_3_clicked();

private:
    Ui::retek2Class ui;

	QSqlDatabase db;

    void saveCaptionTabla(QString);

	void feltoltTabla(void);		
	void feltoltTmpMap(void);
	void feltoltEljaras(QString tablanev);
	void feltoltIdegenkulcs(QString tablanev);
	void feltoltMezoLista(QString tablanev);
    void feltoltMezoLista(zTable t);

	QString generateTmp(QString);
	

    static QTableWidgetItem* CreateTableItem(QVariant v);    
};

#endif // RETEK2_H
