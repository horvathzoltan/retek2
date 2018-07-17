#include "globals.h"
#include "retek2.h"
#include "zfilenamehelper.h"
#include "zstringhelper.h"
#include "zstringmaphelper.h"
#include "zsourcehelper.h"
#include "ztablerow.h"
#include "ztextfilehelper.h"

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


retek2::retek2(QWidget *parent):QMainWindow(parent){
	ui.setupUi(this);
}

retek2::~retek2()= default;


void retek2::init()
{	
    zlog.init(ui.textBrowser);
    beallitasok.init(ui.lineEdit_User, ui.lineEdit_Password, ui.lineEdit_Server, ui.lineEdit_Catalog, ui.comboBox_connections, ui.comboBox);

    // TODO egy belső típust kell létrehozni, ami a perzisztens és kód közti kötést írja le, mindkét irányban
    // amennyiben ez lehetséges - figyelembevéve, hogy
    // csak az sql szintet vizsgálva nem lehet eldönteni, hogy az a kódban mivé alakul
    // illetve a kód irányából, pl. a string konkrétan melyik szöveges típusra alakul
    // amíg erre nincsen megoldás, nem lehet egybe hozni ezeket a táblákat,
    // így ezek konverziója kód szintű marad, illetve- kell legyen


    //sql->c# irány
	typeMap.insert("uniqueidentifier", "Guid");
	typeMap.insert("int", "int");
	typeMap.insert("datetime", "DateTime");
	typeMap.insert("date", "Date");
    typeMap.insert("nvarchar", "string");
	typeMap.insert("nchar", "string");	
	typeMap.insert("char", "string");
	typeMap.insert("varchar", "string");
	typeMap.insert("float", "float");
	typeMap.insert("bit", "bool");
	typeMap.insert("decimal", "decimal");
	typeMap.insert("xml", "string");

    //c# -> sql irány
    typeMapR.insert("Guid","uniqueidentifier");
    typeMapR.insert("int", "int");
    typeMapR.insert("DateTime", "datetime");
    typeMapR.insert("Date","date");
    typeMapR.insert("string","nvarchar");
    typeMapR.insert("float", "float");
    typeMapR.insert("bool","bit");
    typeMapR.insert("decimal", "decimal");

    // ztables feltöltése
    // - itt valójában csak a beállításokat kellene betölteni
    // 
    beallitasok.load();    

    auto b = beallitasok.getSelectedDbConnection();

    // TODO b->adatbazisNev helyett projectnek legyen saját neve
    if(b != nullptr){
        QString path = zFileNameHelper::append(QDir::homePath(),beallitasok.munkadir,b->adatbazisNev);

        QStringList xmlFilter("*.xml");
        QStringList files = zFileNameHelper::FindFileNameInDir(path, "", xmlFilter);
        zforeach(f, files){
            auto txt = zTextFileHelper::load(*f);
            auto t = zTable::createTableByXML(txt);
            ztables << t;
            zTablaToList(t);
        }
    }
    else{
        // a projet neve az adatbázis nevével egyezik meg

        //
        zlog.log("A project nem rendelkezik adatbázis kapcsolattal");
    }

    // TODO - xml beolvasás után a forrást ellenőrízni -
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

    zlog.trace("retek2 init OK");
}

void retek2::initBy(dbConnection* c){
    if(c==nullptr) return;

    beallitasok.setUI(*c);
    ztokenizer.init(ui.tableWidget_MezoLista);

    zStringMapHelper::StringMapFeltolt(zFileNameHelper::append(QDir::homePath(),beallitasok.munkadir, c->adatbazisNev, "caption_global.txt"), &globalCaptionMap); // globális elnevezéstábla
}

void retek2::tablaListaFeltolt() {
    zSQL zsql;
    auto c = beallitasok.getSelectedDbConnection();
    zsql.init(*c);

    QList<QString> tns = zsql.getTableNames();
    ui.listWidget_ztables->clear();
    zTable t;
    zforeach(tn,tns){
        t = zsql.getTable(*(tn));
        if(t.rows.length()>0){
            ztables << t;
            zTablaToList(t);
        }
    }
}

