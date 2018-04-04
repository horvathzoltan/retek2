#include "retek2.h"
#include <QDebug>
#include <QString>
#include <QSqlQuery>
#include <QSqlError>
#include <QListWidget>
#include <QTableWidgetItem>
#include <QFileInfo>
#include <QFile>
#include <QRegularExpression>
#include <QTableWidgetItem>
#include <QDir>
#include <QTextCodec>
#include <QMap>
#include <QMessageBox>

#include "ztable.h"
#include "ztablerow.h"
#include "zsql.h"
#include "zstringhelper.h"
#include "zstringmaphelper.h"
#include "zfilenamehelper.h"
#include "globals.h"
#include "zenumizer.h"
#include "ztextfilehelper.h"

retek2::retek2(QWidget *parent):QMainWindow(parent){
	ui.setupUi(this);
}

retek2::~retek2()
{

}

void retek2::init(void)
{	
    zlog.init(ui.textBrowser);
    beallitasok.init(ui.lineEdit_User, ui.lineEdit_Password, ui.lineEdit_Server, ui.lineEdit_Catalog, ui.comboBox_connections);

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

    beallitasok.load();    

    initBy(beallitasok.getSelected());

    zlog.trace("retek2 init OK");

    //ui.lineEdit_ContextName->setText(getAdatbazisnev()+"Context2");
}

void retek2::initBy(dbConnection* b){
    if(b==nullptr) return;

    beallitasok.setUI(*b);
    zsql.init(*b);
    ztokenizer.init(ui.tableWidget_MezoLista);
    feltoltTabla(); // bal tábla panel feltöltése
    zStringMapHelper::StringMapFeltolt(zFileNameHelper::append(QDir::homePath(),beallitasok.munkadir, b->adatbazisNev, "caption_global.txt"), &globalCaptionMap); // globális elnevezéstábla
}

void retek2::saveCaptionTabla(QString tablanev) {
    auto b = beallitasok.getSelected();
    if(b==nullptr) return;

    QString fn = zFileNameHelper::append(QDir::homePath(),beallitasok.munkadir,b->adatbazisNev, "caption_" + tablanev + ".txt");

    QMap<QString, QString> tablaCaptionMap;
    //tablaCaptionMap.clear();

	int rows = ui.tableWidget_MezoLista->rowCount();
	for (int i = 0; i < rows; i++) {
		QTableWidgetItem *item_colName = ui.tableWidget_MezoLista->item(i, C_ix_colName);
		QTableWidgetItem *item_Caption = ui.tableWidget_MezoLista->item(i, C_ix_Caption);
		//QString colName = item_Caption->text();

		tablaCaptionMap.insert(item_colName->text(), item_Caption->text());
	}
	if(tablaCaptionMap.count()>0)
        zStringMapHelper::StringMapSave(fn, &tablaCaptionMap);
}

void retek2::feltoltTabla(void) {
    QList<QString> tns = zsql.getTableNames();
    ui.listWidget_tabla->clear();
    zforeach(tn,tns){
        new QListWidgetItem(*tn, ui.listWidget_tabla);
    }
}

void retek2::TableSelect(QListWidgetItem* i) {
	if(!tablanev.isEmpty())
		saveCaptionTabla(tablanev);

	tablanev = i->text();    


    //feltoltEljaras(tablanev);
    feltoltMezoLista(tablanev);

    //ui.listWidget_IdegenKulcs->clear();
    //feltoltPk(tablanev);

    //feltoltIdegenkulcs(tablanev);
    //ui.tabWidget->setCurrentWidget(ui.tab);
}

//void retek2::feltoltEljaras(QString tablanev) {
//    zlog.trace("feltoltEljaras " + tablanev);
//}


//void retek2::feltoltIdegenkulcs(QString tablanev) {
//    zlog.trace("feltoltIdegenkulcs " + tablanev);

//	ui.listWidget_IdegenKulcs->clear();

//    //QString str("***");

//    //ui.listWidget_IdegenKulcs->addItem(str);

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



