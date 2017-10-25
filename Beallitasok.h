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

//zSQL("QMYSQL", "127.0.0.1", "wiki1", "root","Aladar123", "w1"),

    QString driver = "QMYSQL";
    QString adatbazisNev = "wiki1";
    QString server = "127.0.0.1";
    QString user = "root";
    QString password= "Aladar123";


    /*
   // SQL SERVER
    QString driver = "QODBC";
    QString adatbazisNev = "enyv";
    QString server = "DIMOTION";
    QString user = "sa";
    QString password= "Gtr7jv8fh2";
*/
    /*
    QString adatbazisNev = "Gloster";
    QString server = "DEATHSTAR";
    QString user = "sa";
    QString password= "Gtr7jv8fh2";
    */

/*
    QString adatbazisNev = "ServiceFlex";
    QString server = "deathstar";
    QString user = "sa";
    QString password= "Gtr7jv8fh2";
*/
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
 /*    QString adatbazisNev = "elelmiszermentes";
    QString server = "178.48.178.42,420";
    QString user = "elelmiszermentes";
    QString password= "Aladar123";

*/

    //QString connectionString = connectionTemplate.arg(server).arg(adatbazisNev);

	QString tmpDir = R"(C:\retek2\template_dir)";
    QString munkadir = R"(C:\retek2\munka_dir)";
	QString globalCaptionsFileName = "global_captions"; 
};

