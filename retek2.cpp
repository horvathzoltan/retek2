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
#include "zstringmaphelper.h"
#include "zfilenamehelper.h"
#include "globals.h"

retek2::retek2(QWidget *parent):QMainWindow(parent){
	ui.setupUi(this);
}

retek2::~retek2()
{

}

/*
void retek2::getCredsToBeall(Beallitasok *b1){
    if(b1){
    b1->user = ui1->lineEdit_User->text();
    b1->password = ui1->lineEdit_Password->text();
    b1->server = ui1->lineEdit_Server->text();
    b1->adatbazisNev = ui1->lineEdit_Catalog->text();
    }
}

void retek2::setCredsToBeall(Beallitasok *b, Ui *ui){
    if(b && ui){
    ui->lineEdit_User->setText(b->user);
    ui->lineEdit_Password->setText(b->password);
    ui->lineEdit_Server->setText(b->server);
    ui->lineEdit_Catalog->setText(b->adatbazisNev);
    }
}
*/



void retek2::init(void)
{	
    zlog.init(ui.textBrowser);
    beallitasok.init(ui.lineEdit_User, ui.lineEdit_Password, ui.lineEdit_Server, ui.lineEdit_Catalog);

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

	dxMap.insert("uniqueidentifier", "dxTextBox");
	dxMap.insert("int", "dxNumberBox");
	dxMap.insert("datetime", "DateTime");
	dxMap.insert("date", "dxDateBox");
	dxMap.insert("nchar", "dxDateBox");
	dxMap.insert("nvarchar", "dxTextBox");
	dxMap.insert("char", "dxTextBox");
	dxMap.insert("varchar", "dxTextBox");
	dxMap.insert("float", "dxNumberBox");
	dxMap.insert("bit", "dxCheckBox");
	dxMap.insert("decimal", "dxNumberBox");
	dxMap.insert("xml", "dxTextBox");


    beallitasok.setUI();
    //QString connstr = beallitasok.getConnStr();


    zsql.init(beallitasok.driver, beallitasok.server, beallitasok.adatbazisNev, beallitasok.user, beallitasok.password);

    feltoltTabla(); // bal tábla panel feltöltése

    feltoltTmpMap(); // a view template könyvtárában az összes templatek feltöltése

    zStringMapHelper::StringMapFeltolt(zFileNameHelper::getCClassFilename(beallitasok.munkadir, beallitasok.adatbazisNev, "caption_global.txt"), &globalCaptionMap); // globális elnevezéstábla

    zlog.trace("retek2 init OK");

    //ui.lineEdit_ContextName->setText(getAdatbazisnev()+"Context2");
}



//void retek2::feltoltCaptionTabla(QString tablanev) {
//	QString fn = getCClassFilename("caption_"+tablanev+".txt");

//    zstringmaphelper::StringMapFeltolt(fn, &tablaCaptionMap);
//}

void retek2::saveCaptionTabla(QString tablanev) {
    QString fn = zFileNameHelper::getCClassFilename(beallitasok.munkadir,beallitasok.adatbazisNev, "caption_" + tablanev + ".txt");

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

void retek2::feltoltTmpMap(void){
	QString viewTemplateDirName = getTemplateFilename("View");
    if(viewTemplateDirName == NULL) {zLog::ShowDialog("nincs sablon:");return;}

	auto viewTemplateDir = QDir(viewTemplateDirName);

	auto filelist = viewTemplateDir.entryList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden | QDir::AllDirs | QDir::Files, QDir::DirsFirst);//(QDir::Filter::Files,QDir::SortFlag::NoSort)

	foreach(auto f, filelist) {
		QString k = f;
		auto ix = f.indexOf('.');
		if (ix > -1) k = f.left(ix);

		QString v = ReadAllTextFromFile(viewTemplateDirName + "\\" + f);
		tmpMap.insert(k, v);
	}
}

void retek2::TableSelect(QListWidgetItem* i) {
	if(!tablanev.isEmpty())
		saveCaptionTabla(tablanev);

	tablanev = i->text();    

	feltoltIdegenkulcs(tablanev);
	feltoltEljaras(tablanev);    
    feltoltMezoLista(tablanev);

    ui.tabWidget->setCurrentWidget(ui.tab);
}



