#pragma once

#include <QString>
#include <QLineEdit>

class Beallitasok
{
private:
    QLineEdit *widget_user;
    QLineEdit *widget_password;
    QLineEdit *widget_server;
    QLineEdit *widget_adatbazisNev;

public:
    Beallitasok();
    ~Beallitasok();

    void init(QLineEdit*, QLineEdit*, QLineEdit*, QLineEdit*);

    void Load();

    void getUI();
    void setUI();

    QString getTemplateFilename(QString tfname);

    QString getModelFilename(QString tfname, QString dirname);

//zSQL("QMYSQL", "127.0.0.1", "wiki1", "root","Aladar123", "w1"),

    QString driver = "QMYSQL";
    QString adatbazisNev = "wiki1";
    QString server = "127.0.0.1";
    QString user = "root";
    QString password= "Aladar123";

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
    QString tmpDir = R"(retek2/template_dir)";
    QString munkadir = R"(retek2/munka_dir)";
    /*
#if defined(Q_OS_WIN)
	QString tmpDir = R"(C:\retek2\template_dir)";
    QString munkadir = R"(C:\retek2\munka_dir)";
#elif defined(Q_OS_LINUX)
    QString tmpDir = R"(retek2/template_dir)";
    QString munkadir = R"(retek2/munka_dir)";
#endif
*/
	QString globalCaptionsFileName = "global_captions"; 

    QString getCaptionFileName(QString tablanev);
};

