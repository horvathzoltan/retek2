#include "globals.h"
#include "zmacro.h"
#include "retek2.h"

#include "zfilenamehelper.h"
#include "zstringhelper.h"
#include "zstringmaphelper.h"
#include "zsourcehelper.h"
#include "ztablerow.h"
#include "ztextfilehelper.h"
#include "zshortguid.h"
#include "zfilenamehelper.h"
#include "zoshelper.h"
#include "ziconhelper.h"
#include "zdownloader.h"
#include "ztableerror.h"
//#include "highlighter.h"

#include <QRegularExpression>
#include <QTableWidgetItem>

#include <QFileInfo>

#include <QListWidget>
#include <QMessageBox>
#include <QSqlError>

#include <QClipboard>
#include <QDir>
#include <QFileDialog>
#include <QIcon>
#include <QStandardPaths>

//#include <QDebug>
//#include <QString>
//#include <QSqlQuery>
//#include <QFile>
//#include <QTableWidgetItem>
//#include <QTextCodec>
//#include <QMap>
//#include "ztable.h"
//#include "zsql.h"
//#include "zenumizer.h"
#include <QWidget>
#include "zconversionmap.h"

//sss

Highlighter* retek2::h1 = nullptr;
Highlighter* retek2::h2 = nullptr;

retek2::retek2(QWidget *parent):QMainWindow(parent)
{
    //ui=NULL;
    //zTableNameDialog = nullptr;
    //ui.setupUi(this);
}

retek2::~retek2()
{
    delete h1;
    delete h2;
}

/**
    // Ha van elmentett projectnév
    // beállítjuk a listában aktuálisnak
    // majd a project név alapján elérési utat képzünk
    // és az ott található táblákat beolvassuk
    // az ott tárolt xml leírójuk alapján
*/

void retek2::init()
{	
    ui.setupUi(this);
    setEnabled(false);
    ui.listWidget_ztables->setIconSize(QSize(48,24));
    zLog::init(retek2::logToGUI, false, &ui, false);

    //zInfo(QStringLiteral("...init 1"));
    beallitasok.init(
                ui.lineEdit_User,
                ui.lineEdit_Password,
                ui.lineEdit_Server,
                ui.comboBox_connections,
                ui.comboBox,
                ui.listWidget_projects,
                ui.comboBox_srcconn,
                ui.comboBox_docconn
                );
    //zInfo(QStringLiteral("...init 2"));

    zosHelper::setLocale();

    //zInfo(QStringLiteral("...init 3"));

    auto sp = zFileNameHelper::getSettingsDir();
    //zInfo(QStringLiteral("...init 3.1"));
    auto pp = zFileNameHelper::getProjectDir();
    //zInfo(QStringLiteral("...init 3.2"));
    bool ok = true;
    if(zTextFileHelper::isExistDirW(sp)){
        //zInfo(QStringLiteral("...init 3.3"));
        beallitasok.load();
        //zInfo(QStringLiteral("...init 3.4"));
        // Mezőmegnevezés
        globalCaptionMaps = zConversionMap::loadAll(sp, zFileNameHelper::captionFileFilter);
        //zInfo(QStringLiteral("...init 3.5"));
        // típuskonverzió
        globalSqlMaps= zConversionMap::loadAll(sp, zFileNameHelper::sqlmapFileFilter);
        //zInfo(QStringLiteral("...init 3.6"));
        globalClassMaps= zConversionMap::loadAll(sp, zFileNameHelper::classmapFileFilter);
        //zInfo(QStringLiteral("...init 3.7"));
    }
    else
    {
        ok = false;
        zInfo(QStringLiteral("No settings dir: %1").arg(sp));
    }

    //zInfo(QStringLiteral("...init 4"));

    if(zTextFileHelper::isExistDirW(pp)){
        // Mezőmegnevezés 2
        projectCaptionMaps = zConversionMap::loadAll(pp, zFileNameHelper::captionFileFilter);
        auto projectdirs = zFileNameHelper::GetSubdirs(pp);
        beallitasok.fillProjectList(projectdirs);

        loadCurrentProject(); // ez tölti a ztablakat XML-ből

        fillListWidgetByCurrentProject();

        //auto e = downloader.download(QStringLiteral(R"(https://docs.google.com/document/d/1tPwsVMObxU9QmA3XR4RpbHPpjcG7hVbd7KQqLD_ABK8/edit?usp=sharing)"));

        // TODO - periodikus validáció
        // QTimer: a forrás doc, src és sql->create table hash alapján - és az
        // utolsó megnyitás után
        // ha volt piszkálva, előző = 1 perc
        // ha nem volt piszkálva, előző*=2 , ha az kisebb, mint 16 -egyébként = 16 , azaz percenként mindenképp nézünk
        // tehát minden doksira kell egy SHA1 és ha az változik, akkor piszka volt
        // akkor lehet végrehajtani, ha nem fut a táblán művelet - illetve egyik táblán sem fut művelet

        auto valmap = validateCurrentProject();
        auto sqlmap = validateCurrentProject_SQL();
        auto srcmap = validateCurrentProject_Source();
        auto docmap = validateCurrentProject_Document();

        setListWidgetIconsByCurrentProject(sqlmap, srcmap, docmap, valmap);

        ztokenizer.init(ui.tableWidget_MezoLista);
    }
    else
    {
        zInfo(QStringLiteral("No project dir: %1").arg(pp));
        ok = false;
    }

    h1 = new Highlighter(ui.textBrowser_sources->document());
    h2 = new Highlighter(ui.textBrowser_docs->document());

    auto a2 = zConversionMap::externals(globalClassMaps);
    //zInfo(QStringLiteral("...init 6"));
    setEnabled(true);
    zInfo(QStringLiteral("retek2 init %1").arg(ok?"ok":"failed"));
}

void retek2::fillListWidgetByCurrentProject()
{
    zforeach(t, ztables)
    {
        add_zTablaToListWidget(*t);
    }
}

QListWidgetItem* retek2::getTablesItem(const QString &tn){
    auto items = ui.listWidget_ztables->findItems(tn, Qt::MatchExactly);
    if(items.isEmpty()) return nullptr;
    if(items.count()>1)
    {
        zInfo(QStringLiteral("Több tábla található azonos néven: %1 %2").arg(tn, zLog::ERROR));
        return nullptr;
    }
    return items[0];
}

void retek2::setZTablesItem(const zTable &t,
                            const QMap<QString, bool>& sqlmap,
                            const QMap<QString, bool> &srcmap,
                            const QMap<QString, bool>& docmap,
                            const QMap<QString, bool>& valmap, bool isIcons)
{
 //   auto items = ui.listWidget_ztables->findItems(t.name, Qt::MatchExactly);
 //   if(items.isEmpty()) return;
 //   if(items.count()>1) {zInfo(QStringLiteral("Több tábla található azonos néven: %1 %2").arg(t.name, zLog::ERROR));return;}

    auto item = getTablesItem(t.name);
    if(!item) return;

    if(isIcons)
    {
        QStringList icons = getIconsByFlags(t.name, sqlmap, srcmap, docmap);
        auto icon = zIconHelper::concatenate(icons);
        item->setIcon(icon);
    }

    if(valmap.contains(t.name))
    {
        if(valmap.value(t.name, false))
        {
            item->setBackgroundColor(Qt::white);
            item->setToolTip(zStringHelper::Empty);
        }
        else
        {
            item->setBackgroundColor(Qt::yellow);
            auto ttip = zStringHelper::Empty;
            zforeach(a,t.eval)
            {
                if(!ttip.isEmpty()) ttip+=zStringHelper::NewLine;
                ttip+= a->toString();
            }
            if(ttip.isEmpty()) ttip = QStringLiteral("hibalista a TextOutputon");
            item->setToolTip(ttip);
        }
    }
    else{
        item->setBackgroundColor(Qt::white);
        item->setToolTip(zStringHelper::Empty);
    }

}

void retek2::setListWidgetIconsByCurrentProject(const QMap<QString, bool>& sqlmap, const QMap<QString, bool> &srcmap, const QMap<QString, bool>& docmap, const QMap<QString, bool>& valmap)
{
    zforeach(t, ztables)
    {
        setZTablesItem(*t, sqlmap, srcmap, docmap, valmap);
    }
}