void retek2::zTablaToList(QList<zTable> ts){
    zforeach(t, ts){
        zTablaToList(*t);
    }
}

void retek2::zTablaToList(zTable t){
    QString tn = t.tablename;
    int sourcetype;
    QIcon icon;
    if(!t.sql_conn.isEmpty())
        sourcetype = zTableSourceTypes::SQL;
    else if(!t.source_conn.isEmpty())
        sourcetype = zTableSourceTypes::ENTITY;


    switch(sourcetype){
        case zTableSourceTypes::SQL:
            icon=QIcon::fromTheme("office-database");
            break;
        case zTableSourceTypes::ENTITY:
            icon=QIcon::fromTheme("text-x-c++");
            break;
        case zTableSourceTypes::TXT:
            icon=QIcon::fromTheme("text");
            break;
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



void retek2::fejadatFeltolt(zTable t){
    ui.lineEdit_tablename->setText(t.tablename);
    ui.lineEdit_classname->setText(t.classname);
    ui.lineEdit_classname_plural->setText(t.classname_plural);

    //ui.lineEdit_tablename->setDisabled(t.sourcetype==SQL);
    //ui.lineEdit_classname->setDisabled(t.sourcetype==TXT);

}

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
        ui.tableWidget_MezoLista->setItem(r_ix, C_ix_nullable, CreateTableItem(QVariant(r.isNullable)));
    }    
    ui.tableWidget_MezoLista->blockSignals(false);
}

void retek2::feltoltKulcsLista(zTable t) {
    zlog.trace("feltoltKulcsLista: " + t.tablename);

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
        zlog.log("nincs tábla kiválasztva", zLog::ERROR);
        return;
    }

    auto classname = zStringHelper::getClassNameCamelCase(table->tablename);

    auto b = beallitasok.getSelectedDbConnection();
    if(b==nullptr) return;

    //saveTablaToXML(table->tablename);

    if (ui.checkBox_XML->isChecked()) {
        zlog.trace("XML");

        QString e;
        QXmlStreamWriter s(&e);
        s.setAutoFormatting(true);
        s.writeStartDocument();

        table->toXML(&s);
        s.writeEndDocument();

        zlog.log(e);
    }

	if (ui.checkBox_CClass->isChecked()) {
        zlog.trace("C# Class");

        auto txt = generateTmp("MVC_CClass.cs");
        zlog.log(txt);
        auto fn = zFileNameHelper::append(QDir::homePath(),beallitasok.munkadir,b->adatbazisNev,table->tablename + ".cs");
        zTextFileHelper::save(txt, fn);
	}

    if (ui.checkBox_Enum->isChecked()) {

        if(!table->sql_conn.isEmpty()){
        zlog.trace("Enum");

        auto dbconn = beallitasok.getUI();
        if(dbconn.isValid()){
            zSQL zsql;
            if(zsql.init(dbconn)){
                auto ed = GetEnumData(&zsql);
                QString txt = zEnumizer::GenerateEnum(ed);

                zlog.log(txt);
                auto fn = zFileNameHelper::append(QDir::homePath(),beallitasok.munkadir,b->adatbazisNev,table->tablename + "_enum.cs");
                zTextFileHelper::save(txt, fn);
            }
            else{
                zlog.trace("nincs sqlinit");
            }
        }
        else{
            zlog.trace("nincs dbconn");
        }
    }
    else{
        zlog.trace("nem sql típusú");
    }
}


    if (ui.checkBox_Entity->isChecked()) {
        zlog.trace("C# Entity");

        auto txt = generateTmp("DAL_Entity.cs");
        zlog.log(txt);
        auto fn = beallitasok.getModelFilename(classname + ".cs", "Entities");
        zTextFileHelper::save(txt, fn);
    }

	//checkBox_Context