void retek2::feltoltEljaras(QString tablanev) {
    //if (!is_dbOK) return;
	qDebug() << "feltoltEljaras " << tablanev;

}

void retek2::feltoltIdegenkulcs(QString tablanev) {
    //if (!is_dbOK) return;
	qDebug() << "feltoltIdegenkulcs " << tablanev;

	ui.listWidget_IdegenKulcs->clear();

	QString commandTextTemplate = "SELECT  obj.name AS FK_NAME, "
		"sch.name AS[schema_name], "
		"tab1.name AS[table], "
		"col1.name AS[column], "
		"tab2.name AS[referenced_table], "
		"col2.name AS[referenced_column] "
		"FROM sys.foreign_key_columns fkc "
		"INNER JOIN sys.objects obj "
		"ON obj.object_id = fkc.constraint_object_id "
		"INNER JOIN sys.tables tab1 "
		"ON tab1.object_id = fkc.parent_object_id "
		"INNER JOIN sys.schemas sch "
		"ON tab1.schema_id = sch.schema_id "
		"INNER JOIN sys.columns col1 "
		"ON col1.column_id = parent_column_id AND col1.object_id = tab1.object_id "
		"INNER JOIN sys.tables tab2 "
		"ON tab2.object_id = fkc.referenced_object_id "
		"INNER JOIN sys.columns col2 "
		"ON col2.column_id = referenced_column_id AND col2.object_id = tab2.object_id "
		"where tab1.name = '%1'";

	QString commandText = commandTextTemplate.arg(tablanev);

	QSqlQuery query(commandText);

	// FK_NAME	schema_name	table	column	referenced_table	referenced_column
	//
	//FK_k_szallitolevel_tetel_szallitolevel1	dbo	k_szallitolevel_tetel	szallitolevel_id	szallitolevel	id
	//FK_k_szallitolevel_tetel_szallitotetel	dbo	k_szallitolevel_tetel	szallitotetel_id	szallitotetel	id
	//
	// column -> referenced_table . referenced_column
	while (query.next()) {
		QString col = query.value("column").toString();
		QString r_tab = query.value("referenced_table").toString();
		QString r_col = query.value("referenced_column").toString();

		QString tmp = "%1 -> %2.%3";
		QString str = tmp.arg(col).arg(r_tab).arg(r_col);

		ui.listWidget_IdegenKulcs->addItem(str);
	}
}



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

QTableWidgetItem* retek2::CreateTableItem(QVariant v){
    auto a = new QTableWidgetItem();
    a->setData(Qt::DisplayRole, v);
    return a;
}

/*!
    \fn retek2::GenerateByText()

    1.step: generates table and record metadata
    2.step: validates specified database by generated metadata
*/