QStringList retek2::getIconsByFlags(const QString& name, const QMap<QString, bool> &sqlmap, const QMap<QString, bool> &srcmap, const QMap<QString, bool> &docmap)
{
    QStringList e;

//    if(name=="fue")
//    {
//        zTrace();
//    }
//    if(valmap.contains(name))
//    {
//        if(!valmap.value(name, false))
//        {
//            e << QStringLiteral(":/alert-triangle.ico");
//        }
//        else
//        {
//            e << QStringLiteral(":/alert-triangle.ico|x");
//        }
//    }

    if(sqlmap.contains(name))
    {
        if(sqlmap.value(name, false))
        {
            e << QStringLiteral(":/database.ico");
        }
        else
        {
            e << QStringLiteral(":/database.ico|x");
        }
    }

    if(srcmap.contains(name))
    {
        if(srcmap.value(name, false))
        {
            e << QStringLiteral(":/class.ico");
        }
        else
        {
            e << QStringLiteral(":/class.ico|x");
        }
    }


    if(docmap.contains(name))
    {
        if(docmap.value(name, false))
        {
            e << QStringLiteral(":/file-text.ico");
        }
        else
        {
            e << QStringLiteral(":/file-text.ico|x");
        }
    }

    return e;
}

void retek2::loadCurrentProject()
{      
    if(beallitasok.currentProjectName.isEmpty())
    {
        zInfo(QStringLiteral("Nincs aktuális project. ERROR"));
    }
    else
    {
        ui.projectLabel->setText(beallitasok.currentProjectName);
        ztables.clear();
        ui.listWidget_ztables->clear();

        QString currentProjectPath = zFileNameHelper::getCurrentProjectDir();
        QStringList files = zFileNameHelper::FindFileNameInDir(currentProjectPath, QString(), zFileNameHelper::xmlFilter);
        zforeach(f, files)
        {
            auto txt = zTextFileHelper::load(*f);

            auto t = zTable::createTableByXML(txt);

            if(t.isEmpty())
            {
                zError(QStringLiteral("Nincs tábla: %1").arg(*f));
                continue;
            }

            if(t.count()>1)
            {
                zError(QStringLiteral("Több tábla: %1").arg(*f));
                continue;
            }

            auto t0 = t[0];

            t0.XMLPath = *f;
            // ha nincs név, lesz
            if(t0.name.isEmpty())
            {
             //   QString fn = zFileNameHelper::getfileName(*f);
                QString fn = t0.getName();
                zWarning(QStringLiteral("Nincs név: %1 (.xml)").arg(fn));
                t0.name = fn;
            }
            //bool isValid = t0.Validate(ztables, t0.eval, zfn());

//            if(zTable::find(ztables, t0.name, zTableSearchBy::Name))
//            {
//                QString fn = zFileNameHelper::getfileName(*f);
//                zError(QStringLiteral("Már van tábla ilyen néven: %1, %2 (.xml)").arg(t0.name, fn));
//            }
//            else
            ztables << t0;
//            add_zTablaToListWidget(t0);

//            if(!isValid)
//            {
//                zInfo(QStringLiteral("A tábla nem valid: %1 error").arg(t0.name));
//            }
          }
       }

}



void retek2::validateTable(zTable& t, QMap<QString,bool>& e)
{
    QStringList errlist;
    bool isValid = t.Validate(ztables, t.eval, zTable::VALIDATETABLEKEY, errlist);
    e.insert(t.name, isValid);
    if(!isValid)
    {
        zInfo(QStringLiteral("A tábla nem valid: %1 error").arg(t.getName()));
        zInfo(errlist);
    }
}

QMap<QString,bool> retek2::validateCurrentProject(){
    //zTrace();
    QMap<QString,bool> e;

    zforeach(t, ztables)
    {
        validateTable(*t, e);
    }
    return e;
}



QMap<QString,bool> retek2::validateCurrentProject_SQL(){
    //zTrace();
    QMap<QString,bool> e;

    zforeach(t, ztables)
    {       
        validateTableSQL(*t, e);
    }

    return e;
}

void retek2::validateTableSQL(zTable& t, QMap<QString,bool>& e){
    //zTrace(QStringLiteral("sql validating table: %1").arg(t->name));
    if(t.sql_conn.isEmpty()) return;

    if(t.sql_updateTimeStamp.isNull())
    {
        zInfo(QStringLiteral("no sql_updateTimeStamp"));
    }

    QDateTime lastUpdateTimeStamp = t.getSqlUpdateTimestamp();
    // ha újabb -  ha nem újabb, meg kell tudni, hogy valid-e
    // amíg nem tároljuk az utolsó állapotot
    if(true || lastUpdateTimeStamp > t.sql_updateTimeStamp) // ha az sql újabb
    {
        zInfo(QStringLiteral("newer available: %1 at: %2").arg(t.name, t.sql_updateTimeStamp.toString()));
        auto isValid = t.validateSQL();
        e.insert(t.name, isValid);
        zInfo(QStringLiteral("isValid: %1").arg(zStringHelper::boolToString(isValid)));
    } // egyébként nincs változás
    else
    {
        zInfo(QStringLiteral("not changed."));
    }
}

// TODO if(true ||  - nem kell az élesben
QMap<QString, bool> retek2::validateCurrentProject_Source(){
    QMap<QString, bool> e;
    zforeach(t, ztables)
    {        
        validateTableSource(*t,e);
    }
    return e;
}

void retek2::validateTableSource(zTable& t, QMap<QString,bool>& e)
{
    if(t.class_path.isEmpty()) return;

    if(t.source_updateTimeStamp.isNull())
    {
        zInfo(QStringLiteral("no source_updateTimeStamp"));
    }

    QDateTime lastUpdateTimeStamp = t.getSourceUpdateTimestamp();

    if(true || lastUpdateTimeStamp > t.source_updateTimeStamp) // ha a forrás újabb
    {
        auto a = t.validateSource();
        e.insert(t.name, a);
    }
    else
    {
        zInfo(QStringLiteral("not changed."));
    }
}

// TODO if(true ||  - nem kell az élesben
QMap<QString, bool> retek2::validateCurrentProject_Document(){
    QMap<QString, bool>e;

    zforeach(t, ztables)
    {
        validateTableDocument(*t, e);
    }

    return e;
}

void retek2::validateTableDocument(zTable& t, QMap<QString,bool>& e)
{
    if(t.document_path.isEmpty()) return;

    if(t.document_updateTimeStamp.isNull())
    {
        zInfo(QStringLiteral("no document_updateTimeStamp"));
    }

    QDateTime lastUpdateTimeStamp = t.getDocUpdateTimestamp();

    if(true || lastUpdateTimeStamp > t.document_updateTimeStamp) // ha a leírás újabb
    {
        auto a = t.validateDocument();
        e.insert(t.name, a);
    }
    else
    {
        zInfo(QStringLiteral("not changed."));
    }
}

void retek2::add_zTablaToListWidget(const zTable& t){
   // QString tn = t.name; // displayed name
    if(t.name.isEmpty())
    {
        QString tn = t.getName();
        zError(QStringLiteral("Nincs megnevezés: %1").arg(tn));
        return;
    }

    new QListWidgetItem(t.name, ui.listWidget_ztables);
}



////	QString commandTextTemplate = "SELECT  obj.name AS FK_NAME, "
////		"sch.name AS[schema_name], "
////		"tab1.name AS[table], "
////		"col1.name AS[column], "
////		"tab2.name AS[referenced_table], "
////		"col2.name AS[referenced_column] "
////		"FROM sys.foreign_key_columns fkc "
////		"INNER JOIN sys.objects obj "
////		"ON obj.object_id = fkc.constraint_object_id "
////		"INNER JOIN sys.tables tab1 "
////		"ON tab1.object_id = fkc.parent_object_id "
////		"INNER JOIN sys.schemas sch "
////		"ON tab1.schema_id = sch.schema_id "
////		"INNER JOIN sys.columns col1 "
////		"ON col1.column_id = parent_column_id AND col1.object_id = tab1.object_id "
////		"INNER JOIN sys.tables tab2 "
////		"ON tab2.object_id = fkc.referenced_object_id "
////		"INNER JOIN sys.columns col2 "
////		"ON col2.column_id = referenced_column_id AND col2.object_id = tab2.object_id "
////		"where tab1.name = '%1'";

////	QString commandText = commandTextTemplate.arg(tablanev);

////	QSqlQuery query(commandText);

////	// FK_NAME	schema_name	table	column	referenced_table	referenced_column
////	//
////	//FK_k_szallitolevel_tetel_szallitolevel1	dbo	k_szallitolevel_tetel	szallitolevel_id	szallitolevel	id
////	//FK_k_szallitolevel_tetel_szallitotetel	dbo	k_szallitolevel_tetel	szallitotetel_id	szallitotetel	id
////	//
////	// column -> referenced_table . referenced_column
////	while (query.next()) {
////		QString col = query.value("column").toString();
////		QString r_tab = query.value("referenced_table").toString();
////		QString r_col = query.value("referenced_column").toString();

////		QString tmp = "%1 -> %2.%3";
////		QString str = tmp.arg(col).arg(r_tab).arg(r_col);

