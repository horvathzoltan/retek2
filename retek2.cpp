#include "globals.h"
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

#include <QRegularExpression>
#include <QTableWidgetItem>

#include <QFileInfo>

#include <QListWidget>
#include <QMessageBox>
#include <QSqlError>

#include <QDir>
#include <QIcon>

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


//retek2::retek2(QWidget *parent):QMainWindow(parent)
//{
//	ui.setupUi(this);
//}

retek2::~retek2()= default;

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
    ui.listWidget_ztables->setIconSize(QSize(48,24));
    //zlog.init(ui.textBrowser, ui.tabWidget, 4, false);// 4.tab-on van a log
    zLog::init(retek2::logToGUI, false, &ui, false);

    /*
    zError("a");
    zWarning("b");
    zInfo("c");
    zDebug();
    zTrace();
*/
    //asm("int $3");

    //zTrace("a");

    //beallitasok.init(ui.lineEdit_User, ui.lineEdit_Password, ui.lineEdit_Server, ui.lineEdit_Catalog, ui.comboBox_connections, ui.comboBox, ui.listWidget_projects);
    beallitasok.init(
                ui.lineEdit_User,
                ui.lineEdit_Password,
                ui.lineEdit_Server,
                ui.comboBox_connections,
                ui.comboBox,
                ui.listWidget_projects,
                ui.comboBox_srcconn
                );
//    beallitasok.initPaths();

    zosHelper::setLocale();

// sql-> osztály irány: Money -> decimal_money -> decimal
// osztály-> sql irány: decimal -> decimal??? -> Money
// itt a probléma az, hogy látjuk, hogy definíció szerint decimal, és tudhatjuk, hogy sql irányban money - de    - honnan tudjuk, hogy ez az adattípus pont money, ha nem ismerjük az sql-t?
// illetve kell egy belső típus, ami többet tud az adatról, mint amennyi triviális
// itt a neve utal rá, vagy ismerem az sql-t és együtt tudom vizsgálni
// illetve - ez beolvasáskor kiderül, hogy decimal
//
// az sql típusától is függ, hogy milyen beolvasás táblát használunk
//
// ennek szerepe van a perzisztens tárolás kialakításakor, illetve     az ui-n nyilván az altípsutól függő controlt kell feltenni, validációt alkalmazni

    beallitasok.load();

    auto sp = zFileNameHelper::getSettingsDir();
    auto pp = zFileNameHelper::getProjectDir();

    // magát a mapot úgy kell beolvasni ahogy van -
    // amire keresünk az legyen case insensitiv, vagy normalizált

    // Mezőmegnevezés
    globalCaptionMaps = zConversionMap::loadAll(sp, zFileNameHelper::captionFileFilter);
    projectCaptionMaps = zConversionMap::loadAll(pp, zFileNameHelper::captionFileFilter);
    // típuskonverzió
    globalSqlMaps= zConversionMap::loadAll(sp, zFileNameHelper::sqlmapFileFilter);
    globalClassMaps= zConversionMap::loadAll(sp, zFileNameHelper::classmapFileFilter);

    auto projectdirs = zFileNameHelper::GetSubdirs(pp);
    beallitasok.fillProjectList(projectdirs);


    loadCurrentProject(); // ez tölti a ztablakat XML-ből

    fillListWidgetByCurrentProject();

    auto sqlmap = validateCurrentProject_SQL();
    auto srcmap = validateCurrentProject_Source();
    auto docmap = validateCurrentProject_Document();

    setListWidgetIconsByCurrentProject(sqlmap, srcmap, docmap);


    ztokenizer.init(ui.tableWidget_MezoLista);

    // TODO: konzisztencia sql és entity vonatkozásokban is
    // a megjelenítésnél ikonokat kell használni , ha a sql közés ok, ha a forrás kötés ok, illetve ha mindkettő megvan
    //
    // - xml beolvasás után a forrást ellenőrízni
    //
    // - ha az sql tábla frissebb, akkor frissíteni - illetve detektálni és a listában piros háttérrel jelezni
    // illetve, ha ez a változás érdemi - ekkro be lehet olvasni -  és van tábla validáció, ami összeveti
    // - ha az entitás fájl frissebb, akkor hasonlóképpen - lehet validálni
    //
    // azt kellene inkább vizsgálni: hogy ha a leíró sql vagy kód kötése megvan-e
    // forrás típus alapján az sql illetve entity - egyértelmű, hiszen származik belőle
    // és ha az sql vagy forrás módosult, míg a leíró nem, azaz a leíró keletkezése óta újabb az sql
    // ezt detektálni és jelezni kell, illetve kérdés, hogy felszedjük-e a változásokat
    // továbbá
    // hogy a leíró honnan származik, és az érvényes-e még a származás miatt egyértelmű, hogy annak kell előbb lennie
    //
    // egy általánosabb probléma, hogy
    // ha van forrás (az nem txt) fedi-e a leírót
    // ez nem függ dátumtól, bár származás esetén nem irreleváns, de
    // egymást nem fedő esetet lejet jóra hozni javítás által, mindkét oldalon -
    // ha a leíró módosult, míg a forrás nem, bár ez em egyértelmű mert az sql simán lehet régebbi, mint a leíró
    // elvileg ezt is jelezni kellene,

    zInfo(QStringLiteral("retek2 init ok"));
}

