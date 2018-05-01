#include "zpluralize.h"

#include <QRegularExpression>
#include "globals.h"

zPluralizer::zPluralizer()
{    
//    zPluralizer::_userDictionary = QMap<QString, QString>();
}

QMap<QString, QString> _userDictionary = QMap<QString, QString>();

QRegularExpression IsAlphabetsRegexp("[^a-zA-Z\\s]");

// névmások
QStringList _pronounList = QStringList {
                   "I", "we", "you", "he", "she", "they", "it",
                   "me", "us", "him", "her", "them",
                   "myself", "ourselves", "yourself", "himself", "herself", "itself",
                   "oneself", "oneselves",
                   "my", "our", "your", "his", "their", "its",
                   "mine", "yours", "hers", "theirs", "this", "that", "these", "those",
                   "all", "another", "any", "anybody", "anyone", "anything", "both", "each",
                   "other", "either", "everyone", "everybody", "everything", "most", "much", "nothing",
                   "nobody", "none", "one", "others", "some", "somebody", "someone", "something",
                   "what", "whatever", "which", "whichever", "who", "whoever", "whom", "whomever",
                   "whose",
               };

// nem ragozható/ragozandó szavak
QStringList _uninflectiveWordList = QStringList {
                "bison", "flounder", "pliers", "bream", "gallows", "proceedings",
                "breeches", "graffiti", "rabies", "britches", "headquarters", "salmon",
                "carp", "----", "scissors", "ch----is", "high-jinks", "sea-bass",
                "clippers", "homework", "series", "cod", "innings", "shears", "contretemps",
                "jackanapes", "species", "corps", "mackerel", "swine", "debris", "measles",
                "trout", "diabetes", "mews", "tuna", "djinn", "mumps", "whiting", "eland",
                "news", "wildebeest", "elk", "pincers", "police", "hair", "ice", "chaos",
                "milk", "cotton", "pneumonoultramicroscopicsilicovolcanoconiosis",
                "information", "aircraft", "scabies", "traffic", "corn", "millet", "rice",
                "hay", "----", "tobacco", "cabbage", "okra", "broccoli", "asparagus",
                "lettuce", "beef", "pork", "venison", "mutton",  "cattle", "offspring",
                "molasses", "shambles", "shingles"};

QStringList _uninflectiveSuffixList =QStringList {
        "fish", "ois", "sheep", "deer", "pos", "itis", "ism" };

QStringList _knownPluralWords = QStringList {};

QMap<QString, QString> _irregularPluralsDictionary = QMap<QString, QString> {
                    {"brother", "brothers"}, {"child", "children"},
                    {"cow", "cows"}, {"ephemeris", "ephemerides"}, {"genie", "genies"},
                    {"money", "moneys"}, {"mongoose", "mongooses"}, {"mythos", "mythoi"},
                    {"octopus", "octopuses"}, {"ox", "oxen"}, {"soliloquy", "soliloquies"},
                    {"trilby", "trilbys"}, {"crisis", "crises"}, {"synopsis","synopses"},
                    {"rose", "roses"}, {"gas","gases"}, {"bus", "buses"},
                    {"axis", "axes"},{"memo", "memos"}, {"casino","casinos"},
                    {"silo", "silos"},{"stereo", "stereos"}, {"studio","studios"},
                    {"lens", "lenses"}, {"alias","aliases"},
                    {"pie","pies"}, {"corpus","corpora"},
                    {"viscus", "viscera"},{"hippopotamus", "hippopotami"}, {"trace", "traces"},
                    {"person", "people"}, {"chili", "chilies"}, {"analysis", "analyses"},
                    {"basis", "bases"}, {"neurosis", "neuroses"}, {"oasis", "oases"},
                    {"synthesis", "syntheses"}, {"thesis", "theses"}, {"change", "changes"},
                    {"lie", "lies"}, {"calorie", "calories"}, {"freebie", "freebies"}, {"case", "cases"},
                    {"house", "houses"}, {"valve", "valves"}, {"cloth", "clothes"}, {"tie", "ties"},
                    {"movie", "movies"}, {"bonus", "bonuses"}, {"specimen", "specimens"}
                };