////		ui.listWidget_IdegenKulcs->addItem(str);
////	}
//}

QPalette retek2::getPaletteByColor(const QPalette &r, const QColor& c){
    QPalette p = r;
    p.setColor(QPalette::Base, c);
    return p;
}

void retek2::mezoListaFeltolt(const zTable& t){
    //zTrace();
    //zInfo(t.toString());
    ui.tableWidget_MezoLista->blockSignals(true);
    ui.tableWidget_MezoLista->setRowCount(0);
    for(int r_ix=0;r_ix<t.rows.length();r_ix++)
    {
        auto r = t.rows[r_ix];
        ui.tableWidget_MezoLista->insertRow(r_ix);

        ui.tableWidget_MezoLista->setItem(r_ix, C_ix_colName,  CreateTableItem(QVariant(r.colName)));
        ui.tableWidget_MezoLista->setItem(r_ix, C_ix_colType,  CreateTableItem(QVariant(r.colType)));
        ui.tableWidget_MezoLista->setItem(r_ix, C_ix_dlen, CreateTableItem(QVariant(r.dlen)));
        ui.tableWidget_MezoLista->setItem(r_ix, C_ix_Caption, CreateTableItem(QVariant(r.Caption)));     
        ui.tableWidget_MezoLista->setItem(r_ix, C_ix_nullable, CreateTableItem(QVariant(r.isNullable)));
    }
    ui.lineEdit_name->setToolTip(zStringHelper::Empty);
    ui.lineEdit_pkname->setToolTip(zStringHelper::Empty);

    static auto yb = QBrush(Qt::yellow);
    static auto pki = QIcon(QStringLiteral(":/alert-triangle.ico"));
    static QAction pkia(pki, zStringHelper::Empty);
    static QPalette normalpal = ui.lineEdit_name->palette();
    static QPalette yellowpal = getPaletteByColor(normalpal, Qt::yellow);
    ui.lineEdit_name->removeAction(&pkia);
    ui.lineEdit_pkname->removeAction(&pkia);

    ui.lineEdit_name->setPalette(normalpal);
    ui.lineEdit_pkname->setPalette(normalpal);

    zforeach(a,t.eval)
    {
        if(!(a->isValid())) continue;

        if(a->rowName.isEmpty())
        { // tábla propertyre vonatkozik
            QLineEdit *w=nullptr;

            if(a->colName==QStringLiteral("name")){
                w = ui.lineEdit_name;
            }
            else if(a->colName==QStringLiteral("pkname"))
            {
                w = ui.lineEdit_pkname;
            }

            if(w)
            {
                auto ttip = w->toolTip();
                if(!ttip.isEmpty())ttip+=zStringHelper::NewLine;
                ttip+=a->toString();
                w->setToolTip(ttip);
                w->setPalette(yellowpal);
                w->addAction(&pkia, QLineEdit::LeadingPosition);
            }

        }
        else
        {
            auto rix = zTablerow::findIx(t.rows, a->rowName);
            if(rix>-1)
            {
                auto cix = ColNameIxes.value(a->colName, -1);
                if(cix>-1)
                {
                    auto i = ui.tableWidget_MezoLista->item(rix, cix);
                    i->setBackground(yb);
                    auto ttip = i->toolTip();
                    if(!ttip.isEmpty())ttip+=zStringHelper::NewLine;
                    ttip+=a->toString();
                    i->setToolTip(ttip);
                    i->setIcon(pki);
                }
                else
                {
                    zTrace();
                }
            }
        }
    }
//    auto yb = QBrush(Qt::yellow);
//    auto pki = QIcon(QStringLiteral(":/alert-triangle.ico"));
//    // todo: ha pk, ikon, ha fk, navprop, ikon, ha reversenavprop, akkor is ikon, vagy ha constraint van rajta
//    for(int r_ix=0;r_ix<t.rows.length();r_ix++){
//        auto i = ui.tableWidget_MezoLista->item(r_ix, C_ix_colName);
//        i->setBackground(yb);
//        i->setToolTip(QStringLiteral("1 macska 3 macska"));
//        i->setIcon(pki);
//    }
    ui.tableWidget_MezoLista->blockSignals(false);
}

const QMap<QString, int> retek2::ColNameIxes
{
    {QStringLiteral("colName"), C_ix_colName},
    {QStringLiteral("colType"), C_ix_colType},
    {QStringLiteral("dlen"), C_ix_dlen},
    {QStringLiteral("isNullable"), C_ix_nullable}
};
//const QMap<zTablerow::ErrCode, int> retek2::ErrCodeColIxes
//{
//    {ErrCode::noteq, QStringLiteral("noteq")},
//    {ErrCode::unknown, QStringLiteral("unknown")}
//};


void retek2::feltoltKulcsLista(zTable t) {
    //zTrace();
    //zInfo("Constraints "+t.name);

    ui.listWidget_IdegenKulcs->clear();
    //QString pkname = t.pkname();
    //if(!pkname.isEmpty())
    if(t.hasPkname())
    {
        ui.listWidget_IdegenKulcs->addItem("PK:"+t.pkname());
    }
    auto fkl = t.getFK();
    auto rfkl = t.getRFK();

    zforeach(fk, fkl)
       ui.listWidget_IdegenKulcs->addItem("FK:"+*(fk));

    zforeach(rfk, rfkl)
       ui.listWidget_IdegenKulcs->addItem("RFK:"+*(rfk));
}


QTableWidgetItem* retek2::CreateTableItem(const QVariant& v){
    auto a = new QTableWidgetItem();
    a->setData(Qt::DisplayRole, v);

    return a;
}

/*!
 \fn retek2::GenerateAll()


 generate files by selected templates
*/

const QString retek2::NOTAB = QStringLiteral("nincs tábla kiválasztva");

void retek2::GenerateNotab(){
    //TODO class initializer
    // csv col -> placeholder

     QString clp;

    if(ui.checkBox_CSVtoPlaceholder->isChecked()){
        zInfo(QStringLiteral("C# CSV->Placeholder"));

        // kell a csv file
        //
        auto csvFileName = zFileNameHelper::fileNameDialog(this, QStringLiteral("CSV"), QStringLiteral("CSV (*.csv *.txt)"));
        auto tmpFileName = zFileNameHelper::fileNameDialog(this, QStringLiteral("Template"), QStringLiteral("TXT (*.txt)"));

        QString in_csv = zTextFileHelper::load(csvFileName);
        QString in_txt = zTextFileHelper::load(tmpFileName);
        // a csv egy sorát alakítja mapra
        // b: nincs fejléc, a placeholderek #COL1, #COL2
        //QMap<QString, QString> map = zCSVHelper::RowToMap(in_csv, x);
        QString txt = zTokenizer::ReplacePlaceholders(in_csv, in_txt);

        zInfo(txt);
        auto fn = tmpFileName+"_2";//beallitasok.getModelFilename(".cs", csvFileName);
        zTextFileHelper::save(txt, fn);

        if(ui.checkBox_clipboard->isChecked())
        {
            if(!clp.isEmpty()) clp+=zStringHelper::NewLine;
            clp+=txt;
        }
    }
    if(ui.checkBox_clipboard->isChecked() && !clp.isEmpty())
    {
        QClipboard *clipboard = QGuiApplication::clipboard();
        clipboard->setText(clp);
    }
}