/*!
 \fn retek2::GenerateAll()

 generate files by selected templates
*/
void retek2::GenerateAll() {
	qDebug("GenerateAll");
    if (!tablanev.isEmpty()){
		saveCaptionTabla(tablanev);
        }
	if (ui.checkBox_CClass->isChecked()) {
		qDebug("C# Class");

        auto txt = generateTmp("MVC_CClass.cs");
        zlog.trace(txt);
        SaveAllTextToFile(&txt, zFileNameHelper::getCClassFilename(beallitasok.munkadir,beallitasok.adatbazisNev,tablanev + ".cs"));
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
        SaveAllTextToFile(&txt, getModelFilename(getOsztalynevUpper(tablanev) + ".cs"));
	}
	if (ui.checkBox_Meta->isChecked()) {
		qDebug("ModelMeta");
		auto txt = generateTmp("MVC_ModelMeta.cs");
        SaveAllTextToFile(&txt, getModelFilename(getOsztalynevUpper(tablanev) + "Meta" + ".cs"));
	}

	if (ui.checkBox_Controller->isChecked()) {
		qDebug("Controller");
		auto txt = generateTmp("MVC_Controller.cs");
        SaveAllTextToFile(&txt, getModelFilename(getOsztalynevUpper(tablanev) + "Controller" + ".cs", "Controllers"));
	}

    if (ui.checkBox_DataProvider->isChecked()) {
        qDebug("DataProvider");
        auto txt = generateTmp("MVC_DataProvider.cs");
        SaveAllTextToFile(&txt, getModelFilename(getOsztalynevUpper(tablanev) + "DataProvider" + ".cs", "DataProviders"));
    }

    if (ui.checkBox_View->isChecked()) {
        qDebug("View");
        auto txtIndex = generateTmp("MVC_ViewIndex.cshtml");
        SaveAllTextToFile(&txtIndex, getModelFilename(getOsztalynevLower(tablanev) + "ViewIndex" + ".cshtml", "Views"));

        auto txtAdatlap = generateTmp("MVC_ViewAdatlapDX.cshtml");
        SaveAllTextToFile(&txtAdatlap, getModelFilename(getOsztalynevLower(tablanev) + "ViewAdatlapDX" + ".cshtml", "Views"));
    }

	if (ui.checkBox_ViewIndex->isChecked()) {
		qDebug("Index");
		auto txt = generateTmp("MVC_Index.cshtml");
		SaveAllTextToFile(&txt, getModelFilename(getOsztalynevLower(tablanev) + "ViewIndex" + ".cshtml"));
	}

	if (ui.checkBox_ViewEdit->isChecked()) {
		qDebug("Edit");
        auto txt = generateTmp("MVC_Edit.cshtml");
		SaveAllTextToFile(&txt, getModelFilename(getOsztalynevLower(tablanev) + "ViewEdit" + ".cshtml"));
	}

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

    }
    //else{
    //    qDebug("Nincs tabla kivalasztva!");
    //}

}

QString retek2::getTemplateFilename(QString tfname) {
    auto fn = QString(beallitasok.tmpDir+R"(\%1\%2)").arg(beallitasok.adatbazisNev).arg(tfname);
    if(QFileInfo(fn).exists())
        return fn;
    else{
        qDebug() << "nincs project templatefile:"+ fn;
        fn = QString(beallitasok.tmpDir+R"(\%1)").arg(tfname);
        qDebug()<<"nincs defaule template:" +fn;
        if(QFileInfo(fn).exists())
            return fn;
        else{
            qDebug() << "nincs default templatefile:"+ fn;
            }
        }
    return NULL;
}

QString retek2::getModelFilename(QString tfname, QString dirname) {
    auto e = QString(beallitasok.munkadir+R"(\%2\%1)").arg(dirname).arg(beallitasok.adatbazisNev);
    QDir d(e);if(!d.exists()){d.mkpath(d.absolutePath());}

    e += "\\"+tfname;
    qDebug()<<e;
    return e;
}





QString retek2::generateTmp(QString tmp_file) {
	qDebug() << tmp_file;
    auto tmp_fn = getTemplateFilename(tmp_file);

    if(tmp_fn == NULL) {zLog::ShowDialog("nincs sablon: "+ tmp_file);return "";}

    QString tmp = ReadAllTextFromFile(tmp_fn);

    tokenize(&tmp, nullptr, 0);

	return tmp;
}

QString retek2::getContextNev(void) {
    return "context1";
}

//QString retek2::getAdatbazisnev(void) {
//    return beallitasok.adatbazisNev;
//}

QString retek2::getOsztalynevUpper(QString tnev) {
	auto o = tnev.split('_');

	for (int i = 0; i < o.length(); i++) o[i][0] = o[i][0].toUpper();

	return o.join("");
}

QString retek2::getOsztalynevLower(QString tnev) {
	return tnev;
}


QString retek2::get_liNev(QString O) {
	auto o = O.toLower();

	if (o.endsWith("_id"))
		return o.left(o.length() - 3);
	else if (o.endsWith("id"))
		return o.left(o.length() - 2);
	return o;
}