QMap<QString, QString> _assimilatedClassicalInflectionDictionary = QMap<QString, QString> {
                        {"alumna", "alumnae"}, {"alga", "algae"}, {"vertebra", "vertebrae"},
                        {"codex", "codices"},
                        {"murex", "murices"}, {"silex", "silices"}, {"aphelion", "aphelia"},
                        {"hyperbaton", "hyperbata"}, {"perihelion", "perihelia"},
                        {"asyndeton", "asyndeta"}, {"noumenon", "noumena"},
                        {"phenomenon", "phenomena"}, {"criterion", "criteria"}, {"organon", "organa"},
                        {"prolegomenon", "prolegomena"}, {"agendum", "agenda"}, {"datum", "data"},
                        {"extremum", "extrema"}, {"bacterium", "bacteria"}, {"desideratum", "desiderata"},
                        {"stratum", "strata"}, {"candelabrum", "candelabra"}, {"erratum", "errata"},
                        {"ovum", "ova"}, {"forum", "fora"}, {"addendum", "addenda"},  {"stadium", "stadia"},
                        {"automaton", "automata"}, {"polyhedron", "polyhedra"},
                    };

QMap<QString, QString> _classicalInflectionDictionary = QMap<QString, QString> {
                       {"stamen", "stamina"}, {"foramen", "foramina"}, {"lumen", "lumina"},
                       {"anathema", "anathemata"}, {"----", "----ta"}, {"oedema", "oedemata"},
                       {"bema", "bemata"}, {"enigma", "enigmata"}, {"sarcoma", "sarcomata"},
                       {"carcinoma", "carcinomata"}, {"gumma", "gummata"}, {"schema", "schemata"},
                       {"charisma", "charismata"}, {"lemma", "lemmata"}, {"soma", "somata"},
                       {"diploma", "diplomata"}, {"lymphoma", "lymphomata"}, {"stigma", "stigmata"},
                       {"dogma", "dogmata"}, {"magma", "magmata"}, {"stoma", "stomata"},
                       {"drama", "dramata"}, {"melisma", "melismata"}, {"trauma", "traumata"},
                       {"edema", "edemata"}, {"miasma", "miasmata"}, {"abscissa", "abscissae"},
                       {"formula", "formulae"}, {"medusa", "medusae"}, {"amoeba", "amoebae"},
                       {"hydra", "hydrae"}, {"nebula", "nebulae"}, {"antenna", "antennae"},
                       {"hyperbola", "hyperbolae"}, {"nova", "novae"}, {"aurora", "aurorae"},
                       {"lacuna", "lacunae"}, {"parabola", "parabolae"}, {"apex", "apices"},
                       {"latex", "latices"}, {"vertex", "vertices"}, {"cortex", "cortices"},
                       {"pontifex", "pontifices"}, {"vortex", "vortices"}, {"index", "indices"},
                       {"simplex", "simplices"}, {"iris", "irides"}, {"----oris", "----orides"},
                       {"alto", "alti"}, {"contralto", "contralti"}, {"soprano", "soprani"},
                       {"b----o", "b----i"}, {"crescendo", "crescendi"}, {"tempo", "tempi"},
                       {"canto", "canti"}, {"solo", "soli"}, {"aquarium", "aquaria"},
                       {"interregnum", "interregna"}, {"quantum", "quanta"},
                       {"compendium", "compendia"}, {"lustrum", "lustra"}, {"rostrum", "rostra"},
                       {"consortium", "consortia"}, {"maximum", "maxima"}, {"spectrum", "spectra"},
                       {"cranium", "crania"}, {"medium", "media"}, {"speculum", "specula"},
                       {"curriculum", "curricula"}, {"memorandum", "memoranda"}, {"stadium", "stadia"},
                       {"dictum", "dicta"}, {"millenium", "millenia"}, {"t----zium", "t----zia"},
                       {"emporium", "emporia"}, {"minimum", "minima"}, {"ultimatum", "ultimata"},
                       {"enconium", "enconia"}, {"momentum", "momenta"}, {"vacuum", "vacua"},
                       {"gymnasium", "gymnasia"}, {"optimum", "optima"}, {"velum", "vela"},
                       {"honorarium", "honoraria"}, {"phylum", "phyla"}, {"focus", "foci"},
                       {"nimbus", "nimbi"}, {"succubus", "succubi"}, {"fungus", "fungi"},
                       {"nucleolus", "nucleoli"}, {"torus", "tori"}, {"genius", "genii"},
                       {"radius", "radii"}, {"umbilicus", "umbilici"}, {"incubus", "incubi"},
                       {"stylus", "styli"}, {"uterus", "uteri"}, {"stimulus", "stimuli"}, {"apparatus", "apparatus"},
                       {"impetus", "impetus"}, {"prospectus", "prospectus"}, {"cantus", "cantus"},
                       {"nexus", "nexus"}, {"sinus", "sinus"}, {"coitus", "coitus"}, {"plexus", "plexus"},
                       {"status", "status"}, {"hiatus", "hiatus"}, {"afreet", "afreeti"},
                       {"afrit", "afriti"}, {"efreet", "efreeti"}, {"cherub", "cherubim"},
                       {"goy", "goyim"}, {"seraph", "seraphim"}, {"alumnus", "alumni"}
                   };


