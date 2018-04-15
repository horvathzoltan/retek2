#include "Beallitasok.h"
#include "zfilenamehelper.h"
#include "ztextfilehelper.h"
#include "zstringhelper.h"
#include "globals.h"

#include <QDir>
#include <QFileInfo>

Beallitasok::Beallitasok(){
    //"QMYSQL", "wiki1", "127.0.0.1", "root", "Aladar123"
    //dbConnections.append(dbConnection{"QMYSQL", "wiki1", "127.0.0.1", "root", "Aladar123"});

     selected_ix = 0;
};

Beallitasok::~Beallitasok(){};

void Beallitasok::init(QLineEdit* wu, QLineEdit* wp, QLineEdit* wserver, QLineEdit* wcatalog, QComboBox *qc, QComboBox *dc)
{
    this->widget_user = wu;
    this->widget_password = wp;
    this->widget_server = wserver;
    this->widget_adatbazisNev = wcatalog;
    this->widget_connections = qc;
    this->widget_driver = dc;
}

/*
        QString driver;
        QString adatbazisNev;
        QString server;
        QString user;
        QString password;
*/
dbConnection Beallitasok::getUI()
{
    QString driver;
    //int i = widget_driver->currentIndex();
    //if(i>-1)
         driver = widget_driver->currentText();

    QString user = widget_user->text();
    QString password = widget_password->text();
    QString server = widget_server->text();
    QString adatbazisNev = widget_adatbazisNev->text();

    return dbConnection{ driver, adatbazisNev, server, user, password};
}

void Beallitasok::setUI(dbConnection b)
{
    widget_driver->setCurrentText(b.driver);
    widget_user->setText(b.user);
    widget_password->setText(b.password);
    widget_server->setText(b.server);
    widget_adatbazisNev->setText(b.adatbazisNev);
    //b.driver
}


QString Beallitasok::getCaptionFileName(QString tablanev){
    auto b = dbConnections[selected_ix];

    QString fn = zFileNameHelper::append(QDir::homePath(), munkadir, b.adatbazisNev, "caption_"+tablanev+".txt");
    return fn;
}

QString Beallitasok::getModelFilename(QString tfname, QString dirname) {
     auto b = getSelected();

     if(b!=nullptr){
        //auto e = QString(munkadir+R"(\%2\%1)").arg(dirname).arg(adatbazisNev);
        QString  e = zFileNameHelper::append(QDir::homePath(),munkadir, b->adatbazisNev, dirname);
        QDir d(e);if(!d.exists()){d.mkpath(d.absolutePath());}

        e += QDir::separator()+tfname;
        zlog.trace(e);
        return e;
     }
     else
        return QString();
}

/*
 * a beállítások alapján a template névhez tartozó fájl nevét adja
*/
QString Beallitasok::getTemplateFilename(QString tfname) {
    auto b = getSelected();

    if(b!=nullptr){
    bool isVal = true;
    if(tmpDir.isEmpty())
        {zlog.log("A template könyvtár a beállításokban nincs megadva");isVal=false;}
    if(tfname.isEmpty())
        {zlog.log("A template fájlnév nincs megadva");isVal=false;}
    if(b->adatbazisNev.isEmpty())
        {zlog.log("Az adatbázisnév nincs megadva");isVal=false;}
    if(isVal == false)
        {zLog::ShowDialog("A template fájlnév nem meghatározható");return NULL;}

    auto fn = zFileNameHelper::append(QDir::homePath(),tmpDir, b->adatbazisNev, tfname);

    //zlog.log("project template keresese:"+ fn);
    if(QFileInfo(fn).exists())
        return fn;
    else{
        zlog.log("nincs project template:" +fn);

        fn = zFileNameHelper::append(QDir::homePath(),tmpDir, tfname);
        //fn = QString(beallitasok.tmpDir+R"(\%1)").arg(tfname);

        if(QFileInfo(fn).exists())
            return fn;
        else{
            zlog.log("nincs default template:"+ fn);
            }
        }
}
    else
        zlog.log("nincs kiválasztott dbconnection:");

    return NULL;
}

dbConnection* Beallitasok::getSelected(){
    if(dbConnections.isEmpty())
        return nullptr;
    return
        &(dbConnections[selected_ix]);
}

void Beallitasok::setSelected(int i){
   selected_ix = i;
}

/*
fel kell olvasni a kapcsolatokat
template_dir/connections.csv
fel kell olvasni a kapcsolat fájlt

*/
void Beallitasok::load(){

    QString fn = zFileNameHelper::append(QDir::homePath(),settingsdir, dbconnections_filename, "");       

    QString txt = zTextFileHelper::load(fn);
    widget_connections->clear();

    if(!txt.isEmpty()){
        //zlog.log(QString("beállítások_dbconnections: %1").arg(fn));
        QStringList csvl = zStringHelper::toStringList(txt);
        zforeach(csvr, csvl){
            auto dbconn = dbConnection::FromCSV(*csvr);
            if(dbconn.isValid()){
                addDbConnection(dbconn);
            }
        }

        selected_ix = 0;
    }
    return;
}

void Beallitasok::addConnection(dbConnection b){  

    QString fn = zFileNameHelper::append(QDir::homePath(),settingsdir, dbconnections_filename, "");
    QString csvr= b.ToCSV();

    zlog.log(QString("dbconnection append %1 %2").arg(fn).arg(csvr));
    zTextFileHelper::append(fn, csvr);  

    addDbConnection(b);
}

void Beallitasok::addDbConnection(dbConnection b){
    beallitasok.dbConnections.append(b);
    widget_connections->addItem(b.Getname());
}