/*
QString retek2::get_epList() {
	int rows = ui.tableWidget_MezoLista->rowCount();
	QString proplist = "";

	for (int i = 0; i < rows; i++) {
		QTableWidgetItem *item_colName = ui.tableWidget_MezoLista->item(i, C_ix_colName);
		QTableWidgetItem *item_colType = ui.tableWidget_MezoLista->item(i, C_ix_colType);
		QTableWidgetItem *item_len = ui.tableWidget_MezoLista->item(i, C_ix_dlen);
		QTableWidgetItem *item_isnullable = ui.tableWidget_MezoLista->item(i, C_ix_nullable);
		QTableWidgetItem *item_Caption = ui.tableWidget_MezoLista->item(i, C_ix_Caption);
		QString e1;

		int len = item_len->text().toInt();
		bool isnullable = toBool(item_isnullable->text());
		auto mezoType = item_colType->text();
		auto epType = getePropType(mezoType, len, isnullable);
		auto mezonev = item_colName->text();
		auto osztalyNev = getOsztalynev(mezonev);

		e1 = QString("entity.Property(e = > e.%1)\n").arg(osztalyNev);

		if (!isnullable)
			e1 += QString(".IsRequired()\n");

		e1 += QString(".HasColumnName(\"%1\");\n").arg(mezonev);

		if (!mezoType.startsWith("int"))
			e1 += QString(".HasColumnType(\"%1\");\n").arg(epType);

		proplist += e1 + "// " + item_colName->text() + "\n";
	}

	return proplist;
}
*/
/*
QString retek2::get_fkList(QString osztalynev) {
	int rows = ui.listWidget_IdegenKulcs->count();

	QString e = "";

	for (int i = 0; i < rows; i++) {
		QListWidgetItem *item = ui.listWidget_IdegenKulcs->item(i);

		auto itx = item->text().split("->");
		auto itx2 = itx[1].split(".");

		auto mezonev = itx[0];
		auto to_tablaNev = itx2[0];
		auto to_tablaMezo = itx2[1];
		// listanv: Hely -> class Szallitolevel -> public virtual TorzsHely Hely { get; set; }
		// osztlynv : Szallitolevel
		// PropNv a meznvbl: HelyId
		auto propNev = getOsztalynev(mezonev);
		auto liNev = get_liNev(propNev);


		e += QString("entity.HasOne(d => d.%1)\n.WithMany(p = > p.%2)\n.HasForeignKey(d = > d.%3)\n.OnDelete(DeleteBehavior.Restrict)\n.HasConstraintName(\"%4\");\n")
			.arg(liNev)
			.arg(osztalynev)
			.arg(propNev)
			.arg("Fk11");
	}
	//
	// entity.HasOne(d => d.Hely)
	//				.WithMany(p => p.Szallitolevel)
	//				.HasForeignKey(d => d.HelyId)
	//				.OnDelete(DeleteBehavior.Restrict)
	//				.HasConstraintName("FK_hely_id");
	//

	return e;
}
*/
/*
QString retek2::getReq(QString tmp, QMap<QString, QVariant> *map) {
	auto a = map->value("isnullable").toBool(); //isnullable - elfogad null-t?
	if (a == false) { // nem
		return tmp.mid(2, tmp.length() - 4);
	}
	else
		return "";
}*/



QString retek2::ReadAllTextFromFile(QString filename) {
	QFile f(filename);
	if (!f.open(QFile::ReadOnly | QFile::Text)) return "";
	return QTextStream(&f).readAll();
}

void retek2::SaveAllTextToFile(QString *txt, QString fn) {
	QFile f(fn);

    if (!f.open(QIODevice::WriteOnly | QIODevice::Text)){
        zLog::ShowDialog("nem menthet: "+fn);
        return;
        }

	QTextStream out(&f);
	QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));

	out.setCodec(QTextCodec::codecForName("UTF-8"));
	out.setGenerateByteOrderMark(true);
	out << (*txt).toUtf8();
	f.close();
}

bool retek2::toBool(QString ez) {
	if (ez.toLower() == "yes" || ez.toLower() == "true") return true; else return false;
}



