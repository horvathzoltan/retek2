#ifndef RETEK2_H
#define RETEK2_H


#include <QtWidgets/QMainWindow>
#include "ui_retek2.h"
#include <QSqlQuery>
#include "ztable.h"
#include "zenumizer.h"
#include "dbconnection.h"
#include "zsql.h"

#endif // RETEK2_H
class retek2 : public QMainWindow
{
	Q_OBJECT

public:
    explicit retek2(QWidget *parent = nullptr);
    ~retek2() final;
    void init();

    void tablaAdatokBejegyez(const QString& tn);
    void zTablaToList(QList<zTable> t);
    void zTablaToList(const zTable& t);
    void loadCurrentProject();
    
private slots:
	void GenerateAll();
    //void ValidateByText();
	void TableSelect(QListWidgetItem*);
    void on_pushButton_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();   

    void on_pushButton_2_clicked();

    // void on_comboBox_connections_currentIndexChanged(int index);

    void on_lineEdit_classname_plural_editingFinished();

    void on_lineEdit_classname_editingFinished();

    void on_lineEdit_tablename_editingFinished();

    void on_pushButton_5_clicked();

    void on_pushButton_6_clicked();

    void on_tableWidget_MezoLista_cellChanged(int row, int column);

    void on_listWidget_ztables_itemClicked(QListWidgetItem *item);


    void on_pushButton_projects_apply_clicked();

    void on_comboBox_connections_currentIndexChanged(const QString &arg1);

    void on_listWidget_schemas_currentTextChanged(const QString &currentText);

private:
    Ui::retek2Class ui;

	QSqlDatabase db;

    void saveTablaToXML(const QString&);

    void tablesFeltolt(const dbConnection& c);
    void feltoltTmpMap();
    //void feltoltEljaras(QString tablanev);

    //void feltoltIdegenkulcs(QString tablanev);
    //void mezoListaFeltolt(QString tablanev);

    void fejadatFeltolt(const zTable& t);
    void mezoListaFeltolt(zTable t);
    void feltoltKulcsLista(zTable t);
    //void feltoltFk(zTable t);

	QString generateTmp(const QString&);
	

    static QTableWidgetItem* CreateTableItem(const QVariant& v);    

    zEnumizer::EnumSource GetEnumData(zSQL *zsql);

    // void initBy(dbConnection* b);
    void closeEvent(QCloseEvent *event) override;

    void schemasFeltolt(const dbConnection& c);
};

//#endif // RETEK2_H