bool zPluralizer::IsAlphabets(QString word)
        {
            // return false when the word is "[\s]*" or leading or tailing with spaces
            // or contains non alphabetical characters
            QString wt = word.trimmed();

            if (wt.isEmpty() || word != wt || IsAlphabetsRegexp.match(word).hasMatch())
            {
                return false;
            }
            else
            {
                return true;
            }
        }

//QString ToLowerInvariant(QString word){

//    return word.toLower();
//}

bool zPluralizer::IsNoOpWord(QString word)
        {

            if (!IsAlphabets(word) || word.length() <= 1 || _pronounList.contains(word.toLower()))
            {
                return true;
            }
            else
            {
                return false;
            }
        }

QString zPluralizer::GetSuffixWord(QString word, QString *prefixWord)
        {
            // use the last space to separate the words
            int lastSpaceIndex = word.lastIndexOf(' ');
            *prefixWord = word.left(lastSpaceIndex + 1);
            return word.mid(lastSpaceIndex + 1);

            //
        }

bool zPluralizer::DoesWordContainSuffix(QString word, QStringList suffixes)
        {
        zforeach(s, suffixes){
            if(word.endsWith(*(s)))
                    return true;
                };
        return false;
        }


bool zPluralizer::IsUninflective(QString word)
        {
            //EDesignUtil.CheckArgumentNull<string>(word, "word");
            if (DoesWordContainSuffix(word, _uninflectiveSuffixList)
                           || (word.toLower() != word && word.endsWith("ese"))
                           || _uninflectiveWordList.contains(word.toLower()))
            {
                return true;
            }
            else
            {
                return false;
            }
        }

bool zPluralizer::IsPlural(QString word)
        {
            //EDesignUtil.CheckArgumentNull<string>(word, "word");

            if (_userDictionary.values().contains(word)){
                return true;
            }
            if (_userDictionary.contains(word))
            {
                return false;
            }

            if (IsUninflective(word) || _knownPluralWords.contains(word.toLower()))
            {
                return true;
            }
            else if (InternalSingularize(word) == word)
            {
                return false;
            }
            else
            {
                return true;
            }
        }
// https://referencesource.microsoft.com/#System.Data.Entity.Design/System/Data/Entity/Design/PluralizationService/PluralizationServiceUtil.cs,a56c9a76ca325fc3
bool zPluralizer::TryInflectOnSuffixInWord(QString word, QStringList suffixes,  int l, QString newWord, QString *outWord)
        {
            *outWord = "";

            if (DoesWordContainSuffix(word, suffixes))
            {
                *outWord =  word.left(word.length()-l) + newWord;

                return true;
            }
            else
            {
                return false;
            }
        }

//bool ExistsInFirst(QString value)
//        {
//            return ExistsInFirst(value.ToLowerInvariant());
//        }