void retek2::GenerateAll() {
    if(ui.checkBox_CSVtoPlaceholder->isChecked())
    {
        GenerateNotab();
        return;
    }

    if (table == nullptr){
        // nem tábla műveletek
//        if(ui.checkBox_CSVtoPlaceholder->isChecked())
//        {
//            GenerateNotab();
//        }
//        else
//        {
//            zError(NOTAB);
//        }
        return;
    }

    auto class_name = zStringHelper::getclass_nameCamelCase(table->name);
    QString clp;



    //saveTablaToXML(table->tablename);
    if (ui.checkBox_XML->isChecked()) {

        zInfo(QStringLiteral("XML"));

        QString e;
        QXmlStreamWriter s(&e);
        s.setAutoFormatting(true);
        s.writeStartDocument();

        table->toXML(&s);
        s.writeEndDocument();

        zInfo(e);
        if(ui.checkBox_clipboard->isChecked())
        {
            if(!clp.isEmpty()) clp+=zStringHelper::NewLine;
            clp+=e;
        }
    }

	if (ui.checkBox_CClass->isChecked()) {
        zInfo(QStringLiteral("C# Class"));

        auto txt = generateTmp(QStringLiteral("MVC_CClass.cs"));
        zInfo(txt);
        //auto fn = zFileNameHelper::append(QDir::homePath(),beallitasok.projectdir,beallitasok.currentProjectName,table->name + ".cs");
        auto fn = zFileNameHelper::getCurrentProjectFileName(table->name + ".cs");
        zTextFileHelper::save(txt, fn);
        if(ui.checkBox_clipboard->isChecked())
        {
            if(!clp.isEmpty()) clp+=zStringHelper::NewLine;
            clp+=txt;
        }
	}

        // TODO enum from csv
    if (ui.checkBox_Enum->isChecked())
    {
        if(!table->sql_conn.isEmpty())
        {
            zInfo(QStringLiteral("Enum"));

            auto dbconn = beallitasok.getUI();
            if(dbconn.isValid())
            {
                zSQL zsql;
                if(zsql.init(dbconn)){
                    auto ed = GetEnumData(&zsql);
                    QString txt = zEnumizer::GenerateEnum(ed);

                    zInfo(txt);
                    auto fn = zFileNameHelper::getCurrentProjectFileName(table->name + "_enum.cs");
                            //append(QDir::homePath(),beallitasok.projectdir,beallitasok.currentProjectName,table->name + "_enum.cs");
                    zTextFileHelper::save(txt, fn);
                    if(ui.checkBox_clipboard->isChecked())
                    {
                        if(!clp.isEmpty()) clp+=zStringHelper::NewLine;
                        clp+=txt;
                    }
                }
                else{
                    zInfo(QStringLiteral("nincs sqlinit"));
                }
            }
            else
            {
                zInfo(QStringLiteral("nincs dbconn"));
            }
        }
        else
        {
            zInfo(QStringLiteral("nem sql típusú"));
        }
    }



    if (ui.checkBox_Entity->isChecked()) {
        zInfo(QStringLiteral("C# Entity"));

        auto txt = generateTmp(QStringLiteral("DAL_Entity.cs"));
        zInfo(txt);
        auto fn = beallitasok.getModelFilename(class_name + ".cs", QStringLiteral("Entities"));
        zTextFileHelper::save(txt, fn);

        if(ui.checkBox_clipboard->isChecked())
        {
            if(!clp.isEmpty()) clp+=zStringHelper::NewLine;
            clp+=txt;
        }
    }

	//checkBox_Context
/*	if (ui.checkBox_Context->isChecked()) {
		qDebug("Context");
		auto txt = generateContext();
		SaveAllTextToFile(&txt, getContextFilename(tablanev + "Context.cs"));
	}*/

	if (ui.checkBox_Model->isChecked()) {
        zInfo(QStringLiteral("Model"));
        auto txt = generateTmp(QStringLiteral("MVC_Model.cs"));
        //zInfo\([\w)]+\);
        zTextFileHelper::save(txt, beallitasok.getModelFilename(class_name + ".cs", QStringLiteral("Models")));
        if(ui.checkBox_clipboard->isChecked())
        {
            if(!clp.isEmpty()) clp+=zStringHelper::NewLine;
            clp+=txt;
        }
	}

	if (ui.checkBox_Meta->isChecked()) {
        zInfo(QStringLiteral("ModelMeta"));
        auto txt = generateTmp(QStringLiteral("MVC_ModelMeta.cs"));
        zTextFileHelper::save(txt, beallitasok.getModelFilename(class_name + "Meta" + ".cs", QStringLiteral("Models")));
        if(ui.checkBox_clipboard->isChecked())
        {
            if(!clp.isEmpty()) clp+=zStringHelper::NewLine;
            clp+=txt;
        }
	}

	if (ui.checkBox_Controller->isChecked()) {
        zInfo(QStringLiteral("Controller"));
        auto txt = generateTmp(QStringLiteral("MVC_Controller.cs"));
        zTextFileHelper::save(txt, beallitasok.getModelFilename(class_name + "Controller" + ".cs", QStringLiteral("Controllers")));
        if(ui.checkBox_clipboard->isChecked())
        {
            if(!clp.isEmpty()) clp+=zStringHelper::NewLine;
            clp+=txt;
        }
	}

    if (ui.checkBox_DataProvider->isChecked()) {
        zInfo(QStringLiteral("DataProvider"));
        auto txt = generateTmp(QStringLiteral("MVC_DataProvider.cs"));
        zTextFileHelper::save(txt, beallitasok.getModelFilename(class_name + "DataProvider" + ".cs", QStringLiteral("DataProviders")));
        if(ui.checkBox_clipboard->isChecked())
        {
            if(!clp.isEmpty()) clp+=zStringHelper::NewLine;
            clp+=txt;
        }
    }

    if (ui.checkBox_View->isChecked()) {
        zInfo(QStringLiteral("View"));
        auto txtIndex = generateTmp(QStringLiteral("MVC_ViewIndex.cshtml"));
        zTextFileHelper::save(txtIndex, beallitasok.getModelFilename(class_name + "ViewIndex" + ".cshtml", QStringLiteral("Views")));

        auto txtAdatlap = generateTmp(QStringLiteral("MVC_ViewAdatlapDX.cshtml"));
        zTextFileHelper::save(txtAdatlap, beallitasok.getModelFilename(class_name + "ViewAdatlapDX" + ".cshtml", QStringLiteral("Views")));

        if(ui.checkBox_clipboard->isChecked())
        {
            if(!clp.isEmpty()) clp+=zStringHelper::NewLine;
            clp+=txtIndex;
            if(!clp.isEmpty()) clp+=zStringHelper::NewLine;
            clp+=txtAdatlap;
        }
    }

//	if (ui.checkBox_ViewIndex->isChecked()) {
//        zTrace("Index");
//		auto txt = generateTmp("MVC_Index.cshtml");
//        zTextFileHelper::save(txt, beallitasok.getModelFilename(class_name + "ViewIndex" + ".cshtml", "Views"));
//	}

//	if (ui.checkBox_ViewEdit->isChecked()) {
//        zTrace("Edit");
//        auto txt = generateTmp("MVC_Edit.cshtml");
//        zTextFileHelper::save(txt, beallitasok.getModelFilename(class_name + "ViewEdit" + ".cshtml", "Views"));
//	}
/*
	if (ui.checkBox_ViewCreate->isChecked()) {
		qDebug("Create");
		auto txt = generateTmp("MVC_Create.cshtml");
		SaveAllTextToFile(&txt, getModelFilename(getOsztalynevLower(tablanev) + "ViewCreate" + ".cshtml"));
	}

	if (ui.checkBox_ViewDetails->isChecked()) {
		qDebug("Details");
		auto txt = generateTmp("MVC_Details.cshtml");
		SaveAllTextToFile(&txt, getModelFilename(getOsztalynevLower(tablanev) + "ViewDetails" + ".cshtml"));
	}

	if (ui.checkBox_ViewDetails->isChecked()) {
		qDebug("Delete");
		auto txt = generateTmp("MVC_Delete.cshtml");
		SaveAllTextToFile(&txt, getModelFilename(getOsztalynevLower(tablanev) + "ViewDelete" + ".cshtml"));

    }*/
    //else{
    //    qDebug("Nincs tabla kivalasztva!");
    //}


    if(ui.checkBox_clipboard->isChecked() && !clp.isEmpty())
    {
        QClipboard *clipboard = QGuiApplication::clipboard();
        clipboard->setText(clp);
    }
}






/*
tmp file tartalmának feldolgozása
- tokenek alapján a táblákból generált adatok beillesztése
*/

QString retek2::generateTmp(const QString& tmp_file) {
    //qDebug() << tmp_file;
    if(tmp_file.isEmpty())
    {
        zLog::dialogError(QStringLiteral("Nincs sablon fájl"));
        return zStringHelper::Empty;
    }
    auto tmp_fn = beallitasok.getTemplateFilename(tmp_file);
    if(tmp_fn == nullptr)
    {
        zLog::dialogError(QStringLiteral("A sablon fájl nem található: ")+ tmp_file);
        return zStringHelper::Empty;
    }
    QString tmp = zTextFileHelper::load(tmp_fn);   
    ztokenizer.tokenize(&tmp, nullptr, 0, beallitasok.currentProjectName);
	return tmp;
}



/*!
  Validáció - SQL validáció -
 * A 2-es tabon megadott szöveg alapján generál táblaszerkezetet
 * ami alapján validálja az adatbázisban szereplő táblát.
 *
 * Ehez meg kell adni egy adatzbázis kapcsolatot - a validálandó ztabla connectionja az
 */

