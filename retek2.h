#ifndef RETEK2_H
#define RETEK2_H


#include <QtWidgets/QMainWindow>
#include "highlighter.h"
#include "ui_retek2.h"
#include "ui_ztablenames.h"
#include <QSqlQuery>
#include "ztable.h"
#include "zenumizer.h"
#include "dbconnection.h"
#include "srcconnection.h"
#include "docconnection.h"
#include "zsql.h"
#include "zlog.h"

#endif // RETEK2_H
class retek2 : public QMainWindow
{
	Q_OBJECT

public:
    static Highlighter* h1;
    static Highlighter* h2;

    //static const QMap<zTablerow::ErrCode, int> ErrCodeColIxes;

   // explicit
    retek2(QWidget *parent = nullptr);
    ~retek2() final;

    void init();

    void tablaAdatokBejegyez(const QString& tn);
    void add_zTablaToListWidget(QList<zTable> t);
    void add_zTablaToListWidget(const zTable& t);
    void loadCurrentProject();
    
    QStringList listWidgetItemsText(QList<QListWidgetItem *> items);

    void fillListWidgetByCurrentProject();
    QMap<QString,bool> validateCurrentProject();
    QMap<QString, bool> validateCurrentProject_SQL();
    QMap<QString, bool> validateCurrentProject_Source();
    QMap<QString, bool> validateCurrentProject_Document();
    void setListWidgetIconsByCurrentProject(const QMap<QString, bool>&sqlmap, const QMap<QString, bool>&srcmap, const QMap<QString, bool> &docmap, const QMap<QString, bool> &valmap);
    QStringList getIconsByFlags(const QString &name, const QMap<QString, bool> &sqlmap, const QMap<QString, bool> &srcmap, const QMap<QString, bool> &docmap);
    static QColor getLogColor(const QString &msg);
    void validateTable(zTable &t, QMap<QString,bool>&);
    void validateTableSQL(zTable &t, QMap<QString,bool>&);
    void validateTableDocument(zTable& t, QMap<QString,bool>& e);
    void validateTableSource(zTable& t, QMap<QString,bool>& e);
    void setZTablesItem(const zTable &t, const QMap<QString, bool>& sqlmap, const QMap<QString, bool> &srcmap, const QMap<QString, bool>& docmap, const QMap<QString, bool>& valmap, bool isIcons = true);



private slots:
	void GenerateAll();

    void on_pushButton_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();   

    void on_pushButton_2_clicked();

    void on_pushButton_5_clicked();

    //void on_pushButton_6_clicked();

    void on_tableWidget_MezoLista_cellChanged(int row, int column);

    void on_listWidget_ztables_itemClicked(QListWidgetItem *item);

    void on_pushButton_projects_apply_clicked();

    void on_comboBox_connections_currentIndexChanged(const QString &arg1);

    void on_listWidget_schemas_currentTextChanged(const QString &currentText);

    void on_pushButton_table_import_clicked();

    void on_pushButton_GenerateAll_clicked();

    void on_pushButton_getCaption_clicked();

    void on_listWidget_tables_itemClicked(QListWidgetItem *item);

    void on_pushButton_table_connect_clicked();

    void on_comboBox_srcconn_currentIndexChanged(const QString &arg1);

    void on_pushButton_srcimport_clicked();

    void on_pushButton_sourcepath_clicked();

    //void on_comboBox_srcconn_currentIndexChanged(int index);

    void on_listWidget_sources_itemClicked(QListWidgetItem *item);

    void on_comboBox_docconn_currentIndexChanged(const QString &arg1);

    void on_listWidget_docs_itemClicked(QListWidgetItem *item);

    void on_pushButton_docimport_clicked();

    void on_pushButton_validate_clicked();

    //void on_listWidget_ztables_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);

    void on_lineEdit_pkname_editingFinished();

    void on_pushButton_setPK_clicked();

    void on_lineEdit_name_editingFinished();

private:
    Q_DISABLE_COPY(retek2);


    Ui::retek2Class ui;
    Ui::Dialog_ztable_name zTableNameDialog;

	QSqlDatabase db;

    void saveTablaToXML(const QString&);

    void fieldsFeltolt(const dbConnection& c, const QString& schemaName, const QString& tableName);
    void tablesFeltolt(const dbConnection& c, const QString&);
    void feltoltTmpMap();
    //void feltoltEljaras(QString tablanev);

    //void feltoltIdegenkulcs(QString tablanev);
    //void mezoListaFeltolt(QString tablanev);

    void zTableNamesToUi(const zTable& t);
    void zTableNamesFromUi(zTable& t);

    void mezoListaFeltolt(const zTable &t);
    void feltoltKulcsLista(zTable t);
    //void feltoltFk(zTable t);

	QString generateTmp(const QString&);
	

    static QTableWidgetItem* CreateTableItem(const QVariant& v);    

    zEnumizer::EnumSource GetEnumData(zSQL *zsql);

    void closeEvent(QCloseEvent *event) override;

    void schemasFeltolt(const dbConnection& c);
    void sourcesFeltolt(const srcConnection& c);
    QString docsFeltolt(const docConnection& c);

    QString getCaptionByRowIx(int idix);
    void setCaptionByRowIx(int idix, const QString& caption);

    static void logToGUI(ErrLevels errlevel, const QString &msg, const QString &loci, const QString &st, void *ptr);

    static const QMap<QString, int> ColNameIxes;

    static QPalette getPaletteByColor(const QPalette& r, const QColor& c);
    static bool TextBrowserSearch(QTextBrowser *tb, const QString& a);
    QString docRefresh(docConnection *c);

    void setEnabled(bool e);
    QListWidgetItem* getTablesItem(const QString &tn);
    bool setTableName(const QString &n);
};


//#endif // RETEK2_H