void retek2::fillListWidgetByCurrentProject()
{
    //zTrace();
    zforeach(t, ztables)
    {
        add_zTablaToListWidget(*t);
    }
}

void retek2::setListWidgetIconsByCurrentProject(const QMap<QString, bool>& sqlmap, const QMap<QString, bool> &srcmap, const QMap<QString, bool>& docmap)
{
    zforeach(t, ztables)
    {
        auto items = ui.listWidget_ztables->findItems(t->name, Qt::MatchExactly);
        if(items.isEmpty()) continue;
        if(items.count()==1)
        {
            QStringList icons = getIconsByFlags(t->name, sqlmap, srcmap, docmap);
            auto icon = zIconHelper::concatenate(icons);
            items[0]->setIcon(icon);
        }
        else
        {
            zError(QStringLiteral("Több tábla található azonos néven: %1").arg(t->name));
        }
    }
}


QStringList retek2::getIconsByFlags(const QString& name, const QMap<QString, bool> &sqlmap, const QMap<QString, bool> &srcmap, const QMap<QString, bool> &docmap)
{
    QStringList e;


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
            zError(QStringLiteral("Nincs aktuális project. ERROR").arg(beallitasok.currentProjectName));
    }
    else
    {
        ztables.clear();
        ui.listWidget_ztables->clear();

        QString currentProjectPath = zFileNameHelper::getCurrentProjectDir();
        QStringList files = zFileNameHelper::FindFileNameInDir(currentProjectPath, QString(), zFileNameHelper::xmlFilter);
        zforeach(f, files){
            auto txt = zTextFileHelper::load(*f);

            auto t = zTable::createTableByXML(txt);
            if(t.isEmpty() )
            {
                zError(QStringLiteral("Nincs tábla: %1").arg(*f));
            }
            else
            {
                if(t.count()>1)
                {
                    zError(QStringLiteral("Több tábla: %1").arg(*f));
                }
                else
                {
                    auto t0 = t[0];

                    // ha nincs név, lesz
                    if(t0.name.isEmpty())
                    {
                        QString fn = zFileNameHelper::getfileName(*f);
                        zWarning(QStringLiteral("Nincs név: %1 (.xml)").arg(fn));
                        t0.name = fn;
                    }
                    bool isValid = t0.Validate(ztables);

//                    if(zTable::find(ztables, t0.name, zTableSearchBy::Name))
//                    {
//                        QString fn = zFileNameHelper::getfileName(*f);
//                        zError(QStringLiteral("Már van tábla ilyen néven: %1, %2 (.xml)").arg(t0.name, fn));
//                    }
//                    else
                    ztables << t0;
//                    add_zTablaToListWidget(t0);

                    if(!isValid)
                    {
                        zError(QStringLiteral("A tábla nem valid: %1").arg(t0.name));
                    }
                }
            }
        }
    }
}