void retek2::on_pushButton_2_clicked()
{        
    QString schemaName = zStringHelper::Empty;//ui.comboBox_connections->currentText();
    zSQL zsql;
    auto dbconn = beallitasok.getUI();
    if(dbconn.isValid()){
        if(zsql.init(dbconn)) // ez a szerverig - tehát a sémanév és a táblanév nyitott marad
        {
            zInfo(QStringLiteral("GenerateByText: zsql.init ok"));

            auto txt = ui.textEdit->toPlainText();
            auto tl = zTable::createTableByText(txt);
            if(tl.length()>0)
            {
                zforeach(t,tl)
                {
                    auto t_sql = zsql.getTable(schemaName, t->sql_table);
                    t_sql.initSql(dbconn.Name, schemaName, t_sql.sql_table);

                    QList<zTableError> e;
                    //auto vl =
                    t_sql.Compare(*t, e, QStringLiteral("sql_validation"));
                    zError("--- "+t->sql_table+" ---");

                    zforeach(ee, e)
                    {
                        //auto msg = (ee->message);

                        zInfo(ee->message);
                    }
                }
                zInfo(QStringLiteral("--- --- ---"));
            }
            else
            {
                zError(QStringLiteral("nincs egyezés, nincs vizsgálat"));
            }
        }
        else
        {
            zError(QStringLiteral("GenerateByText: zsql.init sikertelen"));
        }
    }
}


// osztályleíró - tagonkénti
/*
#Adm
DateCre,DateTime
DateMod,datetime


Inventory
Id,int,key,Identity,required
Name,String,30
OperationTypeId,int
InventoryStatusId,int
StorageId,int
DateStart,DateTime,nullable
DateEnd,datetime,nullable
Adm
RecountRequired,bool
UserId,guid
Comments,string,200

*/

/*
Macro def: Adm
*/
void retek2::on_pushButton_3_clicked()
{
    zTrace();

    auto txt = ui.textEdit->toPlainText();
    auto tl = zTable::createTableByText(txt);

    if(tl.isEmpty())
    {
        zInfo(QStringLiteral("Nem jött létre tábla error"));
        return;
    }

    zforeach(t,tl){
        if(zTable::find(ztables, t->name, zTableSearchBy::Name))
        {
            zInfo(QStringLiteral("Van már ilyen nevű tábla error"));
            continue;
        }
        ztables.append(*t);
        add_zTablaToListWidget(*t);
    }
}

// típusonkénti sororientált leíró
/*
#Adm
DateCre DateMod datetime

class1
mezo1 mezo2 mezo3 string
mezo4 mezo5 int
int mezo6 mezo7
Adm
*/
void retek2::on_pushButton_4_clicked()
{
    zTrace();

    auto txt = ui.textEdit->toPlainText();
    auto tl = zTable::createTableByText_2(txt);

    if(tl.length()==0)
    {
        zInfo(QStringLiteral("nem jött létre adat error"));
        return;
    }

    zforeach(t,tl)
    {
        if(zTable::find(ztables, t->name, zTableSearchBy::Name))
        {
            zInfo(QStringLiteral("Van már ilyen nevű tábla error"));
            continue;
        }
        ztables.append(*t);
        add_zTablaToListWidget(*t);
    }
}


// load from xml
void retek2::on_pushButton_5_clicked()
{
    zTrace();

    auto txt = ui.textEdit->toPlainText();
    auto tl = zTable::createTableByXML(txt);

   if(tl.length()==0) { zInfo(QStringLiteral("nem jött létre adat error")); return;}

    zforeach(t,tl){
        ztables.append(*t);
        add_zTablaToListWidget(*t);
        }
}

/*
Enumot készít a megnevezés és az id mező adatbázisban lévő értéke alapján
*/
/*
void retek2::on_pushButton_5_clicked()
{
    zTrace("on_pushButton_5_clicked");

    auto ed = GetEnumData();
    QString enumstr = zEnumizer::GenerateEnum(ed);
    zInfo\([\w)]+\);
}
*/
/*
 * A guiról felszedi az enum adatait
 * előállítja az azonosítót és a bázistípust
 * Az SQLből lekérdezi az enumerátor-listát
*/
zEnumizer::EnumSource retek2::GetEnumData(zSQL *zsql){
    zEnumizer::EnumSource eredmeny;

    auto select = ui.tableWidget_MezoLista->selectionModel();
    if(select->hasSelection()){
        auto r = select->selectedRows();
        QString fn; // name
        QString ft; // type
        int idix = 0;

        ft = ui.tableWidget_MezoLista->item(idix, C_ix_colType)->text();

        if(r.length()<1){
            zInfo(QStringLiteral("Nincs megnevezés sor kijelölve error"));
            eredmeny= { "", "", QMap<int, QString>() };
        }
        else if (r.length()>1)
        {
            QString rs = zStringHelper::Empty;
            zforeach(r1, r)
            {
                int ix = (*r1).row();
                if(!rs.isEmpty())
                {
                    rs+=',';
                }
                rs+= ui.tableWidget_MezoLista->item(ix, C_ix_colName)->text();
            }
            fn = QStringLiteral("concat_ws(\"_\", %1)").arg(rs);
        }
        else
        {
            int ix = r.first().row();
            fn = ui.tableWidget_MezoLista->item(ix, C_ix_colName)->text();
        }

        auto ms = (*zsql).getTable_SQL_ENUM(table->name, fn);
        QString cn = zStringHelper::getclass_nameCamelCase(table->name);

        eredmeny= { cn, ft, ms };
        }
    else
    {
        zInfo(QStringLiteral("Nincs sor kijelölve"));
        eredmeny= { "", "", QMap<int, QString>() };
    }
    return eredmeny;
}


//void retek2::on_pushButton_6_clicked()
//{
//    zTrace(QStringLiteral("Entitások beolvasása"));

//    auto txt = ui.textEdit->toPlainText();

//    auto sourceFileFilter = QStringList()<<QStringLiteral("*.c")<<QStringLiteral("*.cs");

//    auto txtFileList = zStringHelper::getFilePaths(txt, sourceFileFilter);

//    QList<zTable> tl;

//    if(txtFileList.isEmpty()){
//        tl = zTable::createTableByClassTxt(txt);
//    }
//    else{
//        zforeach(f, txtFileList){
//            QString f_txt = zTextFileHelper::load(*f);
//            tl << zTable::createTableByClassTxt(f_txt);
//        }
//    }

//    zforeach(t,tl){
//        ztables.append(*t);
//        add_zTablaToListWidget(*t);
//        }
//}

/*
a táblanév nem átírható, illetve a mezőnév sem, ezek rendszerszintű azonosítók
*/

void retek2::on_tableWidget_MezoLista_cellChanged(int row, int column)
{
    auto i_rowname = ui.tableWidget_MezoLista->item(row, C_ix_colName)->data(Qt::EditRole).toString();
    auto i_data = ui.tableWidget_MezoLista->item(row, column);

    auto d = i_data->data(Qt::EditRole);
    auto r = zTablerow::getByName(table->rows, i_rowname);

    switch(column){
        case C_ix_Caption:
            r->Caption = d.toString();
            break;
        case C_ix_colType:
            r->colType = d.toString();
            break;
        case C_ix_dlen:
            r->dlen = d.toInt();
            break;
        case C_ix_nullable:
            r->isNullable = d.toBool();
            break;
    //case C_ix_comment:
        default:
            auto mn = ui.tableWidget_MezoLista->horizontalHeaderItem(column)->text();
            zInfo(QStringLiteral("Nem módosítható oszlop: %1 %2").arg(mn, zLog::ERROR));
            break;

    }
}


void retek2::closeEvent (QCloseEvent * /*event*/)
{
    zforeach(t, ztables){
        t->saveTablaToXML();
    }

    auto lftxt = ui.textBrowser->toPlainText();
    if(!lftxt.isEmpty())
    {
        auto lfn = zFileNameHelper::getCurrentProjectFileName(QStringLiteral("log.txt"));
        zTextFileHelper::save(lftxt, lfn, true);
    }

}


/*
Egy tábla listából való kiválasztása esetén
a régi táblát, annak módosításait mentjük,
az új táblát beolvassuk
- nem csak caption vonatkozásában, hiszen bármely adata változhat
- ez eltérést okozhat az adatbázis és a forrás vonatkozásában is
- attól függően, hogy az adat honnan származik
*/

void retek2::on_listWidget_ztables_itemClicked(QListWidgetItem *item)
{
    auto name = item->text();
    auto t = zTable::find(ztables, name, zTableSearchBy::Name);
    table = (zTable*)(t);//(zTable*)(t);

    if(table)
    {
        zTableNamesToUi(*table);
        mezoListaFeltolt(*table);// ez töltö fel a táblának a mezőit
        feltoltKulcsLista(*table);        
    }
    else
    {
        zError(QStringLiteral("Nem található a tábla: %1").arg(name));
    }
}



void retek2::on_pushButton_projects_apply_clicked()
{    
    auto ci = ui.listWidget_projects->currentItem();

    if(ci){
        beallitasok.currentProjectName = ci->text();
        loadCurrentProject();
    }else{
        zLog::dialogError(QStringLiteral("Nincs elem kiválasztva"));
    }
}