/*!
Szmolni kell, hny tokent sikerl feloldani. az utols lps az, amelyikben mr nem sikerl tokent feloldani, teht marad mg token, de egyet sem sikerl kzlk feloldani.
Ezzel el lehet kerlni, hogy:
1. a templateban logiktlan szerkezetek jelenjenek meg
2. a kd byonyolultsgn lehet cskkenteni.

ha for ciklust hasznlunk, akkor elkerlhet a vgtelen ciklus, s maximalizlhat a template begyazottsga,
for(int i = 10 ... ha elrtk a 10-et, akkor a template too complex
*/

void retek2::tokenize(QString *tmp, QMap<QString, QVariant>*map, int whsp) {
	int szint = 0;
    tokenizeR(tmp, 0, &szint, map, whsp);
}

int retek2::tokenizeR(QString *tmp, int ix, int* szint, QMap<QString, QVariant> *map, int whsp) {
	int ix1 = 0;
    //int ix2 = ix;
    int spc = 0;
	for (int i = ix; i < tmp->length(); i++) {
		if (i == -1) continue;

        if((*tmp)[i] == ' ')
            spc++;
        else if ((*tmp)[i] == '\t')
            spc+=8;

		if ((*tmp)[i] == '<' && (*tmp)[i + 1] == '%' && (*tmp)[i + 2] == '=') {
			(*szint)++;
            i = tokenizeR(tmp, i + 3, szint, map, whsp+spc);
			}
		else if ((*tmp)[i] == '%' && (*tmp)[i + 1] == '>') {
			ix1 = i;
			(*szint)--;
			break;
        } else
        {
            if((*tmp)[i] != ' ')
                spc=0;
        }


		if (*szint > 0) {
		if ((*tmp)[i] == '(' && (*tmp)[i + 1] == '\"') {
			(*szint)++;
            i = tokenizeR(tmp, i + 2, szint, map, whsp+spc);
		}
		else if ((*tmp)[i] == '\"' && (*tmp)[i + 1] == ')') {
			ix1 = i;
			(*szint)--;
			break;
		}
		}
	}
	

	if (*szint == 0 && (ix1 != ix) ) {	
		int l1 = ix1 - ix;

		QString eredmeny = tmp->mid(ix, ix1 - ix);
        QString eredmeny2 = eredmeny;
		QString parameter;

		int ix_p1 = eredmeny.indexOf("(\"");	
		if (ix_p1 > -1) {
			int ix_p2 = eredmeny.indexOf("\")", ix_p1+2);
			if (ix_p2 > -1) {
				parameter = eredmeny.mid(ix_p1 + 2, ix_p2 - ix_p1 -2).trimmed();
				}	
			eredmeny = eredmeny.left(ix_p1).trimmed();
			}
		else
			eredmeny = eredmeny.trimmed();

        eredmeny = getToken(eredmeny, parameter, map, whsp);
		if (eredmeny.isEmpty()) {
			tmp->remove(ix - 3, ix1 - ix + 5);
			ix1 -= l1 + 5;
		}
		else{
			int l2 = eredmeny.length();
			tmp->replace(ix - 3, ix1 - ix + 5, eredmeny);
			ix1 += l2 - l1 - 5;
		}
	}	
	return ix1;
}

QString retek2::getToken(QString token1, QString t2, QMap<QString, QVariant> *map, int whsp) {
	auto t1List = token1.split(' ', QString::SplitBehavior::SkipEmptyParts);
	QString t1 = t1List[0];
	QString t3 = (t1List.length()>1) ? t1List[1] : nullptr;

    if (t1 == "dbname") return beallitasok.adatbazisNev;
	else if (t1 == "tablename") return tablanev;
	else if (t1 == "classname") return getOsztalynevUpper(tablanev);
    else if (t1 == "proplist") return getPropList2(t2, t3, whsp);
    else if (t1 == "propline") return getPropList2(nullptr, t3, whsp);
    else if (t1 == "attrlist") return getAttrList(t2, map, whsp);
	else if (t1 == "contextname") return getContextNev();
	else if (t1 == "newline") return "\n";
	//else if (t1 == "dispnameforprop") return getDispnameforprop(t2, map);
	//else if (t1 == "dispforprop") return getDispforprop(t2, map);
	//dispnameforprop
	//dispforprop
	else if (map) {
		auto a = map->value(t1).toString();
		if (!a.isEmpty())
			return a;
	}
	else
		return "?" + t1 + "?";
	return "!" + t1.toUpper() + "!";
}