// TODO if(true ||  - nem kell az élesben
QMap<QString,bool> retek2::validateCurrentProject_SQL(){
    //const char* a = (const char*)Q_FUNC_INFO;//zfn();
    zTrace();
    QMap<QString,bool> e;

    zforeach(t, ztables)
    {
        //zTrace(QStringLiteral("sql validating table: %1").arg(t->name));
        if(t->sql_conn.isEmpty()) continue;

        if(t->sql_updateTimeStamp.isNull())
        {
            zInfo(QStringLiteral("no sql_updateTimeStamp"));
        }

        QDateTime lastUpdateTimeStamp = t->getSqlUpdateTimestamp();
        if(true || lastUpdateTimeStamp > t->sql_updateTimeStamp) // ha az sql újabb
        {            
            zInfo(QStringLiteral("newer available: %1 at: %2").arg(t->name, t->sql_updateTimeStamp.toString()));
            auto isValid = t->validateSQL();
            e.insert(t->name, isValid);
            zInfo(QStringLiteral("isValid: %1").arg(zStringHelper::boolToString(isValid)));
        } // egyébként nincs változás
        else
        {
            zInfo(QStringLiteral("not changed."));
        }

    }

    return e;
}

// TODO if(true ||  - nem kell az élesben
QMap<QString, bool> retek2::validateCurrentProject_Source(){
    QMap<QString, bool> e;
    zforeach(t, ztables)
    {        
        if(t->class_path.isEmpty()) continue;

        if(t->source_updateTimeStamp.isNull())
        {
            zInfo(QStringLiteral("no source_updateTimeStamp"));
        }

        QDateTime lastUpdateTimeStamp = t->getSourceUpdateTimestamp();

        if(true || lastUpdateTimeStamp > t->source_updateTimeStamp) // ha a forrás újabb
        {
            auto a = t->validateSource();
            e.insert(t->name, a);
        }
        else
        {
            zInfo(QStringLiteral("not changed."));
        }
    }
    return e;
}

// TODO if(true ||  - nem kell az élesben
QMap<QString, bool> retek2::validateCurrentProject_Document(){
    QMap<QString, bool>e;

    zforeach(t, ztables)
    {
        if(t->document_path.isEmpty()) continue;

        if(t->document_updateTimeStamp.isNull())
        {
            zInfo(QStringLiteral("no document_updateTimeStamp"));
        }

        QDateTime lastUpdateTimeStamp = t->getDocUpdateTimestamp();

        if(true || lastUpdateTimeStamp > t->document_updateTimeStamp) // ha a leírás újabb
        {
            auto a = t->validateDocument();
            e.insert(t->name, a);
        }
        else
        {
            zInfo(QStringLiteral("not changed."));
        }
    }

    return e;
}

