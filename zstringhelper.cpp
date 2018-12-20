#include "globals.h"
#include "zfilenamehelper.h"
#include "zlog.h"
#include "zstringhelper.h"
#include "ztextfilehelper.h"

#include <QDir>
#include <QRegularExpression>

/*
zStringHelper::zStringHelper()
{

}
*/
const QChar zStringHelper::SEP = ';';
const QString zStringHelper::Empty = QString();
#ifdef Q_OS_LINUX
const QString zStringHelper::NewLine = QStringLiteral("\n");
#elif define(Q_OS_WIN)
const QString zStringHelper::NewLine = QStringLiteral("\r\n");
#endif


const QString zStringHelper::Equals = QStringLiteral("Equals");

const QStringList zStringHelper::TrueStr = {QStringLiteral("true"), QStringLiteral("yes"), QStringLiteral("ok"), QStringLiteral("1")};
const QStringList zStringHelper::FalseStr = {QStringLiteral("false"), QStringLiteral("no"), QStringLiteral("nok"), QStringLiteral("0")};

const QString zStringHelper::HTML_TAGPATTERN = QStringLiteral(R"(<%1.*?>([\s\S]*?)<\/%1>)");
const QString zStringHelper::HTML_TABLE = QStringLiteral("table");
const QString zStringHelper::HTML_TBODY = QStringLiteral("tbody");
const QString zStringHelper::HTML_TR = QStringLiteral("tr");
const QString zStringHelper::HTML_TD = QStringLiteral("td");
const QString zStringHelper::HTML_P= QStringLiteral("p");
const QString zStringHelper::HTML_SPAN = QStringLiteral("span");

bool zStringHelper::toBool(const QString& ezt){
    if(ezt.isEmpty()) return false;    
    return TrueStr.contains(ezt.toLower());
}

QString zStringHelper::boolToString(bool a)
{
    if(a)
    {
        return TrueStr.first();
    }
    return FalseStr.first();
}

QString zStringHelper::boolToString(bool a, const QString& s)
{
    if(a)
    {
        return s;
    }
    return QStringLiteral("Not")+s;
}

/*
 * ponttal elválasztott tagok esetén
*/
QString zStringHelper::toCamelCase(const QString& s)
{
    auto o = s.split('.');

    for (int i = 0; i < o.length(); i++)
        o[i][0] = o[i][0].toUpper();

    return o.join(Empty);
}

//class_nameCamelCase
QString zStringHelper::getclass_nameCamelCase(const QString& tnev) {
    QString t2 = tnev;//.toLower();
    //QString sep = TXT+'.';

    //if(t2.startsWith(sep))
    //    t2 = t2.remove(0,sep.length());


    //auto o = t2.replace('_', '.');//.split('_');

    /*
    for (int i = 0; i < o.length(); i++)
        o[i][0] = o[i][0].toUpper();

    return o.join("");*/

    return toCamelCase(t2.replace('_', '.'));
}

QStringList zStringHelper::toStringList(const QString &s){
    return s.split(QRegExp(QStringLiteral("(\\r\\n)|(\\n\\r)|\\r|\\n")), QString::SkipEmptyParts);
}

// (?:\".*\")|(?:[\d.]+)
QRegularExpression zStringHelper::r_string_or_number = QRegularExpression(QStringLiteral(R"((?:\".*\")|(?:[^\p{L}][\d.]+))"), QRegularExpression::MultilineOption|QRegularExpression::UseUnicodePropertiesOption);

bool zStringHelper::isclass_name(const QString& str){
    auto m = r_string_or_number.match(str);
    auto i = !m.hasMatch();    // ha nincs egyezés, nem konstans
    return i;
}

//littleM|oUsE littleM|iCe
QString zStringHelper::caseFixer(QString minta, QString ezt){
    int l = (minta.length()<ezt.length())?minta.length():ezt.length();
    for(int i=0;i<l;i++){
        QChar c = minta[i];
        QChar cl= c.toLower();
        if(cl==ezt[i].toLower()){
            if(c.isUpper()){
                ezt[i]=c;
            }
            else{
                ezt[i]=cl;
            }
        }
        else
            break;
    }

    return ezt;
}