/*
tokenelemek feloldsa
*/

QString retek2::getPropList() {
	int rows = ui.tableWidget_MezoLista->rowCount();
	QString proplist = "";

	for (int i = 0; i < rows; i++) {
		QTableWidgetItem *item_colName = ui.tableWidget_MezoLista->item(i, C_ix_colName);
		QTableWidgetItem *item_colType = ui.tableWidget_MezoLista->item(i, C_ix_colType);
		QTableWidgetItem *item_len = ui.tableWidget_MezoLista->item(i, C_ix_dlen);
		QTableWidgetItem *item_isnullable = ui.tableWidget_MezoLista->item(i, C_ix_nullable);
        //QTableWidgetItem *item_Caption = ui.tableWidget_MezoLista->item(i, C_ix_Caption);
		QString e1;

		int len = item_len->text().toInt();
		bool isnullable = toBool(item_isnullable->text());
		auto pType = getPropType(item_colType->text(), len, isnullable);
		auto osztalyNev = getOsztalynevLower(item_colName->text());

		e1 = QString("public %1 %2 { get; set; }").arg(pType).arg(osztalyNev);

		proplist += e1 + "// " + item_colName->text() + "\n";
	}

	return proplist;
}

QString retek2::getAttrList(QString tmp, QMap<QString, QVariant> *map, int whsp) {
	QString  e;
    QString spacer = QString(whsp, ' ');
	if (!map) return "";

	QString ca = map->value("caption").toString();
	
	if(ca.isEmpty())
		ca = map->value("propname").toString();


    if(!e.isEmpty()) e+= spacer;

    e += QString("[Display(Name = \"%1\")]\n").arg(ca);

	bool isNullable = map->value("isnullable").toBool();

	if(!isNullable && ca.toLower() != "id"){
        if(!e.isEmpty()) e+= spacer;
        e += QStringLiteral("[Required(ErrorMessage = \"%1 ktk\")]\n").arg(ca);
	}	

	QString tyo = map->value("proptypeoriginal").toString();
	QString len = map->value("proplen").toString();

	if (!tyo.isEmpty()) {
		if (tyo == "datetime") {
            if(!e.isEmpty()) e+= spacer;
            e += QString("[DataType(DataType.DateTime)]\n[DisplayFormat(DataFormatString = \"{0:yyyy.MM.dd}\", ApplyFormatInEditMode = true)]\n");
		}
		else if( tyo.endsWith("char") && len != "-1") {
            if(!e.isEmpty()) e+= spacer;
            e += QString("[StringLength(%1)]\n").arg(len);
		}
	}

    return  e;
}