void retek2::add_zTablaToListWidget(const zTable& t){
   // QString tn = t.name; // displayed name
    if(t.name.isEmpty())
    {
        zError(QStringLiteral("Nincs megnevezés. table: %1 class: %2").arg(t.sql_table, t.class_name));
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



void retek2::mezoListaFeltolt(const zTable& t){
    zTrace();
    zInfo(t.toString());
    ui.tableWidget_MezoLista->blockSignals(true);
    ui.tableWidget_MezoLista->setRowCount(0);
    for(int r_ix=0;r_ix<t.rows.length();r_ix++){
        auto r = t.rows[r_ix];
        ui.tableWidget_MezoLista->insertRow(r_ix);

        ui.tableWidget_MezoLista->setItem(r_ix, C_ix_colName,  CreateTableItem(QVariant(r.colName)));
        ui.tableWidget_MezoLista->setItem(r_ix, C_ix_colType,  CreateTableItem(QVariant(r.colType)));
        ui.tableWidget_MezoLista->setItem(r_ix, C_ix_dlen, CreateTableItem(QVariant(r.dlen)));
        ui.tableWidget_MezoLista->setItem(r_ix, C_ix_Caption, CreateTableItem(QVariant(r.Caption)));     
        ui.tableWidget_MezoLista->setItem(r_ix, C_ix_nullable, CreateTableItem(QVariant(r.isNullable)));
    }    
    ui.tableWidget_MezoLista->blockSignals(false);
}

void retek2::feltoltKulcsLista(zTable t) {
    //zTrace();
    zInfo("Constraints "+t.name);

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
void retek2::GenerateAll() {        
    if (table == nullptr){
        zError(QStringLiteral("nincs tábla kiválasztva"));
        return;
    }

    auto class_name = zStringHelper::getclass_nameCamelCase(table->name);

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
    }

	if (ui.checkBox_CClass->isChecked()) {
        zInfo(QStringLiteral("C# Class"));

        auto txt = generateTmp(QStringLiteral("MVC_CClass.cs"));
        zInfo(txt);
        //auto fn = zFileNameHelper::append(QDir::homePath(),beallitasok.projectdir,beallitasok.currentProjectName,table->name + ".cs");
        auto fn = zFileNameHelper::getCurrentProjectFileName(table->name + ".cs");
        zTextFileHelper::save(txt, fn);
	}

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
	}

	if (ui.checkBox_Meta->isChecked()) {
        zInfo(QStringLiteral("ModelMeta"));
        auto txt = generateTmp(QStringLiteral("MVC_ModelMeta.cs"));
        zTextFileHelper::save(txt, beallitasok.getModelFilename(class_name + "Meta" + ".cs", QStringLiteral("Models")));
	}

	if (ui.checkBox_Controller->isChecked()) {
        zInfo(QStringLiteral("Controller"));
        auto txt = generateTmp(QStringLiteral("MVC_Controller.cs"));
        zTextFileHelper::save(txt, beallitasok.getModelFilename(class_name + "Controller" + ".cs", QStringLiteral("Controllers")));
	}

    if (ui.checkBox_DataProvider->isChecked()) {
        zInfo(QStringLiteral("DataProvider"));
        auto txt = generateTmp(QStringLiteral("MVC_DataProvider.cs"));
        zTextFileHelper::save(txt, beallitasok.getModelFilename(class_name + "DataProvider" + ".cs", QStringLiteral("DataProviders")));
    }

    if (ui.checkBox_View->isChecked()) {
        zInfo(QStringLiteral("View"));
        auto txtIndex = generateTmp(QStringLiteral("MVC_ViewIndex.cshtml"));
        zTextFileHelper::save(txtIndex, beallitasok.getModelFilename(class_name + "ViewIndex" + ".cshtml", QStringLiteral("Views")));

        auto txtAdatlap = generateTmp(QStringLiteral("MVC_ViewAdatlapDX.cshtml"));
        zTextFileHelper::save(txtAdatlap, beallitasok.getModelFilename(class_name + "ViewAdatlapDX" + ".cshtml", QStringLiteral("Views")));
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
    // a ztablesbe bele kell tenni az sql connectiont, séma és tábla szinten -
    // amikor egy ztable kötődik egy sql-hez, tudjuk megtenni,
    // - vagy mert onnan olvastuk be
    // - vagy mert oda írjuk ki (generálunk create scriptet?)

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

                    QStringList e;
                    auto vl = t_sql.Compare(*t, e);
                    zError("--- "+t->sql_table+" ---");

                    zforeach(ee, e)
                    {
                        zInfo(*ee);
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
Id,int,key,Identity
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
        zError(QStringLiteral("Nem jött létre tábla"));
    }

    zforeach(t,tl){
        if(zTable::find(ztables, t->name, zTableSearchBy::Name))
        {
            zError(QStringLiteral("Van már ilyen nevű tábla"));
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
        zError(QStringLiteral("nem jött létre adat"));
        return;
    }

    zforeach(t,tl)
    {
        if(zTable::find(ztables, t->name, zTableSearchBy::Name))
        {
            zError(QStringLiteral("Van már ilyen nevű tábla"));
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

   if(tl.length()==0) { zError(QStringLiteral("nem jött létre adat")); return;}

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
            zInfo(QStringLiteral("Nincs megnevezés sor kijelölve"));
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
            zError(QStringLiteral("Nem módosítható oszlop: %1").arg(mn));
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
        mezoListaFeltolt(*table);
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
*/

void retek2::on_comboBox_srcconn_currentIndexChanged(const QString &arg1)
{
    ui.listWidget_sources->clear();

    auto c = beallitasok.getSrcConnectionByName(arg1);
    if(c){
        sourcesFeltolt(*c);
    }
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
                beallitasok.addConnection(dbconn);
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

}

//listWidget_schemas
void retek2::schemasFeltolt(const dbConnection& c) {
    zSQL zsql;
    zsql.init(c);

    auto schemaNames = zsql.getSchemaNames();
    ui.listWidget_schemas->addItems(schemaNames);
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

void retek2::on_buttonBox_tableNames_clicked(QAbstractButton *button)
{        
    auto a = qobject_cast<QDialogButtonBox*>(button->parent());

    if(button == a->button(QDialogButtonBox::Apply)){
        zTableNamesFromUi(*table);
    }else if(button == a->button(QDialogButtonBox::Cancel)){
        zTableNamesToUi(*table);
    }
}

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

///log
/// zlog.init(ui.textBrowser, ui.tabWidget, 4, false);// 4.tab-on van a log
void retek2::logToGUI(int errlevel, const QString &msg, const QString &loci, const QString &st, void* uiptr)
{

    auto ui2 = reinterpret_cast<Ui_retek2Class*>(uiptr); // NOLINT
    auto widget2 = ui2->textBrowser;
    auto tabindex2 = 4;
    auto tabwidget2= ui2->tabWidget;
    auto level = zLog::LevelToString(errlevel);
    auto c = widget2->textColor();

    switch(errlevel){
    case zLog::ERROR:
        widget2->setTextColor(QColor(Qt::darkRed));
        tabwidget2->setCurrentIndex(tabindex2);
        widget2->append(level+": "+msg);
        widget2->append(loci);
        widget2->append(st);
        break;
    case zLog::WARNING:
        widget2->setTextColor(QColor(Qt::darkYellow));
        tabwidget2->setCurrentIndex(tabindex2);
        widget2->append(level+": "+msg);
        widget2->append(loci);
        break;
    case zLog::TRACE:
        widget2->setTextColor(QColor("steelblue"));
        widget2->append(level+": "+loci);
        break;
    case zLog::DEBUG:
        widget2->setTextColor(QColor(Qt::darkRed));
        tabwidget2->setCurrentIndex(tabindex2);
        widget2->append(level);
        widget2->append(loci);
        widget2->append(st);
        break;
    case zLog::INFO:
        if(msg.endsWith(QStringLiteral("ok")))
        {
            widget2->setTextColor(QColor(Qt::darkGreen));
        }
        else if (msg.endsWith(QStringLiteral("warning")))
        {
            widget2->setTextColor(QColor(Qt::darkYellow));
        }
        else if (msg.endsWith(QStringLiteral("error")))
        {
            widget2->setTextColor(QColor(Qt::darkRed));
        }
        else
        {
            widget2->setTextColor(QColor(Qt::darkGray));
        }

        widget2->append(msg);
        break;
    default:
        widget2->setTextColor(QColor(Qt::black));
        break;
    }

    widget2->setTextColor(c);
}
