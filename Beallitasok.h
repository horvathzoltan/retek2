#pragma once

#include <QString>

class Beallitasok
{
public:
	Beallitasok();
	~Beallitasok();

    QString adatbazisNev = "enyv";
    QString server = "DIMOTION";
    QString user = "sa";
    QString password= "Gtr7jv8fh2";

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
	QString connectionTemplate = "DRIVER={SQL SERVER};SERVER=%1;DATABASE=%2;";
    //QString connectionString = connectionTemplate.arg(server).arg(adatbazisNev);

	QString tmpDir = R"(C:\retek2\template_dir)";
    QString munkadir = R"(C:\retek2\munka_dir)";
	QString globalCaptionsFileName = "global_captions"; 
};