QString retek2::getPropList2(QString tmp, QString param, int whsp) {
	int rows = ui.tableWidget_MezoLista->rowCount();
	QString proplist = "";
	QStringList exPropsBynameList;
	if (!param.isEmpty() && param.startsWith('-')) {
		exPropsBynameList = param.right(param.length() - 1).toLower().split(',', QString::SplitBehavior::SkipEmptyParts);
	}

    int j = 0;
	for (int i = 0; i < rows; i++) {        

		QTableWidgetItem *item_colName = ui.tableWidget_MezoLista->item(i, C_ix_colName);
		QTableWidgetItem *item_colType = ui.tableWidget_MezoLista->item(i, C_ix_colType);
		QTableWidgetItem *item_len = ui.tableWidget_MezoLista->item(i, C_ix_dlen);
		QTableWidgetItem *item_isnullable = ui.tableWidget_MezoLista->item(i, C_ix_nullable);
		QTableWidgetItem *item_Caption = ui.tableWidget_MezoLista->item(i, C_ix_Caption);
		
		QString colName = item_colName->text();
		if (exPropsBynameList.contains(colName.toLower())) continue;
		auto propName = getOsztalynevLower(colName);		

		if (!tmp.isEmpty()) {
            QString e1;

			int len = item_len->text().toInt();
			bool isnullable = toBool(item_isnullable->text());
			auto colType = item_colType->text();
			auto propType = getPropType(colType, len, isnullable);
			//auto propName = getOsztalynev(colName);
			QString caption = (item_Caption) ? item_Caption->text() : "";
			QString coltypeName = item_colType->text();

			QMap<QString, QVariant> map;
			map.insert("proptype", propType);
			map.insert("propname_", propName.replace('.','_'));
			map.insert("propname", propName);
			map.insert("isnullable", isnullable);
			map.insert("proplen", len);

			if (dxMap.contains(colType)) {
				map.insert("dxWidget", dxMap.value(colType) );
			}

			if (!caption.isEmpty())
				map.insert("caption", caption);
			map.insert("proptypeoriginal", coltypeName);

			/*
			ha a tmp !-al kezddik, akkor fjlbl jn a template

			!bytype,edit.cshtml : a edit_cshtml sztrbl jn fieldtypeName alapjn
			!bytype,create.cshtml : a create_cshtml sztrbl jn fieldtypeName alapjn
			!bytype,details.cshtml : a details_cshtml sztrbl jn fieldtypeName alapjn

			-- egymsba gyazott mapok kellenek
			*/
			map.insert("controlid", colName+'_'+QString::number(i));

			if (tmp.startsWith('!')) {
				auto templatekeyList = tmp.right(tmp.length() - 1).split(',');
				
				if (templatekeyList.length() > 2 && templatekeyList.indexOf(colName) > -1)
					templatekeyList[0] = "byname";

				QString tmpkRoot = templatekeyList[0]+ '_'+ templatekeyList[1];
				QString tmpkDefault = tmpkRoot + "_default";
				QString tmpk = tmpkRoot;

				if (templatekeyList[0] == "bytype") {
					tmpk += "_" + coltypeName;
				}
				else if (templatekeyList[0] == "byname")
					tmpk += "_" + colName;

				if(tmpMap.contains(tmpk))
                    e1 = tmpMap.value(tmpk);
				else
                    e1 = tmpMap.value(tmpkDefault, "?"+ tmpk +"?");
			}
			else
                e1 = tmp;
			
            tokenize(&e1, &map, whsp);

            proplist += QString(j++==0?0:whsp, ' ') + e1;// +"// " + item_colName->text();
			if (i < rows - 1) proplist += "\n";
		}	
		else {
			//auto propName = getOsztalynev(colName);
			if (!proplist.isEmpty()) proplist += ",";
            proplist += propName;
		}
	}

	return proplist;
}


QString retek2::getPropType(QString tipusnev, int length, bool isnullable) {
	QString pt = typeMap.value(tipusnev).toString();
	if (isnullable && !tipusnev.endsWith("char")) pt += '?';
	return pt;
}

QString retek2::getePropType(QString tipusnev, int length, bool isnullable) {
	QString pt = tipusnev;
	if (length > 0 && tipusnev.endsWith("char")) pt += QString("(%1)").arg(length);
	return pt;
}



//KSzallitolevelTetel.SzallitolevelId -> 

void retek2::on_pushButton_clicked()
{
    beallitasok.getUI();

    zsql.init(beallitasok.driver, beallitasok.server, beallitasok.adatbazisNev, beallitasok.user, beallitasok.password);
    //zsql.createConnection();
    feltoltTabla();
}

/*!
 * \brief retek2::GenerateByText
 *
 * A 2-es tabon megadott szöveg alapján generál táblaszerkezetet
 * amialapján validálja az adatbázisban szereplő táblát.
 */


void retek2::GenerateByText(){
    qDebug("GenerateByText");

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


void retek2::on_pushButton_3_clicked()
{
    qDebug("on_pushButton_3_clicked");

    auto txt = ui.textEdit->toPlainText();
    auto tl = zTable::createTableByText(txt);

    if(tl.length()==0) { zlog.log("nincs egyezés, nincs vizsgálat"); return;}

    zforeach(t,tl){
        ztables.append(*t);
        new QListWidgetItem("txt."+t->tablename, ui.listWidget_tabla);
        }
        /*feltoltMezoLista(tl[0]);
        tablanev = tl[0].tablename;
        }
    else{

        }*/
}