/*
Sémák feltöltése

*/
void retek2::on_comboBox_connections_currentIndexChanged(const QString &arg1)
{
    ui.listWidget_schemas->clear();
    ui.listWidget_tables->clear();
    // a szerver conn kell - a neve alapján - tehát nem az adatbázis név, hanem a saját neve alapján
    auto c = beallitasok.getDbConnectionByName(arg1);
    if(c){
        schemasFeltolt(*c);
    }
}

/*
    források feltöltése
    comboBox_srcconn

    Egy könyvtárat adunk meg - ott a források - osztályok 1 db / fájl
*/

void retek2::on_comboBox_srcconn_currentIndexChanged(const QString &arg1)
{
    ui.listWidget_sources->clear();

    auto c = beallitasok.getSrcConnectionByName(arg1);
    if(c)
    {
        if(zTextFileHelper::isExistDirW(c->path))
        {
            ui.lineEdit_sourcepath->setText(c->path);
            ui.lineEdit_sourcepath->setToolTip(c->path);
            ui.textBrowser_sources->clear();
            sourcesFeltolt(*c);
        }
        else{
            zInfo(QStringLiteral("srcconnections.csv: No src dir: %1: %2 error").arg(c->Name).arg(c->path));
        }

    }
}

//listWidget_sources
void retek2::sourcesFeltolt(const srcConnection& c) {
    //    zSQL zsql;
    //    zsql.init(c);

    auto sourceNames = zFileNameHelper::getSourceFilenames(c.path);
    zforeach(sn, sourceNames)
    {
        auto fn = zFileNameHelper::getfileName(*sn);
        auto i = new QListWidgetItem(fn);
        //isetText(fn);
        i->setData(Qt::UserRole, *sn);
        ui.listWidget_sources->addItem(i);
    }
}

/*
    dokumentum - dokumentumosztályok feltöltése
    comboBox_docconn
    Egy dokumentumot adunk meg, ahol több dokumentum leíróját tároljuk
*/

//TODO nem futhat míg nincs típustábla
void retek2::on_comboBox_docconn_currentIndexChanged(const QString &arg1)
{
    auto c = beallitasok.getDocConnectionByName(arg1);

    //auto e =
    docRefresh(c);

}

QString retek2::docRefresh(docConnection* c)
{
    ui.listWidget_docs->clear();
    if(!c->isValid()) return zStringHelper::Empty;

    ui.lineEdit_docpath->setText(c->path);
    ui.lineEdit_docpath->setToolTip(c->path);
    ui.textBrowser_docs->clear();

    QString e = docsFeltolt(*c);
    return e;

    return zStringHelper::Empty;
}

void retek2::setEnabled(bool e)
{
    ui.comboBox_docconn->setEnabled(e);
    ui.comboBox_docconn->blockSignals(!e);

    ui.listWidget_docs->setEnabled(e);
    ui.listWidget_sources->setEnabled(e);
    ui.listWidget_schemas->setEnabled(e);
    ui.listWidget_projects->setEnabled(e);
    ui.listWidget_ztables->setEnabled(e);

    ui.listWidget_docs->blockSignals(!e);
    ui.listWidget_sources->blockSignals(!e);
    ui.listWidget_schemas->blockSignals(!e);
    ui.listWidget_projects->blockSignals(!e);
    ui.listWidget_ztables->blockSignals(!e);
}

// dokumentum fájlt adunk meg, a listboxban a tárolt osztályok listáját kell feltölteni
//listWidget_docs
QString retek2::docsFeltolt(const docConnection& c) {
    auto f_txt = zTextFileHelper::load(c.path);
    auto tbtxt = zTable::createTxtByHtml(f_txt);
    ui.textBrowser_docs->setHtml(tbtxt);
    //ui.textBrowser_docs->setSource(QUrl(c.path));
    if(f_txt.isEmpty()) return f_txt;
    auto tables = zTable::createTableByHtml(f_txt);
    zforeach(t, tables)
    {
        auto i = new QListWidgetItem(t->name);
        i->setData(Qt::UserRole, t->name);
        ui.listWidget_docs->addItem(i);
    }
    return tbtxt;
}

/*
A táblák feltöltése - egy konkrét séma tábláinak feltöltése
*/
void retek2::on_listWidget_schemas_currentTextChanged(const QString &schemaName)
{    
    ui.listWidget_tables->clear();
    QString connName = ui.comboBox_connections->currentText();
    auto c = beallitasok.getDbConnectionByName(connName);
    if(c){        
        tablesFeltolt(*c, schemaName);
    }
}



//KSzallitolevelTetel.SzallitolevelId ->
// connect button
void retek2::on_pushButton_clicked()
{
    zSQL zsql;
    auto dbconn = beallitasok.getUI();
    if(dbconn.isValid()){
        if(zsql.init(dbconn)){
            if(!beallitasok.dbConnections.contains(dbconn)){
                beallitasok.addDbConnection(dbconn);
            }
        }
        //tablaListaFeltolt();
        }
    else{
       zError(QString(QStringLiteral("Az adatbáziskapcsolat adatai hibásak: %1 driver: %2")).arg(dbconn.Name,dbconn.driver));
    }
    //zsql.createConnection();

}


//listWidget_fields
void retek2::fieldsFeltolt(const dbConnection& c, const QString& schemaName, const QString& tableName) {
    zSQL zsql;
    zsql.init(c);

    auto fieldNames = zsql.getFieldNames(schemaName, tableName);
    ui.listWidget_fields->addItems(fieldNames);
}

//listWidget_tables
void retek2::tablesFeltolt(const dbConnection& c, const QString& schemaName) {
    zSQL zsql;
    zsql.init(c);

    auto tableNames = zsql.getTableNames(schemaName);
    ui.listWidget_tables->addItems(tableNames);
    // https://stackoverflow.com/questions/27958381/how-to-place-an-icon-onto-a-qlineedit
}

//listWidget_schemas
void retek2::schemasFeltolt(const dbConnection& c) {
    zSQL zsql;
    zsql.init(c);

    auto schemaNames = zsql.getSchemaNames();
    ui.listWidget_schemas->addItems(schemaNames);
}



void retek2::add_zTablaToListWidget(QList<zTable> ts){
    zforeach(t, ts){
        add_zTablaToListWidget(*t);
    }
}

/*
import és connect - sql műveletek
*/
void retek2::on_pushButton_table_import_clicked()
{
    auto items = ui.listWidget_tables->selectedItems();
    if(items.isEmpty()) return;
    auto currentSchema = ui.listWidget_schemas->currentItem();
    if(!currentSchema) return;
    QString schemaName = currentSchema->text();

    QString connName = ui.comboBox_connections->currentText();

    auto c = beallitasok.getDbConnectionByName(connName);

    zSQL zsql;
    zsql.init(*c);

    //zLog::errorDialog("clicked: "+ tableName);

    //zTable t;
    zforeach(i,items){
        QString tableName = (*i)->text();


        QString v = "_"+zShortGuid::createNew().value;
        QString tx = tableName;
        QDialog dialog(this);
        zTableNameDialog.setupUi(&dialog);
        dialog.setModal(true);
        int isOK = true;
        //dialog.setWindowTitle(QStringliteral("A névvel már létezik tábla"));        

        QStringList fl = listWidgetItemsText(ui.listWidget_fields->selectedItems());

        zTable t = zsql.getTable(schemaName, tableName, fl);
        t.initSql(connName, schemaName, tableName);

        QString classNamePlural;
        auto className = zTable::getClassName(tableName, classNamePlural);
        t.initClass(className, classNamePlural);

        if(t.rows.length()>0){
            while(isOK && zTable::find(ztables, tx, zTableSearchBy::TableName)) {
                zTableNameDialog.lineEdit_name->setText(tableName + v);
                isOK = dialog.exec();
                if(isOK){
                    tx = zTableNameDialog.lineEdit_name->text();
                    }
                }
            if(isOK){
               t.name = tx;
               ztables << t;
               add_zTablaToListWidget(t);
            }
            else{
                continue;
            }
        }
    }
}

/*
A tábla honnan jön, ez meglevő tábla illetve egy adatbázis tábla - kell-e névegyezés
*/

void retek2::on_pushButton_table_connect_clicked()
{
    if(table == nullptr) return; // kell egy belső tábla
    auto items = ui.listWidget_tables->selectedItems();
    if(items.isEmpty()) return; // kell egy, és pontosan egy külső tábla
    if(items.count()>1) return;
    // ha megvannak, bejegyezzük a kapcsolatot
    auto currentSchema = ui.listWidget_schemas->currentItem();
    if(!currentSchema) return;
    QString schemaName = currentSchema->text();
    QString connName = ui.comboBox_connections->currentText();
    QString tableName = (items.first())->text();

    table->initSql(connName, schemaName, tableName);
}

