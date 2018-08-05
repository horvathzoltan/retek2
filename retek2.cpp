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
    zlog.init(ui.textBrowser, ui.tabWidget, 4);// 4.tab-on van a log
    //beallitasok.init(ui.lineEdit_User, ui.lineEdit_Password, ui.lineEdit_Server, ui.lineEdit_Catalog, ui.comboBox_connections, ui.comboBox, ui.listWidget_projects);
    beallitasok.init(ui.lineEdit_User, ui.lineEdit_Password, ui.lineEdit_Server,  ui.comboBox_connections, ui.comboBox, ui.listWidget_projects);
//    beallitasok.initPaths();

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

    //sql->c# irány
    typeMap.insert(QStringLiteral("uniqueidentifier"), QStringLiteral("Guid"));
    typeMap.insert(QStringLiteral("int"), QStringLiteral("int"));
    //typeMap.insert(QStringLiteral("datetime"), QStringLiteral("DateTime"));
    typeMap.insert(QStringLiteral("date"), QStringLiteral("Date"));
    typeMap.insert(QStringLiteral("nvarchar"), QStringLiteral("string"));
    typeMap.insert(QStringLiteral("nchar"), QStringLiteral("string"));
    typeMap.insert(QStringLiteral("char"), QStringLiteral("string"));
    typeMap.insert(QStringLiteral("varchar"), QStringLiteral("string"));
    typeMap.insert(QStringLiteral("float"), QStringLiteral("float"));
    typeMap.insert(QStringLiteral("bit"), QStringLiteral("bool"));
    typeMap.insert(QStringLiteral("decimal"), QStringLiteral("decimal"));
    typeMap.insert(QStringLiteral("xml"), QStringLiteral("string"));

    //c# -> sql irány
    typeMapR.insert(QStringLiteral("Guid"), QStringLiteral("uniqueidentifier"));
    typeMapR.insert(QStringLiteral("int"), QStringLiteral("int"));
    //typeMapR.insert(QStringLiteral("DateTime"), QStringLiteral("datetime"));
    typeMapR.insert(QStringLiteral("Date"), QStringLiteral("date"));
    typeMapR.insert(QStringLiteral("string"), QStringLiteral("nvarchar"));
    typeMapR.insert(QStringLiteral("float"), QStringLiteral("float"));
    typeMapR.insert(QStringLiteral("bool"), QStringLiteral("bit"));
    typeMapR.insert(QStringLiteral("decimal"), QStringLiteral("decimal"));

    beallitasok.load();

    auto sp = zFileNameHelper::getSettingsDir();
    auto pp = zFileNameHelper::getProjectDir();

    // magát a mapot úgy kell beolvasni ahogy van -
    // amire keresünk az legyen case insensitiv, vagy normalizált

    // Mezőmegnevezés
    globalCaptionMaps = zConversionMap::loadAll(sp, zFileNameHelper::captionFileFilter);
    // típuskonverzió
    globalSqlMaps= zConversionMap::loadAll(sp, zFileNameHelper::sqlmapFileFilter);
    globalClassMaps= zConversionMap::loadAll(sp, zFileNameHelper::classmapFileFilter);

    auto projectdirs = zFileNameHelper::GetSubdirs(pp);
    beallitasok.fillProjectList(projectdirs);

    loadCurrentProject();

    ztokenizer.init(ui.tableWidget_MezoLista);

    // TODO: - xml beolvasás után a forrást ellenőrízni
    // - ha az sql tábla frissebb, akkor frissíteni - illetve detektálni és a listában piros háttérrel jelezni
    // illetve, ha ez a változás érdemi - ekkro be lehet olvasni -  és van tábla validáció, ami összeveti
    // - ha az entitás fájl frissebb, akkor hasonlóképpen - lehet validálni
    //
    // azt kellene inkább vizsgálni: hogy ha a leíró sql vagy kód kötése megvan
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

    zlog.ok(QStringLiteral("retek2 init ok"));
}