const QString zStringHelper::p_filename = QStringLiteral(R"((?:file:\/{2})?((?:[a-zA-Z]\:)?(?:[\\\/][\w._]+)+))");
const QRegularExpression zStringHelper::r_filename = QRegularExpression(p_filename, QRegularExpression::MultilineOption|QRegularExpression::UseUnicodePropertiesOption);
// ha fájlnévvel kezdődik, 
QStringList zStringHelper::getFilePaths(const QString& txt, const QStringList& fileExtFilter){
    auto i_filename = r_filename.globalMatch(txt);
    
    QStringList txtlist;
    QStringList fileNameFilter;
    zforeach(f, fileExtFilter){
        fileNameFilter << "*."+(*f);
    }

    while(i_filename.hasNext()){
        QRegularExpressionMatch m_filename = i_filename.next();

        QString filePath = m_filename.captured(1);

        auto filePathInfo = QFileInfo(filePath);


        if(filePathInfo.isFile()){
           txtlist << filePath;
        }
        else if(filePathInfo.isDir()){
            QStringList files = zFileNameHelper::FindFileNameInDir(filePath, Empty, fileNameFilter);
            if(!files.isEmpty()){
                txtlist << files;
                }
            //auto list2 = getTextFromFiles(m_filename.captured(0), fileNameFilter);
        }
        else{

        }

//        / QStringList files = zFileNameHelper::FindFileNameInDir(path, "Data", class_nameFilter);


    }

    return txtlist;
}

QString zStringHelper::zNormalize(const QString& c){
    return c.normalized(QString::NormalizationForm_D).replace(QRegExp("[^a-zA-Z0-9_\\s]"), Empty).replace(' ', '_').toLower();
}


QRegularExpression zStringHelper::getHTMLRegExp(const QString &s)
{
    auto r = QRegularExpression(HTML_TAGPATTERN.arg(s));
    return r;
}


//QString zStringHelper::encodeEntities( const QString& src, const QString& force=QString() )
//{
//    QString tmp(src);
//    uint len = tmp.length();
//    uint i = 0;
//    while( i<len )
//    {
//        if( tmp[i].unicode() > 128 || force.contains(tmp[i]) ){
//            QString rp = "&#"+QString::number(tmp[i].unicode())+"},//;
//            tmp.replace(i,1,rp);
//            len += rp.length()-1;
//            i += rp.length();
//        }else{
//            ++i;
//        }
//    }
//    return tmp;
//}



//QString zStringHelper::decodeEntities( const QString& src )
//{
//    QString ret(src);
//    QRegExp re("&#([0-9]+)},//);
//    re.setMinimal(true);

//    int pos = 0;
//    while( (pos = re.indexIn(src, pos)) != -1 )
//    {
//        ret = ret.replace(re.cap(0), QChar(re.cap(1).toInt(0,10)));
//        pos += re.matchedLength();
//    }
//    return ret;
//}


QString zStringHelper::HtmlDecode(const QString &value)
{
    if (value.isEmpty())
    {
        return Empty;
    }

    if (!StringRequiresHtmlDecoding(value))
    {
        return value;
    }

    QString e = HtmlDecode2(value);
    return e;
}

bool zStringHelper::StringRequiresHtmlDecoding(const QString& s)
{
    if (getHtmlDecodeConformance() == UnicodeDecodingConformance::Compat)
    {
        // this string requires html decoding only if it contains '&'
        return (s.indexOf('&') >= 0);
    }

    // this string requires html decoding if it contains '&' or a surrogate character
    for(auto c : s)
    {
        if (c == '&' || c.isSurrogate())
        {
            return true;
        }
    }
    return false;
}

const QChar zStringHelper::_htmlEntityEndingChars[] =  {';', '&' };
const QSet<QChar> zStringHelper::htmlEntityEndings =  {';', '&' };

//chars[3] = '\u0058';   // Unicode
const QChar zStringHelper::HIGH_SURROGATE_START(0xD800);// '\uD800';
const QChar zStringHelper::LOW_SURROGATE_START(0xDC00);
const QChar zStringHelper::LOW_SURROGATE_END(0xDFFF);

const int zStringHelper::UNICODE_PLANE00_END = 0x00FFFF;
const int zStringHelper::UNICODE_PLANE01_START = 0x10000;
const int zStringHelper::UNICODE_PLANE16_END = 0x10FFFF;

zStringHelper::UnicodeDecodingConformance zStringHelper::getHtmlDecodeConformance()
{
    return UnicodeDecodingConformance::Strict;
}