void retek2::feltoltMezoLista(QString tablanev){    
    auto tns = tablanev.split('.');
    zTable t;
    if(tns.length()>1){
        if(tns[0]=="txt"){
            t = *zTable::getByName(&ztables, tns[1]);
        }
        else
            t = zsql.getTable(tablanev);
        }
    else
        t = zsql.getTable(tablanev);
    feltoltMezoLista(t);
    ui.listWidget_IdegenKulcs->clear();    
    feltoltPk(t);

    feltoltFk(t);
}

void retek2::feltoltMezoLista(zTable t){
    ui.tableWidget_MezoLista->setRowCount(0);
    for(int r_ix=0;r_ix<t.rows.length();r_ix++){
        auto r = t.rows[r_ix];
        ui.tableWidget_MezoLista->insertRow(r_ix);
        ui.tableWidget_MezoLista->setItem(r_ix, C_ix_colName,  CreateTableItem(QVariant(r.colName)));
        ui.tableWidget_MezoLista->setItem(r_ix, C_ix_colType,  CreateTableItem(QVariant(r.colType)));
        ui.tableWidget_MezoLista->setItem(r_ix, C_ix_dlen, CreateTableItem(QVariant(r.dlen)));
        ui.tableWidget_MezoLista->setItem(r_ix, C_ix_Caption, CreateTableItem(QVariant(r.Caption)));
        ui.tableWidget_MezoLista->setItem(r_ix, C_ix_nullable, CreateTableItem(QVariant(r.nullable)));
    }
    zlog.log("feltoltMezoLista: "+t.toString());
}

void retek2::feltoltPk(zTable t) {
    zlog.trace("feltoltPK " + tablanev);

    if(!t.pkname.isEmpty()){
        ui.listWidget_IdegenKulcs->addItem("PK:"+t.pkname);
        }   
}

void retek2::feltoltFk(zTable t) {
    zlog.trace("feltoltFK " + tablanev);

//    QStringList fkl;

//    if(!fkl.isEmpty()){
//        zforeach(fk, fkl){
//            ui.listWidget_IdegenKulcs->addItem("FK:"+t.pkname);
//            }
//    }
}

QTableWidgetItem* retek2::CreateTableItem(QVariant v){
    auto a = new QTableWidgetItem();
    a->setData(Qt::DisplayRole, v);
    return a;
}