/*
 * https://referencesource.microsoft.com/#System.Data.Entity.Design/System/Data/Entity/Design/PluralizationService/EnglishPluralizationService.cs,9f7905f058826204,references
*/
QString zPluralizer::InternalPluralize(QString word){

    // ha felvettük sajátba, akkor úgy konvertáljuk
    if (this->_userDictionary.contains(word)){
                return this->_userDictionary.value(word);
            }

    // ha tartalom alapján nem lehet átalakítani
    if (IsNoOpWord(word)){
                return word;
            }

    QString prefixWord;
    QString suffixWord = GetSuffixWord(word, &prefixWord);

    // ha  a suffix tartalom alapján nem lehet átalakítani
    // by me -> by me
    if (IsNoOpWord(suffixWord)){
        return prefixWord + suffixWord;
    }

    // handle the word that do not inflect in the plural form
    if (IsUninflective(suffixWord)){
        return prefixWord + suffixWord;
    }

    // if word is one of the known plural forms, then just return
    if (_knownPluralWords.contains(suffixWord.toLower()) || IsPlural(suffixWord)){
        return prefixWord + suffixWord;
    }

    // handle irregular plurals, e.g. "ox" -> "oxen"
    if (_irregularPluralsDictionary.contains(suffixWord))
    {
        return prefixWord + _irregularPluralsDictionary.value(suffixWord);
    }
    //

    QString newSuffixWord  ;
    if (TryInflectOnSuffixInWord(suffixWord, QStringList { "man" }, 2, "en", &newSuffixWord)){
        return prefixWord + newSuffixWord;
    }

    if (TryInflectOnSuffixInWord(suffixWord, QStringList { "louse", "mouse" }, 4, "ice", &newSuffixWord)){
        return prefixWord + newSuffixWord;
    }

    if (TryInflectOnSuffixInWord(suffixWord, QStringList { "tooth" }, 4, "eeth", &newSuffixWord)){
        return prefixWord + newSuffixWord;
    }
    if (TryInflectOnSuffixInWord(suffixWord, QStringList { "goose" }, 4, "eese", &newSuffixWord)){
        return prefixWord + newSuffixWord;
    }

    if (TryInflectOnSuffixInWord(suffixWord, QStringList { "foot" }, 3, "eet", &newSuffixWord)){
        return prefixWord + newSuffixWord;
    }

    if (TryInflectOnSuffixInWord(suffixWord, QStringList { "zoon" }, 3, "oa", &newSuffixWord)){
        return prefixWord + newSuffixWord;
    }

    if (TryInflectOnSuffixInWord(suffixWord, QStringList {  "cis", "sis", "xis" }, 2, "es", &newSuffixWord)){
        return prefixWord + newSuffixWord;
    }

    // handle assimilated classical inflections, e.g. vertebra -> vertebrae
    if (_assimilatedClassicalInflectionDictionary.contains(suffixWord)){
        return prefixWord + _assimilatedClassicalInflectionDictionary.values().contains(suffixWord);
    }

// Handle the classical variants of modern inflections
    //
    if (_classicalInflectionDictionary.contains(suffixWord)){
        return prefixWord + _classicalInflectionDictionary.values().contains(suffixWord);
    }

//    if (PluralizationServiceUtil.TryInflectOnSuffixInWord(suffixWord,
//        new List<string>() { "trix" },
//        (s) => s.Remove(s.Length - 1, 1) + "ces", this.Culture, out newSuffixWord))
//    {
//        return prefixWord + newSuffixWord;
//    }

//    if (PluralizationServiceUtil.TryInflectOnSuffixInWord(suffixWord,
//        new List<string>() { "eau", "ieu" },
//        (s) => s + "x", this.Culture, out newSuffixWord))
//    {
//        return prefixWord + newSuffixWord;
//    }

//    if (this._wordsEndingWithInxAnxYnxPluralizationService.ExistsInFirst(suffixWord))
//    {
//        return prefixWord + this._wordsEndingWithInxAnxYnxPluralizationService.GetSecondValue(suffixWord);
//    }

//    // [cs]h and ss that take es as plural form
//    if (PluralizationServiceUtil.TryInflectOnSuffixInWord(suffixWord, new List<string>() { "ch", "sh", "ss" }, (s) => s + "es", this.Culture, out newSuffixWord))
//    {
//        return prefixWord + newSuffixWord;
//    }

//    // f, fe that take ves as plural form
//    if (PluralizationServiceUtil.TryInflectOnSuffixInWord(suffixWord,
//        new List<string>() { "alf", "elf", "olf", "eaf", "arf" },
//        (s) => s.EndsWith("deaf", true, this.Culture) ? s : s.Remove(s.Length - 1, 1) + "ves", this.Culture, out newSuffixWord))
//    {
//        return prefixWord + newSuffixWord;
//    }

//    if (PluralizationServiceUtil.TryInflectOnSuffixInWord(suffixWord,
//        new List<string>() { "nife", "life", "wife" },
//        (s) => s.Remove(s.Length - 2, 2) + "ves", this.Culture, out newSuffixWord))
//    {
//        return prefixWord + newSuffixWord;
//    }

//    // y takes ys as plural form if preceded by a vowel, but ies if preceded by a consonant, e.g. stays, skies
//    if (PluralizationServiceUtil.TryInflectOnSuffixInWord(suffixWord,
//        new List<string>() { "ay", "ey", "iy", "oy", "uy" },
//        (s) => s + "s", this.Culture, out newSuffixWord))
//    {
//        return prefixWord + newSuffixWord;
//    }

//    //

//    if (suffixWord.EndsWith("y", true, this.Culture))
//    {
//        return prefixWord + suffixWord.Remove(suffixWord.Length - 1, 1) + "ies";
//    }

//    // handle some of the words o -> os, and [vowel]o -> os, and the rest are o->oes
//    if (this._oSuffixPluralizationService.ExistsInFirst(suffixWord))
//    {
//        return prefixWord + this._oSuffixPluralizationService.GetSecondValue(suffixWord);
//    }

//    if (PluralizationServiceUtil.TryInflectOnSuffixInWord(suffixWord,
//        new List<string>() { "ao", "eo", "io", "oo", "uo" },
//        (s) => s + "s", this.Culture, out newSuffixWord))
//    {
//        return prefixWord + newSuffixWord;
//    }

//    if (suffixWord.EndsWith("o", true, this.Culture) || suffixWord.EndsWith("s", true, this.Culture))
//    {
//        return prefixWord + suffixWord + "es";
//    }

//    if (suffixWord.EndsWith("x", true, this.Culture))
//    {
//        return prefixWord + suffixWord + "es";
//    }

    // cats, bags, hats, speakers
    return prefixWord + suffixWord + "s";
}


 QString zPluralizer::InternalSingularize(QString word){
     return "a";
 }

