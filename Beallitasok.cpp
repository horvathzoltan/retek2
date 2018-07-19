#include "globals.h"
//#include "Beallitasok.h"
#include "zfilenamehelper.h"
#include "zstringhelper.h"

#include "ztextfilehelper.h"



#include <QDir>
#include <QFileInfo>

const QString Beallitasok::filename =  QStringLiteral("caption_global.txt");

Beallitasok::Beallitasok(){
    //"QMYSQL", "wiki1", "127.0.0.1", "root", "Aladar123"
    //dbConnections.append(dbConnection{"QMYSQL", "wiki1", "127.0.0.1", "root", "Aladar123"});

     selected_ix = 0;
};

Beallitasok::~Beallitasok()= default;;

void Beallitasok::init(QLineEdit* wu, QLineEdit* wp, QLineEdit* wserver, QLineEdit* wcatalog, QComboBox *qc, QComboBox *dc, QListWidget* lv)
{
    this->widget_user = wu;
    this->widget_password = wp;
    this->widget_server = wserver;
    this->widget_adatbazisNev = wcatalog;
    this->widget_connections = qc;
    this->widget_driver = dc;
    this->listWidget_projects = lv;
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

void Beallitasok::setUI(const dbConnection& b)
{
    widget_driver->setCurrentText(b.driver);
    widget_user->setText(b.user);
    widget_password->setText(b.password);
    widget_server->setText(b.server);
    widget_adatbazisNev->setText(b.adatbazisNev);
    //b.driver
}


QString Beallitasok::getCaptionFileName(const QString& tablanev){
    auto b = dbConnections[selected_ix];

    QString fn = zFileNameHelper::append(QDir::homePath(), munkadir, b.adatbazisNev, "caption_"+tablanev+".txt");
    return fn;
}

QString Beallitasok::getModelFilename(const QString& tfname, const QString& dirname) {
     auto b = getSelectedDbConnection();

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
QString Beallitasok::getTemplateFilename(const QString& tfname) {
    auto b = getSelectedDbConnection();

    if(b!=nullptr){
    bool isVal = true;
    if(tmpDir.isEmpty())
        {zlog.log(QStringLiteral("A template könyvtár a beállításokban nincs megadva"));isVal=false;}
    if(tfname.isEmpty())
        {zlog.log(QStringLiteral("A template fájlnév nincs megadva"));isVal=false;}
    if(b->adatbazisNev.isEmpty())
        {zlog.log(QStringLiteral("Az adatbázisnév nincs megadva"));isVal=false;}
    if(isVal == false)
        {zLog::ShowDialog(QStringLiteral("A template fájlnév nem meghatározható"));return nullptr;}

    auto fn = zFileNameHelper::append(QDir::homePath(),tmpDir, b->adatbazisNev, tfname);

    //zlog.log("project template keresese:"+ fn);
    if(QFileInfo::exists(fn))
        return fn;
    else{
        zlog.log("nincs project template:" +fn);

        fn = zFileNameHelper::append(QDir::homePath(),tmpDir, tfname);
        //fn = QString(beallitasok.tmpDir+R"(\%1)").arg(tfname);

        if(QFileInfo::exists(fn))
            return fn;
        else{
            zlog.log("nincs default template:"+ fn);
            }
        }
}
    else
        zlog.log(QStringLiteral("nincs kiválasztott dbconnection:"));

    return nullptr;
}

dbConnection* Beallitasok::getSelectedDbConnection(){
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

beállítások csv visszaolvasása

*/
void Beallitasok::load(){
    QString sdirPath = zFileNameHelper::append(QDir::homePath(),settingsdir);

    QString fn = zFileNameHelper::append(sdirPath, dbconnections_filename);

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

    fn = zFileNameHelper::append(sdirPath, settings_filename);
    txt = zTextFileHelper::load(fn);
    if(!txt.isEmpty()){
        QStringList csvl = zStringHelper::toStringList(txt);
        Beallitasok::FromCSV(csvl.first());
        }
    }

void Beallitasok::FromCSV(QString& i){
    QStringList a = i.split(zStringHelper::SEP);
    if(a.count()>0){
        currentProjectName= a[0];
    }
}

/**
    a projectlista csak a project könyvtár tartalmából származhat
    az aktuális project pedig csak a beállításból származhat
    - futás közben hozhat létre új könyvtárat, elvileg azt fel lehetne venni
    - aktuális könyvtár csak a listából választásra változhat meg
 * @brief Beallitasok::fillProjectList
 * @param projectdirs
 */
void Beallitasok::fillProjectList(const QStringList& projectdirs)
{
    listWidget_projects->addItems(projectdirs);

    if(!currentProjectName.isEmpty()){
        auto items = listWidget_projects->findItems(currentProjectName, Qt::MatchExactly);
        if(items.isEmpty()){
            zlog.log(QStringLiteral("Az aktuális project nem található: %1 ERROR").arg(beallitasok.currentProjectName));
        }
        else{
            listWidget_projects->setCurrentItem(items[0]);
        }
    }
}


void Beallitasok::addConnection(dbConnection b){  

    QString fn = zFileNameHelper::append(QDir::homePath(),settingsdir, dbconnections_filename, QString());
    QString csvr= b.ToCSV();

    zlog.log(QStringLiteral("dbconnection append %1 %2").arg(fn, csvr));
    zTextFileHelper::append(fn, csvr);  

    addDbConnection(b);
}

void Beallitasok::addDbConnection(dbConnection b){
    beallitasok.dbConnections.append(b);
    // itt történik a ztables beolvasása sql-ből
    // ezen a ponton ezt nek feltétlenül kellene
    widget_connections->addItem(b.Getname());
}