void retek2::loadCurrentProject()
{      
    if(beallitasok.currentProjectName.isEmpty())
    {
            zlog.error(QStringLiteral("Nincs aktuális project. ERROR").arg(beallitasok.currentProjectName));
    }
    else
    {
        QStringList knownTypeNames;
        knownTypeNames << zConversionMap::keys(globalClassMaps) << zConversionMap::keys(globalSqlMaps);

        ztables.clear();
        ui.listWidget_ztables->clear();

        QString currentProjectPath = zFileNameHelper::getCurrentProjectDir();
        QStringList files = zFileNameHelper::FindFileNameInDir(currentProjectPath, QString(), zFileNameHelper::xmlFilter);
        zforeach(f, files){
            auto txt = zTextFileHelper::load(*f);

            auto t = zTable::createTableByXML(txt);
            if(t.isEmpty() )
            {
                zlog.error(QStringLiteral("Nincs tábla: %1").arg(*f));
            }
            else
            {
                if(t.count()>1)
                {
                    zlog.error(QStringLiteral("Több tábla: %1").arg(*f));
                }
                else
                {
                    auto t0 = t[0];

                    // ha nincs név, lesz
                    if(t0.name.isEmpty())
                    {
                        QString fn = zFileNameHelper::getfileName(*f);
                        zlog.warning(QStringLiteral("Nincs név: %1 (.xml)").arg(fn));
                        t0.name = fn;
                    }
                    bool isValid = t0.Validate(ztables, knownTypeNames);

//                    if(zTable::find(ztables, t0.name, zTableSearchBy::Name))
//                    {
//                        QString fn = zFileNameHelper::getfileName(*f);
//                        zlog.error(QStringLiteral("Már van tábla ilyen néven: %1, %2 (.xml)").arg(t0.name, fn));
//                    }
//                    else
                    ztables << t0;
                    zTablaToList(t0);

                    if(!isValid)
                    {
                        zlog.error(QStringLiteral("A tábla nem valid: %1").arg(t0.name));
                    }
                }
            }
        }
    }
}