QString zStringHelper::HtmlDecode2(const QString &value)
{
    QString output;

    if (value.isEmpty())
    {
        return Empty;
    }

//    if (output == null) {
//        throw new ArgumentNullException("output");
//    }

//    if (!StringRequiresHtmlDecoding(value)) {
//        output.Write(value);        // good as is
//        return;
//    }

    UnicodeDecodingConformance decodeConformance = getHtmlDecodeConformance();
    int l = value.length();
    int _htmlEntityEndingCharsLen = sizeof(_htmlEntityEndingChars)/sizeof(_htmlEntityEndingChars[0]);
    QChar ch;
    for (int i = 0; i < l; i++) {
        ch = value[i];

        if (ch == '&') {
            // We found a '&'. Now look for the next ';' or '&'. The idea is that
            // if we find another '&' before finding a ';', then this is not an entity,
            // and the next '&' might start a real entity (VSWhidbey 275184)
//            int index = -1;
//            for(int j=0;j<_htmlEntityEndingCharsLen;j++)
//            {
//                index = value.indexOf(_htmlEntityEndingChars[j], i + 1);
//                if(index > -1)
//                {
//                    break;
//                }
//            }

            int index = -1;
            for(int j=0;j<12;j++)
            {
                auto ix = i+1+j;
                if(htmlEntityEndings.contains(value[ix]))
                {
                    index = ix;
                    break;
                }
            }

            if (index > 0 && value[index] == ';')
            {
                QString entity = value.mid(i + 1, index - i - 1);

                if (entity.length() > 1 && entity[0] == '#') {
                    // The # syntax can be in decimal or hex, e.g.
                    //      &#229;  */ decimal
                    //      &#xE5;  */ same char in hex
                    // See http://www.w3.org/TR/REC-html40/charset.html#entities

                    bool pOk;
                    quint16 pv;
                    if(entity[1] == 'x' || entity[1] == 'X')
                    {
                        //hexadecimális
                        QString ep = entity.mid(2);
                        pv = ep.toUShort(&pOk, 16);
                    }
                    else
                    {
                        //decimális
                        QString ep = entity.mid(1);
                        pv = ep.toUShort(&pOk);
                    }

                    if (pOk)
                    {
                        switch (decodeConformance)
                        {
                            case UnicodeDecodingConformance::Strict:
                                // decoded character must be U+0000 .. U+10FFFF, excluding surrogates
                                pOk = ((pv < HIGH_SURROGATE_START) || (LOW_SURROGATE_END < pv && pv <= UNICODE_PLANE16_END));
                                break;

                            case UnicodeDecodingConformance::Compat:
                                // decoded character must be U+0001 .. U+FFFF
                                // null chars disallowed for compat with 4.0
                                pOk = (0 < pv && pv <= UNICODE_PLANE00_END);
                                break;

                            case UnicodeDecodingConformance::Loose:
                                // decoded character must be U+0000 .. U+10FFFF
                                pOk = (pv <= UNICODE_PLANE16_END);
                                break;

                            default:
                                zInfo(QStringLiteral("Should never get here!"));
                                pOk = false;
                                break;
                        }
                    }

                    if(pOk)
                    {
                        if (pv <= UNICODE_PLANE00_END)
                        {
                            // single character                            
                            output += QChar(pv);
                        }
                        else
                        {
                            // multi-character
                            QChar leadingSurrogate, trailingSurrogate;                            
                            ConvertSmpToUtf16(pv, leadingSurrogate, trailingSurrogate);

                            output += leadingSurrogate;
                            output += trailingSurrogate;
                        }

                        i = index; // already looked at everything until semicolon
                        continue;
                    }
                }
                else
                {
                    i = index; // already looked at everything until semicolon

                    QChar entityChar = HtmlEntityLookup(entity);
                    if (entityChar.isNull())
                    {
                        output += '&'+entity+';';
                        continue;
                    }                    
                    ch = entityChar;
                }

            }
        }
        output += ch;
        //output.Write(ch);
    }
    return output;
}


void zStringHelper::ConvertSmpToUtf16(uint smpChar, QChar leadingSurrogate, QChar trailingSurrogate) {
            //zInfo(UNICODE_PLANE01_START <= smpChar && smpChar <= UNICODE_PLANE16_END);

            auto utf32 = static_cast<int>(smpChar - zStringHelper::UNICODE_PLANE01_START);
            leadingSurrogate = QChar((utf32 / 0x400) + zStringHelper::HIGH_SURROGATE_START.unicode());
            trailingSurrogate = QChar((utf32 % 0x400) + zStringHelper::LOW_SURROGATE_START.unicode());
        }


