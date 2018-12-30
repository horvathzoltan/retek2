#include "globals.h"
//#include "Beallitasok.h"
#include "srcconnection.h"
#include "zfilenamehelper.h"
#include "zstringhelper.h"

#include "ztextfilehelper.h"



#include <QDir>
#include <QFileInfo>


Beallitasok::Beallitasok() = default;
//{
//    //"QMYSQL", "wiki1", "127.0.0.1", "root", "Aladar123"
//    //dbConnections.append(dbConnection{"QMYSQL", "wiki1", "127.0.0.1", "root", "Aladar123"});

//     //selected_ix = 0;
//};

Beallitasok::~Beallitasok()= default;;

dbConnection* Beallitasok::findDbConnection(const QString &connName)
{
    zforeach(c, this->dbConnections)
    {
        if(c->Name==connName)
        {
            return c.operator->();
        }
    }
    return nullptr;
}

void Beallitasok::init(QLineEdit* wu, QLineEdit* wp, QLineEdit* wserver,  QComboBox *qc, QComboBox *dc, QListWidget* lv,QComboBox *qsrcc , QComboBox *qdocc )
{
    this->widget_user = wu;
    this->widget_password = wp;
    this->widget_server = wserver;
    this->widget_connections = qc;
    this->widget_src_connections = qsrcc;
    this->widget_doc_connections = qdocc;

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
QString Beallitasok::getModelFilename(const QString& tfname, const QString& dirname)
{
    QString e = zFileNameHelper::getCurrentProjectSubDir(dirname);
    //QString  e = zFileNameHelper::append(QDir::homePath(),projectdir, beallitasok.currentProjectName, dirname);
    QDir d(e);

    if(!d.exists()){d.mkpath(d.absolutePath());}

    //e += QDir::separator()+tfname;
    QString e2 = zFileNameHelper::getCurrentProjectFileName(tfname);
    zInfo(e2);
    return e2;

}

/*
 * a beállítások alapján a template névhez tartozó fájl nevét adja
 * ezt az aktuális projectnév alapján kell megtenni
*/
QString Beallitasok::getTemplateFilename(const QString& tfname)
{
    bool isVal = true;
    if(tmpDir.isEmpty())
    {
        zError("A template könyvtár a beállításokban nincs megadva");
        isVal=false;
    }
    if(tfname.isEmpty())
    {
        zError(QStringLiteral("A template fájlnév nincs megadva"));
        isVal=false;
    }
    if(beallitasok.currentProjectName.isEmpty())
    {
        zError(QStringLiteral("A projectnév nincs megadva"));
        isVal=false;
    }
    if(!isVal)
    {
        zLog::dialogError(QStringLiteral("A template fájlnév nem meghatározható"));
        return nullptr;
    }

    auto fn = zFileNameHelper::getCurrentTmpSubDir(tfname);

    if(QFileInfo::exists(fn))
    {
        return fn;
    }

    zInfo("nincs project template:" +fn);
    fn = zFileNameHelper::getTmpSubDir(tfname);

    if(QFileInfo::exists(fn))
    {
        zInfo("global template:" +fn);
        return fn;
    }

    zError("nincs global template:"+ fn);
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
void Beallitasok::load()
{
//    QString fn = zFileNameHelper::getDbConnFileName();

//    QString txt = zTextFileHelper::load(fn);
//    widget_connections->clear();
//    if(!txt.isEmpty())
//    {
//        //zlog.log(QString("beállítások_dbconnections: %1").arg(fn));
//        QStringList csvl = zStringHelper::toStringList(txt);
//        zforeach(csvr, csvl)
//        {
//            auto dbconn = dbConnection::FromCSV(*csvr);
//            if(dbconn.isValid())
//            {
//                addDbConnection(dbconn);
//            }
//        }

//       // selected_ix = 0;
//    }

    setDbConnections(dbConnection::fromCSVFile(zFileNameHelper::getDbConnFileName()));
    setSrcConnections(srcConnection::fromCSVFile(zFileNameHelper::getSourceConnFileName()));
    setDocConnections(docConnection::fromCSVFile(zFileNameHelper::getDocumentConnFileName()));

    auto fn = zFileNameHelper::getSettingsFileName();//append(settingsPath, settings_filename);
    auto txt = zTextFileHelper::load(fn);
    if(!txt.isEmpty())
    {
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
            zError(QStringLiteral("Az aktuális project nem található: %1 ERROR").arg(currentProjectName));
        }
        else{
            listWidget_projects->setCurrentItem(items[0]);
        }
    }
}


//void Beallitasok::addConnection(dbConnection b){

//    QString fn = zFileNameHelper::getDbConnFileName();

//    //QString fn = zFileNameHelper::append(QDir::homePath(),settingsdir, dbconnections_filename, QString());
//    QString csvr= b.ToCSV();

//    zError(QStringLiteral("dbconnection append %1 %2").arg(fn, csvr));
//    //zTextFileHelper::append(fn, csvr);
//    zTextFileHelper::save(csvr, fn, true);

//    addDbConnection(b);
//}

/*DbConnection*/

void Beallitasok::addDbConnection(const dbConnection& b, bool isSelect)
{
    beallitasok.dbConnections.append(b);
    addDbConnectionToUI(b, isSelect);
    addDbConnectionToCSV(b);
}

void Beallitasok::addDbConnectionToUI(const dbConnection& b, bool isSelect)
{
    widget_connections->addItem(b.Name);
    if(isSelect) widget_src_connections->setCurrentText(b.Name);
}

void Beallitasok::addDbConnectionToCSV(const dbConnection& b)
{
    zTextFileHelper::save(b.toCSV(), zFileNameHelper::getDbConnFileName(), true);
}


void Beallitasok::setDbConnections(const QList<dbConnection>&s)
{
    widget_connections->clear();
    beallitasok.dbConnections.clear();
    zforeach(b, s)
    {
        beallitasok.dbConnections.append(*b);
        addDbConnection(*b);
    }
}

dbConnection* Beallitasok::getDbConnectionByName(const QString& name)
{
    zforeach(o, dbConnections)
    {
        if(o->Name == name) return &(*o);
    }
    return nullptr;
}

/*SrcConnection*/

void Beallitasok::addSrcConnectionToCSV(const srcConnection& b)
{
    zTextFileHelper::save(b.toCSV(), zFileNameHelper::getSourceConnFileName(), true);
}

void Beallitasok::addSrcConnectionToUI(const srcConnection& b, bool isSelect)
{
    widget_src_connections->addItem(b.Name);
    if(isSelect) widget_src_connections->setCurrentText(b.Name);
}

void Beallitasok::addSrcConnection(const srcConnection& b, bool isSelect)
{
    beallitasok.srcConnections.append(b);
    addSrcConnectionToUI(b, isSelect);
    addSrcConnectionToCSV(b);
}

void Beallitasok::setSrcConnections(const QList<srcConnection>&s)
{
    widget_src_connections->clear();
    beallitasok.srcConnections.clear();
    zforeach(b, s)
    {
        beallitasok.srcConnections.append(*b);
        addSrcConnectionToUI(*b);
    }
}

srcConnection* Beallitasok::getSrcConnectionByName(const QString& name)
{
    zforeach(o, srcConnections)
    {
        if(o->Name == name) return &(*o);
    }
    return nullptr;
}

/*docConnection*/

void Beallitasok::addDocConnectionToCSV(const docConnection& b)
{
    zTextFileHelper::save(b.toCSV(), zFileNameHelper::getSourceConnFileName(), true);
}

void Beallitasok::addDocConnectionToUI(const docConnection& b, bool isSelect)
{
    widget_doc_connections->addItem(b.Name);
    if(isSelect) widget_doc_connections->setCurrentText(b.Name);
}

void Beallitasok::addDocConnection(const docConnection& b, bool isSelect)
{
    beallitasok.docConnections.append(b);
    addDocConnectionToUI(b, isSelect);
    addDocConnectionToCSV(b);
}

void Beallitasok::setDocConnections(const QList<docConnection>&s)
{
    widget_doc_connections->clear();
    beallitasok.docConnections.clear();
    zforeach(b, s)
    {
        beallitasok.docConnections.append(*b);
        addDocConnectionToUI(*b);
    }
}

docConnection* Beallitasok::getDocConnectionByName(const QString& name)
{
    zforeach(o, docConnections)
    {
        if(o->Name == name) return &(*o);
    }
    return nullptr;
}