/*	if (ui.checkBox_Context->isChecked()) {
		qDebug("Context");
		auto txt = generateContext();
		SaveAllTextToFile(&txt, getContextFilename(tablanev + "Context.cs"));
	}*/

	if (ui.checkBox_Model->isChecked()) {
        zlog.trace("Model");
		auto txt = generateTmp("MVC_Model.cs");
        //zlog.trace(txt);        
        zTextFileHelper::save(txt, beallitasok.getModelFilename(classname + ".cs", "Models"));
	}

	if (ui.checkBox_Meta->isChecked()) {
        zlog.trace("ModelMeta");
		auto txt = generateTmp("MVC_ModelMeta.cs");        
        zTextFileHelper::save(txt, beallitasok.getModelFilename(classname + "Meta" + ".cs", "Models"));
	}

	if (ui.checkBox_Controller->isChecked()) {
        zlog.trace("Controller");
		auto txt = generateTmp("MVC_Controller.cs");
        zTextFileHelper::save(txt, beallitasok.getModelFilename(classname + "Controller" + ".cs", "Controllers"));
	}

    if (ui.checkBox_DataProvider->isChecked()) {
        zlog.trace("DataProvider");
        auto txt = generateTmp("MVC_DataProvider.cs");
        zTextFileHelper::save(txt, beallitasok.getModelFilename(classname + "DataProvider" + ".cs", "DataProviders"));
    }

    if (ui.checkBox_View->isChecked()) {
        zlog.trace("View");
        auto txtIndex = generateTmp("MVC_ViewIndex.cshtml");
        zTextFileHelper::save(txtIndex, beallitasok.getModelFilename(classname + "ViewIndex" + ".cshtml", "Views"));

        auto txtAdatlap = generateTmp("MVC_ViewAdatlapDX.cshtml");
        zTextFileHelper::save(txtAdatlap, beallitasok.getModelFilename(classname + "ViewAdatlapDX" + ".cshtml", "Views"));
    }

//	if (ui.checkBox_ViewIndex->isChecked()) {
//        zlog.trace("Index");
//		auto txt = generateTmp("MVC_Index.cshtml");
//        zTextFileHelper::save(txt, beallitasok.getModelFilename(classname + "ViewIndex" + ".cshtml", "Views"));
//	}

//	if (ui.checkBox_ViewEdit->isChecked()) {
//        zlog.trace("Edit");
//        auto txt = generateTmp("MVC_Edit.cshtml");
//        zTextFileHelper::save(txt, beallitasok.getModelFilename(classname + "ViewEdit" + ".cshtml", "Views"));
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
        {zLog::ShowDialog("Nincs sablon fájl");return "";}

    auto tmp_fn = beallitasok.getTemplateFilename(tmp_file);

    if(tmp_fn == nullptr) {zLog::ShowDialog("A sablon fájl nem található: "+ tmp_file);return "";}

    QString tmp = zTextFileHelper::load(tmp_fn);

   // auto aaa = QString.is
    auto b = beallitasok.getSelectedDbConnection();
    if(b != nullptr){
        ztokenizer.tokenize(&tmp, nullptr, 0, b->adatbazisNev);
        }
	return tmp;
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
        tablaListaFeltolt();
        }
    else{
       zlog.log(QString("Az adatbáziskapcsolat adatai hibásak: %1 driver: %2").arg(dbconn.Getname(),dbconn.driver), zLog::ERROR);
    }
    //zsql.createConnection();

}

/*!
 * A 2-es tabon megadott szöveg alapján generál táblaszerkezetet
 * ami alapján validálja az adatbázisban szereplő táblát.
 */