QChar zStringHelper::HtmlEntityLookup(const QString& e)
{
    quint16 a;
    a = HtmlNamedEntitiesLatin1[e];
    if(!a){
        a = HtmlNamedEntitiesSymbolic[e];
    }
    if(!a){
        a = HtmlNamedEntitiesSpecial[e];
    }

    return {a};
}

//https://www.w3.org/TR/WD-html40-970708/sgml/entities.html

const QMap<QString,quint16> zStringHelper::HtmlNamedEntitiesLatin1{
    //ENTITIES Full Latin 1
    {"nbsp",160}, /* no-break space */
    {"iexcl",161}, /* inverted exclamation mark */
    {"cent",162}, /* cent sign */
    {"pound",163}, /* pound sterling sign */
    {"curren",164}, /* general currency sign */
    {"yen",165}, /* yen sign */
    {"brvbar",166}, /* broken (vertical) bar */
    {"sect",167}, /* section sign */
    {"uml",168}, /* umlaut (dieresis) */
    {"copy",169}, /* copyright sign */
    {"ordf",170}, /* ordinal indicator, feminine */
    {"laquo",171}, /* angle quotation mark, left */
    {"not",172}, /* not sign */
    {"shy",173}, /* soft hyphen */
    {"reg",174}, /* registered sign */
    {"macr",175}, /* macron */
    {"deg",176}, /* degree sign */
    {"plusmn",177}, /* plus-or-minus sign */
    {"sup2",178}, /* superscript two */
    {"sup3",179}, /* superscript three */
    {"acute",180}, /* acute accent */
    {"micro",181}, /* micro sign */
    {"para",182}, /* pilcrow (paragraph sign) */
    {"middot",183}, /* middle dot */
    {"cedil",184}, /* cedilla */
    {"sup1",185}, /* superscript one */
    {"ordm",186}, /* ordinal indicator, masculine */
    {"raquo",187}, /* angle quotation mark, right */
    {"frac14",188}, /* fraction one-quarter */
    {"frac12",189}, /* fraction one-half */
    {"frac34",190}, /* fraction three-quarters */
    {"iquest",191}, /* inverted question mark */
    {"Agrave",192}, /* capital A, grave accent */
    {"Aacute",193}, /* capital A, acute accent */
    {"Acirc",194}, /* capital A, circumflex accent */
    {"Atilde",195}, /* capital A, tilde */
    {"Auml",196}, /* capital A, dieresis or umlaut mark */
    {"Aring",197}, /* capital A, ring */
    {"AElig",198}, /* capital AE diphthong (ligature) */
    {"Ccedil",199}, /* capital C, cedilla */
    {"Egrave",200}, /* capital E, grave accent */
    {"Eacute",201}, /* capital E, acute accent */
    {"Ecirc",202}, /* capital E, circumflex accent */
    {"Euml",203}, /* capital E, dieresis or umlaut mark */
    {"Igrave",204}, /* capital I, grave accent */
    {"Iacute",205}, /* capital I, acute accent */
    {"Icirc",206}, /* capital I, circumflex accent */
    {"Iuml",207}, /* capital I, dieresis or umlaut mark */
    {"ETH",208}, /* capital Eth, Icelandic */
    {"Ntilde",209}, /* capital N, tilde */
    {"Ograve",210}, /* capital O, grave accent */
    {"Oacute",211}, /* capital O, acute accent */
    {"Ocirc",212}, /* capital O, circumflex accent */
    {"Otilde",213}, /* capital O, tilde */
    {"Ouml",214}, /* capital O, dieresis or umlaut mark */
    {"times",215}, /* multiply sign */
    {"Oslash",216}, /* capital O, slash */
    {"Ugrave",217}, /* capital U, grave accent */
    {"Uacute",218}, /* capital U, acute accent */
    {"Ucirc",219}, /* capital U, circumflex accent */
    {"Uuml",220}, /* capital U, dieresis or umlaut mark */
    {"Yacute",221}, /* capital Y, acute accent */
    {"THORN",222}, /* capital THORN, Icelandic */
    {"szlig",223}, /* small sharp s, German (sz ligature) */
    {"agrave",224}, /* small a, grave accent */
    {"aacute",225}, /* small a, acute accent */
    {"acirc",226}, /* small a, circumflex accent */
    {"atilde",227}, /* small a, tilde */
    {"auml",228}, /* small a, dieresis or umlaut mark */
    {"aring",229}, /* small a, ring */
    {"aelig",230}, /* small ae diphthong (ligature) */
    {"ccedil",231}, /* small c, cedilla */
    {"egrave",232}, /* small e, grave accent */
    {"eacute",233}, /* small e, acute accent */
    {"ecirc",234}, /* small e, circumflex accent */
    {"euml",235}, /* small e, dieresis or umlaut mark */
    {"igrave",236}, /* small i, grave accent */
    {"iacute",237}, /* small i, acute accent */
    {"icirc",238}, /* small i, circumflex accent */
    {"iuml",239}, /* small i, dieresis or umlaut mark */
    {"eth",240}, /* small eth, Icelandic */
    {"ntilde",241}, /* small n, tilde */
    {"ograve",242}, /* small o, grave accent */
    {"oacute",243}, /* small o, acute accent */
    {"ocirc",244}, /* small o, circumflex accent */
    {"otilde",245}, /* small o, tilde */
    {"ouml",246}, /* small o, dieresis or umlaut mark */
    {"divide",247}, /* divide sign */
    {"oslash",248}, /* small o, slash */
    {"ugrave",249}, /* small u, grave accent */
    {"uacute",250}, /* small u, acute accent */
    {"ucirc",251}, /* small u, circumflex accent */
    {"uuml",252}, /* small u, dieresis or umlaut mark */
    {"yacute",253}, /* small y, acute accent */
    {"thorn",254}, /* small thorn, Icelandic */
    {"yuml",255}, /* small y, dieresis or umlaut mark */
};

