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


	QMap<QString, QString> tmpMap;
    //QMap<QString, QString> globalCaptionMap;
    //QMap<QString, QString> tablaCaptionMap;

    const int C_ix_Caption = 0;
    const int C_ix_colName = 1;
    const int C_ix_colType = 2;
    const int C_ix_dlen = 3;
    const int C_ix_nullable = 4;


	QSqlDatabase db;
    //bool is_dbOK;
	QString tablanev="";

	QMap<QString, QString> dxMap;

		

    //void feltoltCaptionGlobal(void);
    //void feltoltCaptionTabla(QString);
	void saveCaptionTabla(QString);

	void feltoltTabla(void);		
	void feltoltTmpMap(void);
	void feltoltEljaras(QString tablanev);
	void feltoltIdegenkulcs(QString tablanev);
	void feltoltMezoLista(QString tablanev);
    void feltoltMezoLista(zTable t);

	QString generateTmp(QString);
	
    void tokenize(QString *tmp, QMap<QString, QVariant> *, int);
    int tokenizeR(QString *txt, int ix, int*, QMap<QString, QVariant>*, int);
    QString getToken(QString t1, QString t2, QMap<QString, QVariant> *map, int);

    //QString getAdatbazisnev(void);
    static QString getCaptionFileName(QString);
	QString getOsztalynevLower(QString tablanev);
	QString getOsztalynevUpper(QString tablanev);
	QString getContextNev(void);

	QString get_liNev(QString o);
	
	QString getPropList();
    QString getPropList2(QString tmp, QString param, int whsp);
	
    QString getAttrList(QString tmp, QMap<QString, QVariant> *map, int whsp);
	
	QString getPropType(QString tipusnev, int length, bool isnullable);
	QString getePropType(QString tipusnev, int length, bool isnullable);
	//QString getReq(QString tmp, QMap<QString, QVariant> *map);

	QString getTemplateFilename(QString tfname);
    QString getModelFilename(QString tfname, QString dirname="Models");	

	QString ReadAllTextFromFile(QString filename);
	void SaveAllTextToFile(QString *classTxt, QString fn);

	static bool toBool(QString);
    //static void zError(QString str);


    static QTableWidgetItem* CreateTableItem(QVariant v);    
};

#endif // RETEK2_H