QStringList retek2::listWidgetItemsText(QList<QListWidgetItem*> items)
{
    QStringList e;

    zforeach(i, items)
    {
        e<<(*i)->text();
    }

    return e;
}




//void retek2::on_buttonBox_clicked(QAbstractButton *button)
//{
//    auto tx = zTableNameDialog.lineEdit_name->text();
//    if(!zTable::find(&ztables, tx, zTableSearchBy::TableName)){
//        //zTableNameDialog.
//        zlog.log("létezik");
//    }
//}


//void retek2::on_buttonBox_accepted()
//{
//    zLog::errorDialog("aaaa");
//}

//void retek2::on_buttonBox_clicked(QAbstractButton *button)
//{
//    zLog::errorDialog("bbb");
//}

void retek2::on_pushButton_GenerateAll_clicked()
{
    GenerateAll();
}

void retek2::zTableNamesToUi(const zTable& t){
    ui.lineEdit_name->setText(t.name);
    ui.lineEdit_tablename->setText(t.sql_table);
    ui.lineEdit_class_name->setText(t.class_name);
    ui.lineEdit_class_name_plural->setText(t.class_name_plural);
    ui.lineEdit_pkname->setText(t.pkname());
    ui.lineEdit_src_src->setText(t.class_path);
    ui.lineEdit_docName->setText(t.docName);
    ui.lineEdit_src_doc->setText(t.document_path);

    ui.lineEdit_src_sql->setText(t.SqlConnToString());
    ui.lineEdit_XMLPath->setText(t.XMLPath);
}

void retek2::zTableNamesFromUi(zTable& t){
    QString name = ui.lineEdit_name->text();

    if(!name.isEmpty()){
        if(name != t.name){
            auto i = ui.listWidget_ztables->findItems(t.name, Qt::MatchExactly);
            if(!i.isEmpty()){                                
                i[0]->setText(name);
                QString fn_old = zFileNameHelper::getCurrentProjectFileName(t.name+".xml");
                t.name = name; // megtaláltuk, innentől kezdve az új néven lesz
                QString fn_new = zFileNameHelper::getCurrentProjectFileName(t.name+".xml");
                QFile file(fn_old);
                if(file.exists())
                {
                    file.rename(fn_new);
                }
                else
                {
                    t.saveTablaToXML();
                }
            }

        }

        //name = ui.lineEdit_tablename->text();
        t.class_name = ui.lineEdit_class_name->text();
        t.class_name_plural = ui.lineEdit_class_name_plural->text();
    }
}

//void retek2::on_buttonBox_tableNames_clicked(QAbstractButton *button)
//{
//    auto a = qobject_cast<QDialogButtonBox*>(button->parent());

//    if(button == a->button(QDialogButtonBox::Apply)){
//        zTableNamesFromUi(*table);
//    }else if(button == a->button(QDialogButtonBox::Cancel)){
//        zTableNamesToUi(*table);
//    }
//}

/*
A kiválasztott sor captionját szerzi meg
*/
void retek2::on_pushButton_getCaption_clicked()
{
    auto selectedItems = ui.tableWidget_MezoLista->selectedItems();
    if(!selectedItems.isEmpty())
    {      
        int row_ix = 0;
        QString caption;

        zforeach(i,selectedItems)
        {
            if((*i)->column()==C_ix_Caption){
                row_ix = (*i)->row();
                caption = getCaptionByRowIx(row_ix);
                if(!caption.isEmpty())
                {
                    setCaptionByRowIx(row_ix, caption);
                }
            }
        }
    }    
}

QString retek2::getCaptionByRowIx(int idix)
{
    QString caption;// = ui.tableWidget_MezoLista->item(idix, C_ix_Caption)->text();
    //if(caption.isEmpty())
    //{
        QString colName = ui.tableWidget_MezoLista->item(idix, C_ix_colName)->text().toLower();
        //caption = zConversionMap::external(globalCaptionMaps, colName);
        caption = zTable::getCaption(colName);
        if(caption.isEmpty())
        {
            zWarning(QStringLiteral("Nem határozható meg felirat a mezőnévhez: %1").arg(colName));
        }
    //}
    return caption;
}

void retek2::setCaptionByRowIx(int idix, const QString& caption)
{
    ui.tableWidget_MezoLista->item(idix, C_ix_Caption)->setText(caption);
}

void retek2::on_listWidget_tables_itemClicked(QListWidgetItem *item)
{
    //zLog::dialogTrace("ez: "+item->text());
    auto tableName = item->text();
    QString schemaName = ui.listWidget_schemas->currentItem()->text();

    ui.listWidget_fields->clear();
    QString connName = ui.comboBox_connections->currentText();
    auto c = beallitasok.getDbConnectionByName(connName);
    if(c){
        fieldsFeltolt(*c, schemaName, tableName);
        //tablesFeltolt(*c, schemaName);
    }
}

///log
/// zlog.init(ui.textBrowser, ui.tabWidget, 4, false);// 4.tab-on van a log
void retek2::logToGUI(ErrLevels errlevel, const QString &msg, const QString &loci, const QString &st, void* uiptr)
{

    auto ui2 = reinterpret_cast<Ui_retek2Class*>(uiptr); // NOLINT
    auto widget2 = ui2->textBrowser;
    auto tabindex2 = 3;
    auto tabwidget2= ui2->tabWidget;
    auto level = zLog::ErrLevelNames[errlevel];//zLog::LevelToString(errlevel);
    auto c = widget2->textColor();

    switch(errlevel){
    case ErrLevels::ERROR:
        widget2->setTextColor(QColor(Qt::darkRed));
        tabwidget2->setCurrentIndex(tabindex2);
        widget2->append(level+": "+msg);
        widget2->append(loci);
        widget2->append(st);
        break;
    case ErrLevels::WARNING:
        widget2->setTextColor(QColor(Qt::darkYellow));
        tabwidget2->setCurrentIndex(tabindex2);
        widget2->append(level+": "+msg);
        widget2->append(loci);
        break;
    case ErrLevels::TRACE:
        widget2->setTextColor(QColor("steelblue"));
        widget2->append(level+": "+loci);
        break;
    case ErrLevels::DEBUG:
        widget2->setTextColor(QColor(Qt::darkRed));
        tabwidget2->setCurrentIndex(tabindex2);
        widget2->append(level);
        widget2->append(loci);
        widget2->append(st);
        break;
    case ErrLevels::INFOAPPEND:
    {
        if(msg.isEmpty()) break;
        auto cursor = widget2->textCursor();
        auto r = QRegExp(QStringLiteral("\\b%1\\b").arg(loci));
        widget2->moveCursor(QTextCursor::End);//
        auto m = widget2->find(r,QTextDocument::FindBackward | QTextDocument::FindCaseSensitively);//
        if(m)
        {
            //auto a =
            widget2->find(QRegExp("$"), QTextDocument::FindCaseSensitively);
            widget2->setTextColor(getLogColor(msg));
            QTextCursor cursor2 = widget2->textCursor();
            //cursor2.removeSelectedText();
            cursor2.insertText(' '+msg);
            //widget2->insertPlainText(' ' + msg);
        }
            //auto a = widget2->find(QRegExp(R"(\b\.\.\.\b)"), QTextDocument::FindBackward |QTextDocument::FindCaseSensitively);
            //auto a = widget2->find(QRegExp("\u2029|\r\n|\r|\n"), QTextDocument::FindBackward |QTextDocument::FindCaseSensitively);
            //auto cursor2 = widget2->textCursor();
            //widget2->moveCursor(QTextCursor::EndOfLine);
            //QTextCursor::move(QTextCursor::EndOfLine);


        widget2->setTextCursor(cursor);
        if(isLogFocus(msg)) tabwidget2->setCurrentIndex(tabindex2);
        break;
    }
    case ErrLevels::INFOCLOSE:
    {
        auto cursor = widget2->textCursor();
        auto r = QRegExp(QStringLiteral("\\b%1\\b").arg(loci));
        widget2->moveCursor(QTextCursor::End);
        //auto m =
        widget2->find(r, QTextDocument::FindBackward | QTextDocument::FindCaseSensitively);//
        QTextCursor cursor2 = widget2->textCursor();
        cursor2.removeSelectedText();
        cursor2.deleteChar();
        widget2->setTextCursor(cursor);
        break;
    }
    case ErrLevels::INFO:
    {
        widget2->setTextColor(getLogColor(msg));
        if(isLogFocus(msg)) tabwidget2->setCurrentIndex(tabindex2);
        widget2->append(msg);
        break;
    }
    default:
        widget2->setTextColor(QColor(Qt::black));
        break;
    }

    widget2->setTextColor(c);
}