const QMap<QString,quint16> zStringHelper::HtmlNamedEntitiesSymbolic{
    /* Latin Extended-B */
    {"fnof",402}, /* latin small f with hook, =function, =florin, u+0192 ISOtech */

    /* Greek */
    {"Alpha",913}, /* greek capital letter alpha,  u+0391 */
    {"Beta",914}, /* greek capital letter beta,  u+0392 */
    {"Gamma",915}, /* greek capital letter gamma,  u+0393 ISOgrk3 */
    {"Delta",916}, /* greek capital letter delta,  u+0394 ISOgrk3 */
    {"Epsilon",917}, /* greek capital letter epsilon,  u+0395 */
    {"Zeta ",918}, /* greek capital letter zeta,  u+0396 */
    {"Eta",919}, /* greek capital letter eta,  u+0397 */
    {"Theta",920}, /* greek capital letter theta,  u+0398 ISOgrk3 */
    {"Iota",921}, /* greek capital letter iota,  u+0399 */
    {"Kappa",922}, /* greek capital letter kappa,  u+039A */
    {"Lambda",923}, /* greek capital letter lambda,  u+039B ISOgrk3 */
    {"Mu",924}, /* greek capital letter mu,  u+039C */
    {"Nu",925}, /* greek capital letter nu,  u+039D */
    {"Xi",926}, /* greek capital letter xi,  u+039E ISOgrk3 */
    {"Omicron",927}, /* greek capital letter omicron,  u+039F */
    {"Pi",928}, /* greek capital letter pi,  u+03A0 ISOgrk3 */
    {"Rho",929}, /* greek capital letter rho,  u+03A1 */
    /* (there is no Sigmaf, and no u+03A2 character either) */
    {"Sigma",931}, /* greek capital letter sigma,  u+03A3 ISOgrk3 */
    {"Tau",932}, /* greek capital letter tau,  u+03A4 */
    {"Upsilon",933}, /* greek capital letter upsilon,  u+03A5 ISOgrk3 */
    {"Phi",934}, /* greek capital letter phi,  u+03A6 ISOgrk3 */
    {"Chi",935}, /* greek capital letter chi,  u+03A7 */
    {"Psi",936}, /* greek capital letter psi,  u+03A8 ISOgrk3 */
    {"Omega",937}, /* greek capital letter omega,  u+03A9 ISOgrk3 */

    {"alpha",945}, /* greek small letter alpha, u+03B1 ISOgrk3 */
    {"beta",946}, /* greek small letter beta,  u+03B2 ISOgrk3 */
    {"gamma",947}, /* greek small letter gamma,  u+03B3 ISOgrk3 */
    {"delta",948}, /* greek small letter delta,  u+03B4 ISOgrk3 */
    {"epsilon",949}, /* greek small letter epsilon,  u+03B5 ISOgrk3 */
    {"zeta",950}, /* greek small letter zeta,  u+03B6 ISOgrk3 */
    {"eta",951}, /* greek small letter eta,  u+03B7 ISOgrk3 */
    {"theta",952}, /* greek small letter theta,  u+03B8 ISOgrk3 */
    {"iota",953}, /* greek small letter iota,  u+03B9 ISOgrk3 */
    {"kappa",954}, /* greek small letter kappa,  u+03BA ISOgrk3 */
    {"lambda",955}, /* greek small letter lambda,  u+03BB ISOgrk3 */
    {"mu",956}, /* greek small letter mu,  u+03BC ISOgrk3 */
    {"nu",957}, /* greek small letter nu,  u+03BD ISOgrk3 */
    {"xi",958}, /* greek small letter xi,  u+03BE ISOgrk3 */
    {"omicron",959}, /* greek small letter omicron,  u+03BF NEW */
    {"pi",960}, /* greek small letter pi,  u+03C0 ISOgrk3 */
    {"rho",961}, /* greek small letter rho,  u+03C1 ISOgrk3 */
    {"sigmaf",962}, /* greek small letter final sigma,  u+03C2 ISOgrk3 */
    {"sigma",963}, /* greek small letter sigma,  u+03C3 ISOgrk3 */
    {"tau",964}, /* greek small letter tau,  u+03C4 ISOgrk3 */
    {"upsilon",965}, /* greek small letter upsilon,  u+03C5 ISOgrk3 */
    {"phi",966}, /* greek small letter phi,  u+03C6 ISOgrk3 */
    {"chi",967}, /* greek small letter chi,  u+03C7 ISOgrk3 */
    {"psi",968}, /* greek small letter psi,  u+03C8 ISOgrk3 */
    {"omega",969}, /* greek small letter omega,  u+03C9 ISOgrk3 */
    {"thetasym",977}, /* greek small letter theta symbol,  u+03D1 NEW */
    {"upsih",978}, /* greek upsilon with hook symbol,  u+03D2 NEW */
    {"piv",982}, /* greek pi symbol,  u+03D6 ISOgrk3 */

    /* General Punctuation */
    {"bull",8226}, /* bullet, =black small circle, u+2022 ISOpub  */
    /* bullet is NOT the same as bullet operator, u+2219 */
    {"hellip",8230}, /* horizontal ellipsis, =three dot leader, u+2026 ISOpub  */
    {"prime",8242}, /* prime, =minutes, =feet, u+2032 ISOtech */
    {"Prime",8243}, /* double prime, =seconds, =inches, u+2033 ISOtech */
    {"oline",8254}, /* overline, =spacing overscore, u+203E NEW */
    {"frasl",8260}, /* fraction slash, u+2044 NEW */

    /* Letterlike Symbols */
    {"weierp",8472}, /* script capital P, =power set, =Weierstrass p, u+2118 ISOamso */
    {"image",8465}, /* blackletter capital I, =imaginary part, u+2111 ISOamso */
    {"real",8476}, /* blackletter capital R, =real part symbol, u+211C ISOamso */
    {"trade",8482}, /* trade mark sign, u+2122 ISOnum */
    {"alefsym",8501}, /* alef symbol, =first transfinite cardinal, u+2135 NEW */

    /* alef symbol is NOT the same as hebrew letter alef, u+05D0 although the same glyph
     * could be used to depict both characters */

    /* Arrows */
    {"larr",8592}, /* leftwards arrow, u+2190 ISOnum */
    {"uarr",8593}, /* upwards arrow, u+2191 ISOnum*/
    {"rarr",8594}, /* rightwards arrow, u+2192 ISOnum */
    {"darr",8595}, /* downwards arrow, u+2193 ISOnum */
    {"harr",8596}, /* left right arrow, u+2194 ISOamsa */
    {"crarr",8629}, /* downwards arrow with corner leftwards, =carriage return, u+21B5 NEW */
    {"lArr",8656}, /* leftwards double arrow, u+21D0 ISOtech */

    /* Unicode does not say that lArr is the same as the 'is implied by' arrow but also
         does not have any other character for that function. So ? lArr can be used for
         'is implied by' as ISOtech suggests */

    {"uArr",8657}, /* upwards double arrow, u+21D1 ISOamsa */
    {"rArr",8658}, /* rightwards double arrow, u+21D2 ISOtech */

    /* Unicode does not say this is the 'implies' character but does not have another
         character with this function so ? rArr can be used for 'implies' as ISOtech suggests */

    {"dArr",8659}, /* downwards double arrow, u+21D3 ISOamsa */
    {"hArr",8660}, /* left right double arrow, u+21D4 ISOamsa */

    /* Mathematical Operators */
    {"forall",8704}, /* for all, u+2200 ISOtech */
    {"part",8706}, /* partial differential, u+2202 ISOtech  */
    {"exist",8707}, /* there exists, u+2203 ISOtech */
    {"empty",8709}, /* empty set, =null set, =diameter, u+2205 ISOamso */
    {"nabla",8711}, /* nabla, =backward difference, u+2207 ISOtech */
    {"isin",8712}, /* element of, u+2208 ISOtech */
    {"notin",8713}, /* not an element of, u+2209 ISOtech */
    {"ni",8715}, /* contains as member, u+220B ISOtech */

    /* should there be a more memorable name than 'ni'? */

    {"prod",8719}, /* n-ary product, =product sign, u+220F ISOamsb */

    /* prod is NOT the same character as u+03A0 'greek capital letter pi' though the same
         glyph might be used for both */

    {"sum",8721}, /* n-ary sumation, u+2211 ISOamsb */

    /* sum is NOT the same character as u+03A3 'greek capital letter sigma' though the same
         glyph might be used for both */

    {"minus",8722}, /* minus sign, u+2212 ISOtech */
    {"lowast",8727}, /* asterisk operator, u+2217 ISOtech */
    {"radic",8730}, /* square root, =radical sign, u+221A ISOtech */
    {"prop",8733}, /* proportional to, u+221D ISOtech */
    {"infin",8734}, /* infinity, u+221E ISOtech */
    {"ang",8736}, /* angle, u+2220 ISOamso */
    {"and",8869}, /* logical and, =wedge, u+2227 ISOtech */
    {"or",8870}, /* logical or, =vee, u+2228 ISOtech */
    {"cap",8745}, /* intersection, =cap, u+2229 ISOtech */
    {"cup",8746}, /* union, =cup, u+222A ISOtech */
    {"int",8747}, /* integral, u+222B ISOtech */
    {"there4",8756}, /* therefore, u+2234 ISOtech */
    {"sim",8764}, /* tilde operator, =varies with, =similar to, u+223C ISOtech */

    /* tilde operator is NOT the same character as the tilde, u+007E, although the same
         glyph might be used to represent both  */

    {"cong",8773}, /* approximately equal to, u+2245 ISOtech */
    {"asymp",8776}, /* almost equal to, =asymptotic to, u+2248 ISOamsr */
    {"ne",8800}, /* not equal to, u+2260 ISOtech */
    {"equiv",8801}, /* identical to, u+2261 ISOtech */
    {"le",8804}, /* less-than or equal to, u+2264 ISOtech */
    {"ge",8805}, /* greater-than or equal to, u+2265 ISOtech */
    {"sub",8834}, /* subset of, u+2282 ISOtech */
    {"sup",8835}, /* superset of, u+2283 ISOtech */

    /* note that nsup, 'not a superset of, u+2283' is not covered by the Symbol font
         encoding and is not included. Should it be, for symmetry? It is in ISOamsn  */

    {"nsub",8836}, /* not a subset of, u+2284 ISOamsn */
    {"sube",8838}, /* subset of or equal to, u+2286 ISOtech */
    {"supe",8839}, /* superset of or equal to, u+2287 ISOtech */
    {"oplus",8853}, /* circled plus, =direct sum, u+2295 ISOamsb */
    {"otimes",8855}, /* circled times, =vector product, u+2297 ISOamsb */
    {"perp",8869}, /* up tack, =orthogonal to, =perpendicular, u+22A5 ISOtech */
    {"sdot",8901}, /* dot operator, u+22C5 ISOamsb */

    /* dot operator is NOT the same character as u+00B7 middle dot */

    /* Miscellaneous Technical */
    {"lceil",8968}, /* left ceiling, =apl upstile, u+2308, ISOamsc  */
    {"rceil",8969}, /* right ceiling, u+2309, ISOamsc  */
    {"lfloor",8970}, /* left floor, =apl downstile, u+230A, ISOamsc  */
    {"rfloor",8971}, /* right floor, u+230B, ISOamsc  */
    {"lang",9001}, /* left-pointing angle bracket, =bra, u+2329 ISOtech */

    /* lang is NOT the same character as u+003C 'less than'
         or u+2039 'single left-pointing angle quotation mark' */

    {"rang",9002}, /* right-pointing angle bracket, =ket, u+232A ISOtech */

    /* rang is NOT the same character as u+003E 'greater than'
         or u+203A 'single right-pointing angle quotation mark' */

    /* Geometric Shapes */
    {"loz",9674}, /* lozenge, u+25CA ISOpub */

    /* Miscellaneous Symbols */

    {"spades",9824}, /* black spade suit, u+2660 ISOpub - black here seems to mean filled as opposed to hollow */
    {"clubs",9827}, /* black club suit, =shamrock, u+2663 ISOpub */
    {"hearts",9829}, /* black heart suit, =valentine, u+2665 ISOpub */
    {"diams",9830}, /* black diamond suit, u+2666 ISOpub */
};