void retek2::zTablaToList(const zTable& t){
    QString tn = t.name;
    int sourcetype = -1;
    QIcon icon;
    if(!t.sql_conn.isEmpty()){
        sourcetype = zTableSourceTypes::SQL;
    }
    else if(!t.class_path.isEmpty()){
        sourcetype = zTableSourceTypes::ENTITY;
    }
    switch(sourcetype){
        case zTableSourceTypes::SQL:
            icon=QIcon::fromTheme(QStringLiteral("office-database"));            
            break;
        case zTableSourceTypes::ENTITY:
            icon=QIcon::fromTheme(QStringLiteral("text-x-c++"));            
            break;
        case zTableSourceTypes::TXT:
            icon=QIcon::fromTheme(QStringLiteral("text"));
        default:
            break;
    }   

    if(tn.isEmpty()){
        zlog.error(QStringLiteral("Nincs megnevezés. table: %1 class: %2").arg(t.sql_table, t.class_name));
        tn = QStringLiteral("?");
        //tn = (!t.tablename.isEmpty())?t.tablename:(!t.class_name.isEmpty())?t.class_name:zStringHelper::Empty;
    }

    new QListWidgetItem(icon, tn, ui.listWidget_ztables, sourcetype);
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



void retek2::mezoListaFeltolt(zTable t){
    zlog.trace("feltoltMezoLista: "+t.toString());
    ui.tableWidget_MezoLista->blockSignals(true);
    ui.tableWidget_MezoLista->setRowCount(0);
    for(int r_ix=0;r_ix<t.rows.length();r_ix++){
        auto r = t.rows[r_ix];
        ui.tableWidget_MezoLista->insertRow(r_ix);

        ui.tableWidget_MezoLista->setItem(r_ix, C_ix_colName,  CreateTableItem(QVariant(r.colName)));
        ui.tableWidget_MezoLista->setItem(r_ix, C_ix_colType,  CreateTableItem(QVariant(r.colType)));
        ui.tableWidget_MezoLista->setItem(r_ix, C_ix_dlen, CreateTableItem(QVariant(r.dlen)));
        ui.tableWidget_MezoLista->setItem(r_ix, C_ix_Caption, CreateTableItem(QVariant(r.Caption)));



       /*auto cw = ui.tableWidget_MezoLista->indexWidget(//(r_ix,C_ix_Caption);
       auto l = cw->layout();
       auto w = l->widget();*/



       //QPushButton btn1(QStringLiteral("ty"), w);


        //ui.tableWidget_MezoLista->setCellWidget(r_ix, C_ix_Caption, pWidget);


        ui.tableWidget_MezoLista->setItem(r_ix, C_ix_nullable, CreateTableItem(QVariant(r.isNullable)));
    }    
    ui.tableWidget_MezoLista->blockSignals(false);
}

void retek2::feltoltKulcsLista(zTable t) {
    zlog.trace("feltoltKulcsLista: " + t.name);

    ui.listWidget_IdegenKulcs->clear();
    if(!t.pkname.isEmpty()){
        ui.listWidget_IdegenKulcs->addItem("PK:"+t.pkname);
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
        zlog.error(QStringLiteral("nincs tábla kiválasztva"));
        return;
    }

    auto class_name = zStringHelper::getclass_nameCamelCase(table->name);

    //saveTablaToXML(table->tablename);

    if (ui.checkBox_XML->isChecked()) {
        zlog.trace(QStringLiteral("XML"));

        QString e;
        QXmlStreamWriter s(&e);
        s.setAutoFormatting(true);
        s.writeStartDocument();

        table->toXML(&s);
        s.writeEndDocument();

        zlog.message(e);
    }

	if (ui.checkBox_CClass->isChecked()) {
        zlog.trace(QStringLiteral("C# Class"));

        auto txt = generateTmp(QStringLiteral("MVC_CClass.cs"));
        zlog.message(txt);
        //auto fn = zFileNameHelper::append(QDir::homePath(),beallitasok.projectdir,beallitasok.currentProjectName,table->name + ".cs");
        auto fn = zFileNameHelper::getCurrentProjectFileName(table->name + ".cs");
        zTextFileHelper::save(txt, fn);
	}

    if (ui.checkBox_Enum->isChecked())
    {
        if(!table->sql_conn.isEmpty())
        {
            zlog.trace("Enum");

            auto dbconn = beallitasok.getUI();
            if(dbconn.isValid())
            {
                zSQL zsql;
                if(zsql.init(dbconn)){
                    auto ed = GetEnumData(&zsql);
                    QString txt = zEnumizer::GenerateEnum(ed);

                    zlog.message(txt);
                    auto fn = zFileNameHelper::getCurrentProjectFileName(table->name + "_enum.cs");
                            //append(QDir::homePath(),beallitasok.projectdir,beallitasok.currentProjectName,table->name + "_enum.cs");
                    zTextFileHelper::save(txt, fn);
                }
                else{
                    zlog.trace(QStringLiteral("nincs sqlinit"));
                }
            }
            else
            {
                zlog.trace(QStringLiteral("nincs dbconn"));
            }
        }
        else
        {
            zlog.trace("nem sql típusú");
        }
    }


    if (ui.checkBox_Entity->isChecked()) {
        zlog.trace(QStringLiteral("C# Entity"));

        auto txt = generateTmp(QStringLiteral("DAL_Entity.cs"));
        zlog.message(txt);
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
        zlog.trace(QStringLiteral("Model"));
        auto txt = generateTmp(QStringLiteral("MVC_Model.cs"));
        //zlog.trace(txt);        
        zTextFileHelper::save(txt, beallitasok.getModelFilename(class_name + ".cs", QStringLiteral("Models")));
	}

	if (ui.checkBox_Meta->isChecked()) {
        zlog.trace(QStringLiteral("ModelMeta"));
        auto txt = generateTmp(QStringLiteral("MVC_ModelMeta.cs"));
        zTextFileHelper::save(txt, beallitasok.getModelFilename(class_name + "Meta" + ".cs", QStringLiteral("Models")));
	}

	if (ui.checkBox_Controller->isChecked()) {
        zlog.trace(QStringLiteral("Controller"));
        auto txt = generateTmp(QStringLiteral("MVC_Controller.cs"));
        zTextFileHelper::save(txt, beallitasok.getModelFilename(class_name + "Controller" + ".cs", QStringLiteral("Controllers")));
	}

    if (ui.checkBox_DataProvider->isChecked()) {
        zlog.trace(QStringLiteral("DataProvider"));
        auto txt = generateTmp(QStringLiteral("MVC_DataProvider.cs"));
        zTextFileHelper::save(txt, beallitasok.getModelFilename(class_name + "DataProvider" + ".cs", QStringLiteral("DataProviders")));
    }

    if (ui.checkBox_View->isChecked()) {
        zlog.trace(QStringLiteral("View"));
        auto txtIndex = generateTmp(QStringLiteral("MVC_ViewIndex.cshtml"));
        zTextFileHelper::save(txtIndex, beallitasok.getModelFilename(class_name + "ViewIndex" + ".cshtml", QStringLiteral("Views")));

        auto txtAdatlap = generateTmp(QStringLiteral("MVC_ViewAdatlapDX.cshtml"));
        zTextFileHelper::save(txtAdatlap, beallitasok.getModelFilename(class_name + "ViewAdatlapDX" + ".cshtml", QStringLiteral("Views")));
    }

//	if (ui.checkBox_ViewIndex->isChecked()) {
//        zlog.trace("Index");
//		auto txt = generateTmp("MVC_Index.cshtml");
//        zTextFileHelper::save(txt, beallitasok.getModelFilename(class_name + "ViewIndex" + ".cshtml", "Views"));
//	}

//	if (ui.checkBox_ViewEdit->isChecked()) {
//        zlog.trace("Edit");
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
 * A 2-es tabon megadott szöveg alapján generál táblaszerkezetet
 * ami alapján validálja az adatbázisban szereplő táblát.
 *
 * Ehez meg kell adni egy adatzbázis kapcsolatot - a validálandó ztabla connectionja az
 */

void retek2::on_pushButton_2_clicked()
{    
    // TODO a ztablesbe bele kell tenni az sql connectiont, séma és tábla szinten
    // ez amikor egy ztable kötődik egy sql-hez, tudjuk megtenni,
    // - vagy mert onnan olvastuk be
    // - vagy mert oda írjuk ki (generálunk create scriptet?)

    QString schemaName = zStringHelper::Empty;//ui.comboBox_connections->currentText();
    zSQL zsql;
    auto dbconn = beallitasok.getUI();
    if(dbconn.isValid()){
        if(zsql.init(dbconn)){
            zlog.trace(QStringLiteral("GenerateByText"));
            }

        auto txt = ui.textEdit->toPlainText();
        auto tl = zTable::createTableByText(txt);
        if(tl.length()>0){
            zforeach(t,tl){
                auto t_sql = zsql.getTable(schemaName, t->sql_table);
                auto vl = t_sql.Compare(*t);
                zlog.error("--- "+t->sql_table+" ---");
                zlog.error(vl);
                }
            zlog.error(QStringLiteral("--- --- ---"));
            }
        else{
            zlog.error(QStringLiteral("nincs egyezés, nincs vizsgálat"));
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
_
OperationType,OperationType
InventoryStatus,OperationType
Storage,Storage
User,User
InventoryItem[]
*/

/*
Macro def: Adm
GenerateByText: Inventory(PK:Id,Name,OperationTypeId,InventoryStatusId,StorageId,DateStart,DateEnd,DateCre,DateMod,RecountRequired,UserId,Comments),OperationType,InventoryStatus,Storage,User,InventoryItem
*/
void retek2::on_pushButton_3_clicked()
{
    zlog.trace(QStringLiteral("on_pushButton_3_clicked"));

    auto txt = ui.textEdit->toPlainText();
    auto tl = zTable::createTableByText(txt);

    if(tl.length()==0) { zlog.error(QStringLiteral("nincs egyezés, nincs vizsgálat")); return;}

    zforeach(t,tl){
        ztables.append(*t);
        zTablaToList(*t);
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
    zlog.trace(QStringLiteral("on_pushButton_4_clicked"));

    auto txt = ui.textEdit->toPlainText();
    auto tl = zTable::createTableByText_2(txt);

    if(tl.length()==0) { zlog.error(QStringLiteral("nem jött létre adat")); return;}

    zforeach(t,tl){
        ztables.append(*t);
        zTablaToList(*t);
        }
}


// load from xml
void retek2::on_pushButton_5_clicked()
{
    zlog.trace(QStringLiteral("on_pushButton_5_clicked"));

    auto txt = ui.textEdit->toPlainText();
    auto tl = zTable::createTableByXML(txt);

   if(tl.length()==0) { zlog.error(QStringLiteral("nem jött létre adat")); return;}

    zforeach(t,tl){
        ztables.append(*t);
        zTablaToList(*t);
        }
}

/*
Enumot készít a megnevezés és az id mező adatbázisban lévő értéke alapján
*/
/*
void retek2::on_pushButton_5_clicked()
{
    zlog.trace("on_pushButton_5_clicked");

    auto ed = GetEnumData();
    QString enumstr = zEnumizer::GenerateEnum(ed);
    zlog.trace(enumstr);
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
            zlog.trace(QStringLiteral("Nincs megnevezés sor kijelölve"));
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
        zlog.trace(QStringLiteral("Nincs sor kijelölve"));
        eredmeny= { "", "", QMap<int, QString>() };
    }
    return eredmeny;
}


void retek2::on_pushButton_6_clicked()
{    
    zlog.trace(QStringLiteral("Entitások beolvasása"));

    auto txt = ui.textEdit->toPlainText();

    auto sourceFileFilter = QStringList()<<QStringLiteral("*.c")<<QStringLiteral("*.cs");

    auto txtFileList = zStringHelper::getFilePaths(txt, sourceFileFilter);

    QList<zTable> tl;

    if(txtFileList.isEmpty()){
        tl = zTable::createTableByClassTxt(txt);
    }
    else{
        zforeach(f, txtFileList){
            QString f_txt = zTextFileHelper::load(*f);
            tl << zTable::createTableByClassTxt(f_txt);
        }
    }

    zforeach(t,tl){
        ztables.append(*t);
        zTablaToList(*t);
        }
}

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
            zlog.error(QStringLiteral("Nem módosítható oszlop: %1").arg(mn));
            break;

    }
}


void retek2::closeEvent (QCloseEvent * /*event*/)
{
    zforeach(t, ztables){
        t->saveTablaToXML();
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
        zlog.error(QStringLiteral("Nem található a tábla: %1").arg(name));
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
       zlog.error(QString(QStringLiteral("Az adatbáziskapcsolat adatai hibásak: %1 driver: %2")).arg(dbconn.Name,dbconn.driver));
    }
    //zsql.createConnection();

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

void retek2::zTablaToList(QList<zTable> ts){
    zforeach(t, ts){
        zTablaToList(*t);
    }
}




void retek2::on_pushButton_table_import_clicked()
{
    auto items = ui.listWidget_tables->selectedItems();
    QString schemaName = ui.listWidget_schemas->currentItem()->text();
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
        zTable t = zsql.getTable(schemaName, tableName);
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
               zTablaToList(t);
            }
            else{
                continue;
            }
        }
    }
}

void retek2::on_pushButton_createSourcePath_clicked()
{

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
    QString txt;
    txt = ui.lineEdit_name->text();
    if(!txt.isEmpty()){
        if(txt != t.name){
            auto i = ui.listWidget_ztables->findItems(t.name, Qt::MatchExactly);
            if(!i.isEmpty()){
                i[0]->setText(txt);
                QString fn_old = zFileNameHelper::getCurrentProjectFileName(t.name+".xml");
                QString fn_new = zFileNameHelper::getCurrentProjectFileName(txt+".xml");
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
        t.name = txt;
        txt = ui.lineEdit_tablename->text();
        txt = ui.lineEdit_class_name->text();
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
    auto select = ui.tableWidget_MezoLista->selectionModel();
    if(select->hasSelection())
    {
        auto rs = select->selectedRows();
        //auto cs = select->selectedColumns();
        auto is = select->selectedIndexes();
        //QString colName; // name
        //QString ft; // type
        int idix = 0;
        QString caption;

        if(!rs.isEmpty())
        {
            zforeach(r,rs)
            {
                idix = r->row();
                caption = getCaptionByRowIx(idix);
                if(!caption.isEmpty())
                {
                    setCaptionByRowIx(idix, caption);
                }
            }
        }
        if(!is.isEmpty())
        {
            zforeach(i,is)
            {
                idix = i->row();
                caption = getCaptionByRowIx(i->row());
                if(!caption.isEmpty())
                {
                    setCaptionByRowIx(idix, caption);
                }
            }
        }
    }
    //QString caption = zConversionMap::value(globalCaptionMaps, ft);
}

QString retek2::getCaptionByRowIx(int idix)
{
    QString caption;// = ui.tableWidget_MezoLista->item(idix, C_ix_Caption)->text();
    //if(caption.isEmpty())
    //{
        QString colName = ui.tableWidget_MezoLista->item(idix, C_ix_colName)->text().toLower();
        caption = zConversionMap::value(globalCaptionMaps, colName);
        if(caption.isEmpty())
        {
            zlog.warning(QStringLiteral("Nem határozható meg felirat a mezőnévhez: %1").arg(colName));
        }
    //}
    return caption;
}

void retek2::setCaptionByRowIx(int idix, const QString& caption)
{
    ui.tableWidget_MezoLista->item(idix, C_ix_Caption)->setText(caption);
}