/*!
 \fn retek2::GenerateAll()

 generate files by selected templates
*/
void retek2::GenerateAll() {
     auto b = beallitasok.getSelected();
     if(b==nullptr) return;

	qDebug("GenerateAll");
    if (!tablanev.isEmpty()){
		saveCaptionTabla(tablanev);
        }

	if (ui.checkBox_CClass->isChecked()) {
		qDebug("C# Class");

        auto txt = generateTmp("MVC_CClass.cs");
        zlog.trace(txt);
        zTextFileHelper::save(txt, zFileNameHelper::append(QDir::homePath(),beallitasok.munkadir,b->adatbazisNev,tablanev + ".cs"));
	}

    if (ui.checkBox_Enum->isChecked()) {
        qDebug("Enum");

        auto ed = GetEnumData();
        QString txt = zEnumizer::GenerateEnum(ed);

        zlog.trace(txt);
        zTextFileHelper::save(txt, zFileNameHelper::append(QDir::homePath(),beallitasok.munkadir,b->adatbazisNev,tablanev + "_enum.cs"));
    }

    if (ui.checkBox_Entity->isChecked()) {
        qDebug("C# Entity");

        auto txt = generateTmp("DAL_Entity.cs");
        zlog.trace(txt);
        zTextFileHelper::save(txt, beallitasok.getModelFilename(ztokenizer.getClassNameCamelCase(tablanev) + ".cs", "Entities"));
    }

	//checkBox_Context
/*	if (ui.checkBox_Context->isChecked()) {
		qDebug("Context");
		auto txt = generateContext();
		SaveAllTextToFile(&txt, getContextFilename(tablanev + "Context.cs"));
	}*/

	if (ui.checkBox_Model->isChecked()) {
		qDebug("Model");
		auto txt = generateTmp("MVC_Model.cs");
        //zlog.trace(txt);
        zTextFileHelper::save(txt, beallitasok.getModelFilename(ztokenizer.getClassNameCamelCase(tablanev) + ".cs", "Models"));
	}

	if (ui.checkBox_Meta->isChecked()) {
		qDebug("ModelMeta");
		auto txt = generateTmp("MVC_ModelMeta.cs");
        zTextFileHelper::save(txt, beallitasok.getModelFilename(ztokenizer.getClassNameCamelCase(tablanev) + "Meta" + ".cs", "Models"));
	}

	if (ui.checkBox_Controller->isChecked()) {
		qDebug("Controller");
		auto txt = generateTmp("MVC_Controller.cs");
        zTextFileHelper::save(txt, beallitasok.getModelFilename(ztokenizer.getClassNameCamelCase(tablanev) + "Controller" + ".cs", "Controllers"));
	}

    if (ui.checkBox_DataProvider->isChecked()) {
        qDebug("DataProvider");
        auto txt = generateTmp("MVC_DataProvider.cs");
        zTextFileHelper::save(txt, beallitasok.getModelFilename(ztokenizer.getClassNameCamelCase(tablanev) + "DataProvider" + ".cs", "DataProviders"));
    }

    if (ui.checkBox_View->isChecked()) {
        qDebug("View");
        auto txtIndex = generateTmp("MVC_ViewIndex.cshtml");
        zTextFileHelper::save(txtIndex, beallitasok.getModelFilename(ztokenizer.getOsztalynevLower(tablanev) + "ViewIndex" + ".cshtml", "Views"));

        auto txtAdatlap = generateTmp("MVC_ViewAdatlapDX.cshtml");
        zTextFileHelper::save(txtAdatlap, beallitasok.getModelFilename(ztokenizer.getOsztalynevLower(tablanev) + "ViewAdatlapDX" + ".cshtml", "Views"));
    }

	if (ui.checkBox_ViewIndex->isChecked()) {
		qDebug("Index");
		auto txt = generateTmp("MVC_Index.cshtml");
        zTextFileHelper::save(txt, beallitasok.getModelFilename(ztokenizer.getOsztalynevLower(tablanev) + "ViewIndex" + ".cshtml", "Views"));
	}

	if (ui.checkBox_ViewEdit->isChecked()) {
		qDebug("Edit");
        auto txt = generateTmp("MVC_Edit.cshtml");
        zTextFileHelper::save(txt, beallitasok.getModelFilename(ztokenizer.getOsztalynevLower(tablanev) + "ViewEdit" + ".cshtml", "Views"));
	}
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








QString retek2::generateTmp(QString tmp_file) {
    //qDebug() << tmp_file;
    if(tmp_file.isEmpty())
        {zLog::ShowDialog("Nincs sablon fájl");return "";}

    auto tmp_fn = beallitasok.getTemplateFilename(tmp_file);

    if(tmp_fn == NULL) {zLog::ShowDialog("A sablon fájl nem található: "+ tmp_file);return "";}

    QString tmp = zTextFileHelper::load(tmp_fn);

    auto b = beallitasok.getSelected();
    if(b != nullptr){
        ztokenizer.tokenize(&tmp, nullptr, 0, b->adatbazisNev);
        }
	return tmp;
}

//KSzallitolevelTetel.SzallitolevelId -> 

void retek2::on_pushButton_clicked()
{
    auto dbconn = beallitasok.getUI();

    if(dbconn.isValid()){
        if(zsql.init(dbconn))
            if(!beallitasok.dbConnections.contains(dbconn)){
                beallitasok.addConnection(dbconn);
            }
        }
    //zsql.createConnection();
    feltoltTabla();
}

/*!
 * A 2-es tabon megadott szöveg alapján generál táblaszerkezetet
 * ami alapján validálja az adatbázisban szereplő táblát.
 */

void retek2::on_pushButton_2_clicked()
{
    zlog.trace("GenerateByText");

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
        new QListWidgetItem("txt."+t->tablename, ui.listWidget_tabla);
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

    if(tl.length()==0) { zlog.log("nincs egyezés, nincs vizsgálat"); return;}

    zforeach(t,tl){
        ztables.append(*t);
        new QListWidgetItem("txt."+t->tablename, ui.listWidget_tabla);
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
zEnumizer::EnumSource retek2::GetEnumData(){
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

        auto ms = zsql.getTable_SQL_ENUM(tablanev, fn);
        QString cn = ztokenizer.getClassNameCamelCase(tablanev);

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
    initBy(beallitasok.getSelected());
}