//               // words that we know of
//               if (this._userDictionary.ExistsInSecond(word))
//               {
//                   return this._userDictionary.GetFirstValue(word);
//               }

//               if (IsNoOpWord(word))
//               {
//                   return word;
//               }

//               string prefixWord;
//               string suffixWord = GetSuffixWord(word, out prefixWord);

//               if (IsNoOpWord(suffixWord))
//               {
//                   return prefixWord + suffixWord;
//               }

//               // handle the word that is the same as the plural form
//               if (this.IsUninflective(suffixWord))
//               {
//                   return prefixWord + suffixWord;
//               }

//               // if word is one of the known singular words, then just return

//               if (this._knownSingluarWords.Contains(suffixWord.ToLowerInvariant()))
//               {
//                   return prefixWord + suffixWord;
//               }

//               // handle simple irregular verbs, e.g. was -> were
//               if (this._irregularVerbPluralizationService.ExistsInSecond(suffixWord))
//               {
//                   return prefixWord + this._irregularVerbPluralizationService.GetFirstValue(suffixWord);
//               }

//               // handle irregular plurals, e.g. "ox" -> "oxen"
//               if (this._irregularPluralsPluralizationService.ExistsInSecond(suffixWord))
//               {
//                   return prefixWord + this._irregularPluralsPluralizationService.GetFirstValue(suffixWord);
//               }

