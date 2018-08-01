#include "globals.h"
//#include "Beallitasok.h"
#include "zfilenamehelper.h"
#include "zstringhelper.h"

#include "ztextfilehelper.h"



#include <QDir>
#include <QFileInfo>


Beallitasok::Beallitasok(){
    //"QMYSQL", "wiki1", "127.0.0.1", "root", "Aladar123"
    //dbConnections.append(dbConnection{"QMYSQL", "wiki1", "127.0.0.1", "root", "Aladar123"});

     //selected_ix = 0;
};

Beallitasok::~Beallitasok()= default;;

void Beallitasok::init(QLineEdit* wu, QLineEdit* wp, QLineEdit* wserver,  QComboBox *qc, QComboBox *dc, QListWidget* lv)
{
    this->widget_user = wu;
    this->widget_password = wp;
    this->widget_server = wserver;
    this->widget_connections = qc;
    this->widget_driver = dc;
    this->listWidget_projects = lv;
}

/**
        * @brief feltölti
        *

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
    QString adatbazisNev = ""; //widget_adatbazisNev->text();

    return dbConnection{ driver, adatbazisNev, server, user, password};
}

void Beallitasok::setUI(const dbConnection& b)
{
    widget_driver->setCurrentText(b.driver);
    widget_user->setText(b.user);
    widget_password->setText(b.password);
    widget_server->setText(b.server);
    //widget_adatbazisNev->setText(b.adatbazisNev);
    //b.driver
}


//QString Beallitasok::getCaptionFileName(const QString& tablanev){
//    auto b = dbConnections[selected_ix];

//    QString fn = zFileNameHelper::append(QDir::homePath(), projectdir, b.adatbazisNev, "caption_"+tablanev+".txt");
//    return fn;
//}

// megy a
QString Beallitasok::getModelFilename(const QString& tfname, const QString& dirname) {

        QString e = zFileNameHelper::getCurrentProjectSubDir(dirname);
        //QString  e = zFileNameHelper::append(QDir::homePath(),projectdir, beallitasok.currentProjectName, dirname);
        QDir d(e);

        if(!d.exists()){d.mkpath(d.absolutePath());}

        //e += QDir::separator()+tfname;
        QString e2 = zFileNameHelper::getCurrentProjectFileName(tfname);
        zlog.trace(e2);
        return e2;

}

/*
 * a beállítások alapján a template névhez tartozó fájl nevét adja
 * ezt az aktuális projectnév alapján kell megtenni
*/
QString Beallitasok::getTemplateFilename(const QString& tfname) {   
    bool isVal = true;
    if(tmpDir.isEmpty())
        {zlog.error("A template könyvtár a beállításokban nincs megadva");isVal=false;}
    if(tfname.isEmpty())
        {zlog.error(QStringLiteral("A template fájlnév nincs megadva"));isVal=false;}
    if(beallitasok.currentProjectName.isEmpty())
        {zlog.error(QStringLiteral("A projectnév nincs megadva"));isVal=false;}
    if(!isVal)
        {zLog::dialogError(QStringLiteral("A template fájlnév nem meghatározható"));return nullptr;}


    // project template
    auto fn = zFileNameHelper::getCurrentTmpSubDir(tfname);
    //auto fn = zFileNameHelper::append(QDir::homePath(),tmpDir, beallitasok.currentProjectName, tfname);
    //auto fn = zFileNameHelper::append(beallitasok.projectPath, tfname);

    //zlog.log("project template keresese:"+ fn);
    if(QFileInfo::exists(fn))
        return fn;
    else{
        zlog.error("nincs project template:" +fn);

        fn = zFileNameHelper::getTmpSubDir(tfname);//append(QDir::homePath(),tmpDir, tfname);
        //fn = QString(beallitasok.tmpDir+R"(\%1)").arg(tfname);

        if(QFileInfo::exists(fn))
            return fn;
        else{
            zlog.error("nincs default template:"+ fn);
            }
        }
//}
//    else
//        zlog.log(QStringLiteral("nincs kiválasztott dbconnection:"));

    return nullptr;
}


dbConnection* Beallitasok::getDbConnectionByName(const QString& name){
    zforeach(o, dbConnections){
        if(o->Name == name) return &(*o);
    }
    return nullptr;
}

//dbConnection* Beallitasok::getDbConnectionBySchemaName(const QString& name){
//    zforeach(o, dbConnections){
//        if(o->schemaName == name) return &(*o);
//    }
//    return nullptr;
//}

// valójában ezt egy helpernek kellene?
//void Beallitasok::initPaths(){
//    settingsPath = zFileNameHelper::getSettingsDir();//append(QDir::homePath(),settingsdir);
//    projectPath  = zFileNameHelper::getProjectDir();//append(QDir::homePath(),projectdir);
//}


/*
fel kell olvasni a kapcsolatokat
template_dir/connections.csv
fel kell olvasni a kapcsolat fájlt

beállítások csv visszaolvasása

*/
void Beallitasok::load(){

    //QString path = beallitasok.settingsPath;
    QString fn = zFileNameHelper::getDbconnFileName();//append(path, dbconnections_filename);

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

       // selected_ix = 0;
    }

    fn = zFileNameHelper::getSettingsFileName();//append(settingsPath, settings_filename);
    txt = zTextFileHelper::load(fn);
    if(!txt.isEmpty()){
        QStringList csvl = zStringHelper::toStringList(txt);
        Beallitasok::FromCSV(csvl.first());
        }
    }

void Beallitasok::FromCSV(QString& i){
    QStringList a = i.split(zStringHelper::SEP);
    if(a.count()>0){
        currentProjectName = a[0];
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
            zlog.error(QStringLiteral("Az aktuális project nem található: %1 ERROR").arg(currentProjectName));
        }
        else{
            listWidget_projects->setCurrentItem(items[0]);
        }
    }
}


void Beallitasok::addConnection(dbConnection b){  

    QString fn = zFileNameHelper::getDbconnFileName();

    //QString fn = zFileNameHelper::append(QDir::homePath(),settingsdir, dbconnections_filename, QString());
    QString csvr= b.ToCSV();

    zlog.error(QStringLiteral("dbconnection append %1 %2").arg(fn, csvr));
    zTextFileHelper::append(fn, csvr);  

    addDbConnection(b);
}



void Beallitasok::addDbConnection(dbConnection b){
    beallitasok.dbConnections.append(b);        
    widget_connections->addItem(b.Name);
}