const QMap<QString,quint16> zStringHelper::HtmlNamedEntitiesSpecial{
    /* C0 Controls and Basic Latin */
    {"quot",34}, /* quotation mark, =apl quote, u+0022 ISOnum */
    {"amp",38}, /* ampersand, u+0026 ISOnum */
    {"lt",60}, /* less-than sign, u+003C ISOnum */
    {"gt",62}, /* greater-than sign, u+003E ISOnum */

    /* Latin Extended-A */
    {"OElig",338}, /* latin capital ligature oe, u+0152 ISOlat2 */
    {"oelig",339}, /* latin small ligature oe, u+0153 ISOlat2 */

    /* ligature is a misnomer, this is a separate character in some languages */

    {"Scaron",352}, /* latin capital letter s with caron, u+0160 ISOlat2 */
    {"scaron",353}, /* latin small letter s with caron, u+0161 ISOlat2 */
    {"Yuml",376}, /* latin capital letter y with diaeresis, u+0178 ISOlat2 */

    /* Spacing Modifier Letters */
    {"circ    ",710}, /* modifier letter circumflex accent, u+02C6 ISOpub */
    {"tilde   ",732}, /* small tilde, u+02DC ISOdia */

    /* General Punctuation */
    {"ensp",8194}, /* en space, u+2002 ISOpub */
    {"emsp",8195}, /* em space, u+2003 ISOpub */
    {"thinsp",8201}, /* thin space, u+2009 ISOpub */
    {"zwnj",8204}, /* zero width non-joiner, u+200C NEW RFC 2070 */
    {"zwj",8205}, /* zero width joiner, u+200D NEW RFC 2070 */
    {"lrm",8206}, /* left-to-right mark, u+200E NEW RFC 2070 */
    {"rlm",8207}, /* right-to-left mark, u+200F NEW RFC 2070 */
    {"ndash",8211}, /* en dash, u+2013 ISOpub */
    {"mdash",8212}, /* em dash, u+2014 ISOpub */
    {"lsquo",8216}, /* left single quotation mark, u+2018 ISOnum */
    {"rsquo",8217}, /* right single quotation mark, u+2019 ISOnum */
    {"sbquo",8218}, /* single low-9 quotation mark, u+201A NEW */
    {"ldquo",8220}, /* left double quotation mark, u+201C ISOnum */
    {"rdquo",8221}, /* right double quotation mark, u+201D ISOnum */
    {"bdquo",8222}, /* double low-9 quotation mark, u+201E NEW */
    {"dagger",8224}, /* dagger, u+2020 ISOpub */
    {"Dagger",8225}, /* double dagger, u+2021 ISOpub */
    {"permil",8240}, /* per mille sign, u+2030 ISOtech */
    {"lsaquo",8249}, /* single left-pointing angle quotation mark, u+2039 ISO proposed */

    /* lsaquo is proposed but not yet ISO standardised */

    {"rsaquo",8250}, /* single right-pointing angle quotation mark, u+203A ISO proposed */

    /* rsaquo is proposed but not yet ISO standardised */
};