void retek2::on_pushButton_2_clicked()
{
    zSQL zsql;
    auto dbconn = beallitasok.getUI();
    if(dbconn.isValid()){
        if(zsql.init(dbconn)){
            zlog.trace("GenerateByText");
            }

        auto txt = ui.textEdit->toPlainText();
        auto tl = zTable::createTableByText(txt);
        if(tl.length()>0){
            zforeach(t,tl){
                auto t_sql = zsql.getTable(t->tablename);
                auto vl = t_sql.Validate(*t);
                zlog.log("--- "+t->tablename+" ---");
                zlog.log(vl);
                }
            zlog.log("--- --- ---");
            }
        else{
            zlog.log("nincs egyezés, nincs vizsgálat");
            }
    }
    return;
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
on_pushButton_3_clicked
Macro def: Adm
GenerateByText: Inventory(PK:Id,Name,OperationTypeId,InventoryStatusId,StorageId,DateStart,DateEnd,DateCre,DateMod,RecountRequired,UserId,Comments),OperationType,InventoryStatus,Storage,User,InventoryItem
*/
void retek2::on_pushButton_3_clicked()
{
    zlog.trace("on_pushButton_3_clicked");

    auto txt = ui.textEdit->toPlainText();
    auto tl = zTable::createTableByText(txt);

    if(tl.length()==0) { zlog.log("nincs egyezés, nincs vizsgálat"); return;}

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
    zlog.trace("on_pushButton_4_clicked");

    auto txt = ui.textEdit->toPlainText();
    auto tl = zTable::createTableByText_2(txt);

    if(tl.length()==0) { zlog.log("nem jött létre adat"); return;}

    zforeach(t,tl){
        ztables.append(*t);
        zTablaToList(*t);
        }
}


// load from xml
void retek2::on_pushButton_5_clicked()
{
    zlog.trace("on_pushButton_5_clicked");

    auto txt = ui.textEdit->toPlainText();
    auto tl = zTable::createTableByXML(txt);

   if(tl.length()==0) { zlog.log("nem jött létre adat"); return;}

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
    auto select = ui.tableWidget_MezoLista->selectionModel();
    if(select->hasSelection()){
        auto r = select->selectedRows();
        QString fn; // name
        QString ft; // type
        int idix = 0;

        ft = ui.tableWidget_MezoLista->item(idix, C_ix_colType)->text();

        if(r.length()<1){
            zlog.trace("Nincs megnevezés sor kijelölve");
            return { "", "", QMap<int, QString>() };
        }
        else if (r.length()>1){
            QString rs = "";
            zforeach(r1, r){
                int ix = (*r1).row();
                if(!rs.isEmpty()) rs+=",";
                rs+= ui.tableWidget_MezoLista->item(ix, C_ix_colName)->text();
            }
            fn = QString("concat_ws(\"_\", %1)").arg(rs);
        }
        else{
            int ix = r.first().row();
            fn = ui.tableWidget_MezoLista->item(ix, C_ix_colName)->text();
        }

        auto ms = (*zsql).getTable_SQL_ENUM(table->tablename, fn);
        QString cn = zStringHelper::getClassNameCamelCase(table->tablename);

        return { cn, ft, ms };
        }
    else{
        zlog.trace("Nincs sor kijelölve");
        return { "", "", QMap<int, QString>() };
    }
}


void retek2::on_comboBox_connections_currentIndexChanged(int index)
{
    beallitasok.setSelected(index);
    initBy(beallitasok.getSelectedDbConnection());

}


void retek2::on_lineEdit_classname_plural_editingFinished()
{
    table->classname_plural =  ui.lineEdit_classname_plural->text();
}

void retek2::on_lineEdit_classname_editingFinished()
{
     table->classname =  ui.lineEdit_classname->text();
}

void retek2::on_lineEdit_tablename_editingFinished()
{
    table->tablename =  ui.lineEdit_tablename->text();
}


// TODO globális caption tábla használatát beépíteni
void retek2::on_pushButton_6_clicked()
{    
    zlog.trace("Entitások beolvasása");

    auto txt = ui.textEdit->toPlainText();

    auto sourceFileFilter = QStringList()<<"*.c"<<"*.cs";

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
    auto r = zTablerow::getByName(&(table->rows), i_rowname);

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
            zlog.log(QString("Nem módosítható oszlop: %1").arg(mn));
            break;

    }
}


void retek2::closeEvent (QCloseEvent *event)
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
void retek2::TableSelect(QListWidgetItem* i) {
    auto tablanev = i->text();
    table = zTable::getByName(&ztables, tablanev);

    if(table != nullptr){
        fejadatFeltolt(*table);
        mezoListaFeltolt(*table);
        feltoltKulcsLista(*table);
        }
}


void retek2::on_listWidget_ztables_itemClicked(QListWidgetItem *item)
{
    auto tablanev = item->text();
    table = zTable::getByName(&ztables, tablanev);

    if(table != nullptr){
        fejadatFeltolt(*table);
        mezoListaFeltolt(*table);
        feltoltKulcsLista(*table);
        }
}
