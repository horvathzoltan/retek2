#ifndef BEALLITASOK_H
#define BEALLITASOK_H


#pragma once

#include "dbconnection.h"
#include "srcconnection.h"
#include "docconnection.h"

#include <QLineEdit>
#include <QList>
#include <QString>

#include <QComboBox>
#include <QListWidget>




class Beallitasok
{
private:
    QLineEdit *widget_user;
    QLineEdit *widget_password;
    QLineEdit *widget_server;
    //QLineEdit *widget_adatbazisNev;
    QComboBox *widget_connections;
    QComboBox *widget_src_connections;
    QComboBox *widget_doc_connections;
    QComboBox *widget_driver;
    QListWidget *listWidget_projects;

    //int selected_ix;
public:
    //static const QString filename;
    QString currentProjectName;
    QList<dbConnection> dbConnections;
    QList<srcConnection> srcConnections;
    QList<docConnection> docConnections;

    dbConnection* findDbConnection(const QString& connName);

    Beallitasok();
    ~Beallitasok();

    void init(QLineEdit*, QLineEdit*, QLineEdit*,  QComboBox *qc, QComboBox *dc, QListWidget *lw, QComboBox *qsrcc, QComboBox *qdocc);

    void load();
    //void addConnection(dbConnection);



    QString getTemplateFilename(const QString& tfname);

    QString getModelFilename(const QString& tfname, const QString& dirname);


//zSQL("QMYSQL", "127.0.0.1", "wiki1", "root","Aladar123", "w1"),

    dbConnection getUI();
    void setUI(const dbConnection&);

    //dbConnection* getSelectedDbConnection();


/*
    QString driver = "QMYSQL";
    QString adatbazisNev = "wiki1";
    QString server = "127.0.0.1";
    QString user = "root";
    QString password= "Aladar123";
*/

/*
torzs_felhasznalo
id
firstname, char, 45, "Vezetéknév"
username, char 50, "Felhasználó"
passwd, "Jelszó"
lastname, "Keresztnév"
admin, nullable varchar(256)
inactive, bit(1)
abrachadabra
*/

/*
   // SQL SERVER
    QString driver = "QODBC";
    QString adatbazisNev = "enyv";
    QString server = "DIMOTION";
    QString user = "sa";
    QString password= "Gtr7jv8fh2";
*/
/*
    QString driver = "QODBC";
    QString adatbazisNev = "Gloster";
    QString server = "DEATHSTAR";
    QString user = "sa";
    QString password= "Gtr7jv8fh2";
*/

//    QString driver = "QODBC";
//    QString adatbazisNev = "ServiceFlex";
//    QString server = "deathstar";
//    QString user = "sa";
//    QString password= "Gtr7jv8fh2";

/*
   QString adatbazisNev = "d_solutions";
    QString server = "178.48.178.42,420";
    QString user = "digital";
    QString password= "Aladar123";
*/
/*
     QString adatbazisNev = "digman";
     QString server = "178.48.178.42,420";
     QString user = "digital";
     QString password= "Aladar123";
*/
/*
    QString adatbazisNev = "elelmiszermentes";
    QString server = "178.48.178.42,420";
    QString user = "elelmiszermentes";
    QString password= "Aladar123";
*/

    //QString connectionString = connectionTemplate.arg(server).arg(adatbazisNev);

    QString approot = QStringLiteral(R"(retek2_data)");
    // ez a templatedir
    QString tmpDir = QStringLiteral(R"(template_dir)");//retek2

    // ez a projectek elérési útja
    QString projectdir = QStringLiteral(R"(munka_dir)");//retek2
    QString settingsdir = QStringLiteral(R"(settings)");//retek2

   // QString settingsPath;
   // QString projectPath;

    QString dbconnections_filename = QStringLiteral(R"(dbconnections.csv)");
    QString source_connections_filename = QStringLiteral(R"(srcconnections.csv)");
    QString document_connections_filename = QStringLiteral(R"(docconnections.csv)");

    QString settings_filename = QStringLiteral(R"(settings.csv)");
    /*
#if defined(Q_OS_WIN)
	QString tmpDir = R"(C:\retek2\template_dir)";
    QString munkadir = R"(C:\retek2\munka_dir)";
#elif defined(Q_OS_LINUX)
    QString tmpDir = R"(retek2/template_dir)";
    QString munkadir = R"(retek2/munka_dir)";
#endif
*/
    QString globalCaptionsFileName = QStringLiteral("global_captions");

    void setSelected(int i);

    void setDbConnections(const QList<dbConnection>&s);
    void addDbConnection(const dbConnection& b, bool isSelect = false);

    void setSrcConnections(const QList<srcConnection>&s);
    void addSrcConnection(const srcConnection& b, bool isSelect = false);

    void setDocConnections(const QList<docConnection>&s);
    void addDocConnection(const docConnection& b, bool isSelect = false);

    void FromCSV(QString& i);

    void fillProjectList(const QStringList&);

    void initPaths();

    dbConnection* getDbConnectionByName(const QString& name);
    srcConnection* getSrcConnectionByName(const QString& name);
    dbConnection* getDbConnectionBySchemaName(const QString& name);
    docConnection* getDocConnectionByName(const QString& name);

private:
    void addSrcConnectionToUI(const srcConnection& b, bool isSelect = false);
    void addSrcConnectionToCSV(const srcConnection& b);

    void addDbConnectionToUI(const dbConnection& b, bool isSelect = false);
    void addDbConnectionToCSV(const dbConnection& b);

    void addDocConnectionToUI(const docConnection& b, bool isSelect = false);
    void addDocConnectionToCSV(const docConnection& b);
};

#endif