//               // handle singluarization for words ending with sis and pluralized to ses,
//               // e.g. "ses" -> "sis"
//               if (this._wordsEndingWithSisPluralizationService.ExistsInSecond(suffixWord))
//               {
//                   return prefixWord + this._wordsEndingWithSisPluralizationService.GetFirstValue(suffixWord);
//               }

//               // handle words ending with se, e.g. "ses" -> "se"
//               if (this._wordsEndingWithSePluralizationService.ExistsInSecond(suffixWord))
//               {
//                   return prefixWord + this._wordsEndingWithSePluralizationService.GetFirstValue(suffixWord);
//               }

//               // handle words ending with sus, e.g. "suses" -> "sus"
//               if (this._wordsEndingWithSusPluralizationService.ExistsInSecond(suffixWord))
//               {
//                   return prefixWord + this._wordsEndingWithSusPluralizationService.GetFirstValue(suffixWord);
//               }

//               string newSuffixWord;
//               if (PluralizationServiceUtil.TryInflectOnSuffixInWord(suffixWord,
//                   new List<string>() { "men" },
//                   (s) => s.Remove(s.Length - 2, 2) + "an", this.Culture, out newSuffixWord))
//               {
//                   return prefixWord + newSuffixWord;
//               }

//               // handle irregular inflections for common suffixes, e.g. "mouse" -> "mice"
//               if (PluralizationServiceUtil.TryInflectOnSuffixInWord(suffixWord,
//                   new List<string>() { "lice", "mice" },
//                   (s) => s.Remove(s.Length - 3, 3) + "ouse", this.Culture, out newSuffixWord))
//               {
//                   return prefixWord + newSuffixWord;
//               }

//               if (PluralizationServiceUtil.TryInflectOnSuffixInWord(suffixWord,
//                   new List<string>() { "teeth" },
//                   (s) => s.Remove(s.Length - 4, 4) + "ooth", this.Culture, out newSuffixWord))
//               {
//                   return prefixWord + newSuffixWord;
//               }
//               if (PluralizationServiceUtil.TryInflectOnSuffixInWord(suffixWord,
//                   new List<string>() { "geese" },
//                   (s) => s.Remove(s.Length - 4, 4) + "oose", this.Culture, out newSuffixWord))
//               {
//                   return prefixWord + newSuffixWord;
//               }
//               if (PluralizationServiceUtil.TryInflectOnSuffixInWord(suffixWord,
//                   new List<string>() { "feet" },
//                   (s) => s.Remove(s.Length - 3, 3) + "oot", this.Culture, out newSuffixWord))
//               {
//                   return prefixWord + newSuffixWord;
//               }
//               if (PluralizationServiceUtil.TryInflectOnSuffixInWord(suffixWord,
//                   new List<string>() { "zoa" },
//                   (s) => s.Remove(s.Length - 2, 2) + "oon", this.Culture, out newSuffixWord))
//               {
//                   return prefixWord + newSuffixWord;
//               }

//               // [cs]h and ss that take es as plural form, this is being moved up since the sses will be override by the ses
//               if (PluralizationServiceUtil.TryInflectOnSuffixInWord(suffixWord,
//                   new List<string>() { "ches", "shes", "sses" },
//                   (s) => s.Remove(s.Length - 2, 2), this.Culture, out newSuffixWord))
//               {
//                   return prefixWord + newSuffixWord;
//               }


//               // handle assimilated classical inflections, e.g. vertebra -> vertebrae
//               if (this._assimilatedClassicalInflectionPluralizationService.ExistsInSecond(suffixWord))
//               {
//                   return prefixWord + this._assimilatedClassicalInflectionPluralizationService.GetFirstValue(suffixWord);
//               }

//               // Handle the classical variants of modern inflections
//               //
//               if (this._classicalInflectionPluralizationService.ExistsInSecond(suffixWord))
//               {
//                   return prefixWord + this._classicalInflectionPluralizationService.GetFirstValue(suffixWord);
//               }