bool retek2::isLogFocus(const QString &msg){
    return msg.endsWith(zLog::ERROR)||msg.endsWith(zLog::FAILED);
}

QColor retek2::getLogColor(const QString &msg)
{
    if (msg.endsWith(zLog::OK)) return Qt::darkGreen;
    if (msg.endsWith(zLog::WARNING)) return Qt::darkYellow;
    if (msg.endsWith(zLog::ERROR)) return Qt::darkRed;
    if (msg.endsWith(zLog::FAILED)) return Qt::darkRed;
    return Qt::darkGray;
}

void retek2::on_pushButton_sourcepath_clicked()
{
    auto homepath = QDir::homePath();
    auto d = QFileDialog::getExistingDirectory(nullptr, QStringLiteral("Select Source Folder"), homepath);

    QFileInfo fileInfo(d);
    QString filename(fileInfo.fileName());

    srcConnection c(filename, d);

    if(c.isValid())
    {
        beallitasok.addSrcConnection(c, true);
    }
}



void retek2::on_listWidget_sources_itemClicked(QListWidgetItem *item)
{    
    auto d = item->data(Qt::UserRole);
    if(!d.isValid()) return;
    auto a = d.toString();
    auto txt = zTextFileHelper::load(a);
    if(txt.isEmpty()) return;
    ui.textBrowser_sources->setText(txt);
    auto str = QStringLiteral("class\\s+%1").arg(item->text());
    TextBrowserSearch(ui.textBrowser_sources, str);
}



void retek2::on_listWidget_docs_itemClicked(QListWidgetItem *item)
{
    auto d = item->data(Qt::UserRole);
    if(!d.isValid()) return;
    auto a = d.toString();
    TextBrowserSearch(ui.textBrowser_docs, a);
}

bool retek2::TextBrowserSearch(QTextBrowser *tb, const QString& a)
{
    auto r = QRegExp(QStringLiteral("\\b%1\\b").arg(a));
    tb->moveCursor(QTextCursor::End);
    return tb->find(r, QTextDocument::FindBackward| QTextDocument::FindCaseSensitively);
}

/*pushButtons*/

/*
pushButton_srcimport - on_pushButton_table_import_clicked
*/

void retek2::on_pushButton_srcimport_clicked()
{
    zTrace();

    auto currentSrc = ui.listWidget_sources->currentItem();
    if(!currentSrc) return;
    QString srcName = currentSrc->data(Qt::UserRole).toString();

    auto name = currentSrc->text();
    //zLog::dialogTrace(srcName);

    QString f_txt = zTextFileHelper::load(srcName);
    auto tl = zTable::createTableByClassTxt(f_txt);


    zforeach(t,tl)
    {
        t->name = zStringHelper::zNormalize(name);
        t->class_path = srcName;
        ztables.append(*t);
        add_zTablaToListWidget(*t);
    }
}

/*
 *  dokumentum - import
közben a dokumentum - aháttérben módosulhatott
- ha elérhető még:
- frissíteni (az osztálylistát és a szerkesztőt is - funkcióban) a combo eventet is így kell átírni
- ha tartalmazza az osztályt,
- akkor frissíteni
*/
void retek2::on_pushButton_docimport_clicked()
{
    zTrace();

    auto docName = ui.comboBox_docconn->currentText();

    auto currentDoc = ui.listWidget_docs->currentItem();
    if(!currentDoc) return;

    auto d = currentDoc->data(Qt::UserRole).toString();
    if(d.isEmpty()) return;
    //auto name = d.toString();

    zInfo(docName + ":" + d);//CGCStock
    auto c = beallitasok.getDocConnectionByName(docName);

    if(!c->isValid()) return;

    auto f_txt = docRefresh(c);

    auto items = ui.listWidget_docs->findItems(d, Qt::MatchExactly);

    if(items.isEmpty()) return;
    if(items.count()>1){
        zInfo(QStringLiteral("Több található: %1").arg(d));
    }
    ui.listWidget_docs->setCurrentItem(items[0]);
    on_listWidget_docs_itemClicked(items[0]);

    //auto f_txt = ui.textBrowser_docs->toHtml();

//    QString f_txt = zTextFileHelper::load(srcName);
    auto tl = zTable::createTableByHtml(f_txt, d);

    if(tl.isEmpty()) return;
    if(tl.count()>1) zInfo(QStringLiteral("egynél több %1 van a dokumentációban").arg(d));

    zforeach(t,tl)
    {
        auto tn = zStringHelper::zNormalize(d);
        t->name = tn;
        t->docName = d;
        t->document_path = c->path;
        QString pluralClassName;
        auto className = zTable::getClassName(tn, pluralClassName);
        t->initClass(className, pluralClassName);
        ztables.append(*t);
        add_zTablaToListWidget(*t);
        zInfo(QStringLiteral("doc:%1").arg(tn));
    }
}


/*
C:\Users\horva\retek2\munka_dir\wiki1\CGCStock.Data\Entity

TODO

Not Connected: Driver not loaded Driver not loaded
Error: init failed: wiki1(127.0.0.1)
..\retek2\zsql.cpp:44 bool __cdecl zSQL::init(const class QString &,const class QString &,const class QString &,const class QString &,const class QString &)
stack trace:
?
Error: getDb: db closedQMYSQL
..\retek2\zsql.cpp:429 class QStringList __cdecl zSQL::getSchemaNames(void)
stack trace:
?

zInfo -> shortguid kulcsos loggolás

-eJ95G9tTN6e0LkqapZCaA Beolvasás: C:/Users/horva/retek2/munka_dir/wiki1/.xml
Warning: Nincs név: class_name:Fue (.xml)
..\retek2\retek2.cpp:310 void __cdecl retek2::loadCurrentProject(void)

nem menthető: fue.xml
*/

void retek2::on_pushButton_validate_clicked()
{
    //zTrace();
    QMap<QString,bool> e;
    QMap<QString,bool> sqlmap;
    QMap<QString,bool> srcmap;
    QMap<QString,bool> valmap;
    QMap<QString,bool> docmap;
    table->eval.clear();

//    zforeach(ee, table->eval)
//    {
//        if(ee->source==zTable::VALIDATETABLEKEY) table->eval.erase(ee);
//        else if(ee->source==zTable::VALIDATESQLKEY) table->eval.erase(ee);
//        else if(ee->source==zTable::VALIDATEDOCKEY) table->eval.erase(ee);
//        else if(ee->source==zTable::VALIDATESRCKEY) table->eval.erase(ee);
//    }

    validateTable(*table, valmap);
    validateTableSQL(*table, sqlmap);
    validateTableDocument(*table, docmap);
    validateTableSource(*table, srcmap);

    setZTablesItem(*table, sqlmap, srcmap, docmap, valmap);
    mezoListaFeltolt(*table);
}

void retek2::on_lineEdit_pkname_editingFinished()
{
    auto a = ui.lineEdit_pkname->text();
    if(a.isEmpty())
    {
        table->pkrowix = -1;
        return;
    }
    int ix = zTablerow::findIx(table->rows, a);
    if(ix>-1) table->pkrowix=ix; else ui.lineEdit_pkname->setText(table->pkname());
}

void retek2::on_pushButton_setPK_clicked()
{    
    auto selectedItems = ui.tableWidget_MezoLista->selectedItems();
    if(selectedItems.isEmpty()) return;
    if(selectedItems.count()>1) return;

    auto rowix = selectedItems[0]->row();
    table->pkrowix = rowix;
    auto pkname = ui.tableWidget_MezoLista->item(rowix, C_ix_colName)->text();
    ui.lineEdit_pkname->setText(pkname);
}



void retek2::on_lineEdit_name_editingFinished()
{
    auto tn = ui.lineEdit_name->text();
    if(!setTableName(tn))
    {
        ui.lineEdit_name->setText(table->name);
    }
}

bool retek2::setTableName(const QString &tn)
{
    if(tn.isEmpty()) return false;
    // megállapítjuk, jó lesz-e , egyedi -e
    auto item = getTablesItem(tn);
    if(item) return false;

    //QString a = table->name;
    item = getTablesItem(table->name);
    if(!item) return false;

    //a fájlt is át kell nevezni
    QString oldname = table->XMLPath;//zFileNameHelper::getCurrentProjectFileName(table->name + ".xml");
    QString newname = zFileNameHelper::getCurrentProjectFileName(tn + ".xml");
    if(! QFile::rename(oldname, newname)) return false;

    table->XMLPath = newname;
    table->name = tn;
    item->setText(tn);
    return true;
}