//               if (PluralizationServiceUtil.TryInflectOnSuffixInWord(suffixWord,
//                   new List<string>() { "trices" },
//                   (s) => s.Remove(s.Length - 3, 3) + "x", this.Culture, out newSuffixWord))
//               {
//                   return prefixWord + newSuffixWord;
//               }

//               if (PluralizationServiceUtil.TryInflectOnSuffixInWord(suffixWord,
//                   new List<string>() { "eaux", "ieux" },
//                   (s) => s.Remove(s.Length - 1, 1), this.Culture, out newSuffixWord))
//               {
//                   return prefixWord + newSuffixWord;
//               }

//               if (this._wordsEndingWithInxAnxYnxPluralizationService.ExistsInSecond(suffixWord))
//               {
//                   return prefixWord + this._wordsEndingWithInxAnxYnxPluralizationService.GetFirstValue(suffixWord);
//               }

//               // f, fe that take ves as plural form
//               if (PluralizationServiceUtil.TryInflectOnSuffixInWord(suffixWord,
//                   new List<string>() { "alves", "elves", "olves", "eaves", "arves" },
//                   (s) => s.Remove(s.Length - 3, 3) + "f", this.Culture, out newSuffixWord))
//               {
//                   return prefixWord + newSuffixWord;
//               }

//               if (PluralizationServiceUtil.TryInflectOnSuffixInWord(suffixWord,
//                   new List<string>() { "nives", "lives", "wives" },
//                   (s) => s.Remove(s.Length - 3, 3) + "fe", this.Culture, out newSuffixWord))
//               {
//                   return prefixWord + newSuffixWord;
//               }

//               // y takes ys as plural form if preceded by a vowel, but ies if preceded by a consonant, e.g. stays, skies
//               if (PluralizationServiceUtil.TryInflectOnSuffixInWord(suffixWord,
//                   new List<string>() { "ays", "eys", "iys", "oys", "uys" },
//                   (s) => s.Remove(s.Length - 1, 1), this.Culture, out newSuffixWord))
//               {
//                   return prefixWord + newSuffixWord;
//               }

//               //

//               if (suffixWord.EndsWith("ies", true, this.Culture))
//               {
//                   return prefixWord + suffixWord.Remove(suffixWord.Length - 3, 3) + "y";
//               }

//               // handle some of the words o -> os, and [vowel]o -> os, and the rest are o->oes
//               if (this._oSuffixPluralizationService.ExistsInSecond(suffixWord))
//               {
//                   return prefixWord + this._oSuffixPluralizationService.GetFirstValue(suffixWord);
//               }

//               if (PluralizationServiceUtil.TryInflectOnSuffixInWord(suffixWord,
//                   new List<string>() { "aos", "eos", "ios", "oos", "uos" },
//                   (s) => suffixWord.Remove(suffixWord.Length - 1, 1), this.Culture, out newSuffixWord))
//               {
//                   return prefixWord + newSuffixWord;
//               }

//               //




//               if (PluralizationServiceUtil.TryInflectOnSuffixInWord(suffixWord,
//                   new List<string>() { "ces" },
//                   (s) => s.Remove(s.Length - 1, 1), this.Culture, out newSuffixWord))
//               {
//                   return prefixWord + newSuffixWord;
//               }

//               if (PluralizationServiceUtil.TryInflectOnSuffixInWord(suffixWord,
//                   new List<string>() { "ces", "ses", "xes" },
//                   (s) => s.Remove(s.Length - 2, 2), this.Culture, out newSuffixWord))
//               {
//                   return prefixWord + newSuffixWord;
//               }

//               if (suffixWord.EndsWith("oes", true, this.Culture))
//               {
//                   return prefixWord + suffixWord.Remove(suffixWord.Length - 2, 2);
//               }

//               if (suffixWord.EndsWith("ss", true, this.Culture))
//               {
//                   return prefixWord + suffixWord;
//               }

//               if (suffixWord.EndsWith("s", true, this.Culture))
//               {
//                   return prefixWord + suffixWord.Remove(suffixWord.Length - 1, 1);
//               }

//               // word is a singlar
//               return prefixWord + suffixWord;
//           }
//}
