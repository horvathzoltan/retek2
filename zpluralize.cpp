#include "zpluralize.h"

#include <QRegularExpression>
#include "globals.h"

zPluralizer::zPluralizer()
{    
//    zPluralizer::_userDictionary = QMap<QString, QString>();
}

QMap<QString, QString> zPluralizer::_userDictionary = QMap<QString, QString>();

QRegularExpression zPluralizer::IsAlphabetsRegexp("[^a-zA-Z\\s]");

// névmások
QStringList zPluralizer::_pronounList = QStringList {
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
QStringList zPluralizer::_uninflectiveWordList = QStringList {
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

QStringList zPluralizer::_uninflectiveSuffixList =QStringList {
        "fish", "ois", "sheep", "deer", "pos", "itis", "ism" };

QStringList zPluralizer::_knownPluralWords = QStringList {};
QStringList zPluralizer::_knownSingluarWords = QStringList {};

QMap<QString, QString> zPluralizer::_irregularPluralsDictionary = QMap<QString, QString> {
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

QMap<QString, QString> zPluralizer::_assimilatedClassicalInflectionDictionary = QMap<QString, QString> {
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

QMap<QString, QString> zPluralizer::_classicalInflectionDictionary = QMap<QString, QString> {
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


QMap<QString, QString> zPluralizer::_wordsEndingWithInxAnxYnxPluralizationDictionary = QMap<QString, QString>{
    {"sphinx", "sphinxes"},
    {"larynx", "larynges"}, {"lynx", "lynxes"}, {"pharynx", "pharynxes"},
    {"phalanx", "phalanxes"}
};

QMap<QString, QString> zPluralizer::_oSuffixPluralizationDictionary = QMap<QString, QString>{
    {"albino", "albinos"}, {"generalissimo", "generalissimos"},
    {"manifesto", "manifestos"}, {"archipelago", "archipelagos"},
    {"ghetto", "ghettos"}, {"medico", "medicos"}, {"armadillo", "armadillos"},
    {"guano", "guanos"}, {"octavo", "octavos"}, {"commando", "commandos"},
    {"inferno", "infernos"}, {"photo", "photos"}, {"ditto", "dittos"},
    {"jumbo", "jumbos"}, {"pro", "pros"}, {"dynamo", "dynamos"},
    {"lingo", "lingos"}, {"quarto", "quartos"}, {"embryo", "embryos"},
    {"lumbago", "lumbagos"}, {"rhino", "rhinos"}, {"fiasco", "fiascos"},
    {"magneto", "magnetos"}, {"stylo", "stylos"}
};

QMap<QString, QString> zPluralizer::_irregularVerbList = QMap<QString, QString>{
    {"am", "are"}, {"are", "are"}, {"is", "are"}, {"was", "were"}, {"were", "were"},
    {"has", "have"}, {"have", "have"}
};

QMap<QString, QString> zPluralizer::_wordsEndingWithSisDictionary = QMap<QString, QString> {
    {"analysis", "analyses"}, {"crisis", "crises"}, {"basis", "bases"},
    {"atherosclerosis", "atheroscleroses"}, {"electrophoresis", "electrophoreses"},
    {"psychoanalysis", "psychoanalyses"}, {"photosynthesis", "photosyntheses"},
    {"amniocentesis", "amniocenteses"}, {"metamorphosis", "metamorphoses"},
    {"toxoplasmosis", "toxoplasmoses"}, {"endometriosis", "endometrioses"},
    {"tuberculosis", "tuberculoses"}, {"pathogenesis", "pathogeneses"},
    {"osteoporosis", "osteoporoses"}, {"parenthesis", "parentheses"},
    {"anastomosis", "anastomoses"}, {"peristalsis", "peristalses"},
    {"hypothesis", "hypotheses"}, {"antithesis", "antitheses"},
    {"apotheosis", "apotheoses"}, {"thrombosis", "thromboses"},
    {"diagnosis", "diagnoses"}, {"synthesis", "syntheses"},
    {"paralysis", "paralyses"}, {"prognosis", "prognoses"},
    {"cirrhosis", "cirrhoses"}, {"sclerosis", "scleroses"},
    {"psychosis", "psychoses"}, {"apoptosis", "apoptoses"}, {"symbiosis", "symbioses"}
};

QMap<QString, QString> zPluralizer::_wordsEndingWithSeDictionary = QMap<QString, QString> {
    {"house", "houses"}, {"case", "cases"}, {"enterprise", "enterprises"},
    {"purchase", "purchases"}, {"surprise", "surprises"}, {"release", "releases"},
    {"disease", "diseases"}, {"promise", "promises"}, {"refuse", "refuses"},
    {"whose", "whoses"}, {"phase", "phases"}, {"noise", "noises"},
    {"nurse", "nurses"}, {"rose", "roses"}, {"franchise", "franchises"},
    {"supervise", "supervises"}, {"farmhouse", "farmhouses"},
    {"suitcase", "suitcases"}, {"recourse", "recourses"}, {"impulse", "impulses"},
    {"license", "licenses"}, {"diocese", "dioceses"}, {"excise", "excises"},
    {"demise", "demises"}, {"blouse", "blouses"},
    {"bruise", "bruises"}, {"misuse", "misuses"}, {"curse", "curses"},
    {"prose", "proses"}, {"purse", "purses"}, {"goose", "gooses"},
    {"tease", "teases"}, {"poise", "poises"}, {"vase", "vases"},
    {"fuse", "fuses"}, {"muse", "muses"},
    {"slaughterhouse", "slaughterhouses"}, {"clearinghouse", "clearinghouses"},
    {"endonuclease", "endonucleases"}, {"steeplechase", "steeplechases"},
    {"metamorphose", "metamorphoses"}, {"----", "----s"},
    {"commonsense", "commonsenses"}, {"intersperse", "intersperses"},
    {"merchandise", "merchandises"}, {"phosphatase", "phosphatases"},
    {"summerhouse", "summerhouses"}, {"watercourse", "watercourses"},
    {"catchphrase", "catchphrases"}, {"compromise", "compromises"},
    {"greenhouse", "greenhouses"}, {"lighthouse", "lighthouses"},
    {"paraphrase", "paraphrases"}, {"mayonnaise", "mayonnaises"},
    {"----course", "----courses"}, {"apocalypse", "apocalypses"},
    {"courthouse", "courthouses"}, {"powerhouse", "powerhouses"},
    {"storehouse", "storehouses"}, {"glasshouse", "glasshouses"},
    {"hypotenuse", "hypotenuses"}, {"peroxidase", "peroxidases"},
    {"pillowcase", "pillowcases"}, {"roundhouse", "roundhouses"},
    {"streetwise", "streetwises"}, {"expertise", "expertises"},
    {"discourse", "discourses"}, {"warehouse", "warehouses"},
    {"staircase", "staircases"}, {"workhouse", "workhouses"},
    {"briefcase", "briefcases"}, {"clubhouse", "clubhouses"},
    {"clockwise", "clockwises"}, {"concourse", "concourses"},
    {"playhouse", "playhouses"}, {"turquoise", "turquoises"},
    {"boathouse", "boathouses"}, {"cellulose", "celluloses"},
    {"epitomise", "epitomises"}, {"gatehouse", "gatehouses"},
    {"grandiose", "grandioses"}, {"menopause", "menopauses"},
    {"penthouse", "penthouses"}, {"----horse", "----horses"},
    {"transpose", "transposes"}, {"almshouse", "almshouses"},
    {"customise", "customises"}, {"footloose", "footlooses"},
    {"galvanise", "galvanises"}, {"princesse", "princesses"},
    {"universe", "universes"}, {"workhorse", "workhorses"}
};


QMap<QString, QString> zPluralizer::_wordsEndingWithSusDictionary = QMap<QString, QString> {
    {"consensus","consensuses"}, {"census", "censuses"}
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
            else if (Singularize(word) == word)
            {
                return false;
            }
            else
            {
                return true;
            }
        }

bool zPluralizer::IsSingular(QString word)
        {
            //EDesignUtil.CheckArgumentNull<string>(word, "word");

            if (_userDictionary.contains(word))
            {
                return true;
            }
            if (_userDictionary.values().contains(word))
            {
                return false;
            }

            if (IsUninflective(word) || _knownSingluarWords.contains(word.toLower()))
            {
                return true;
            }
            else if (!IsNoOpWord(word) && Singularize(word) == word)
            {
                return true;
            }
            else
            {
                return false;
            }
        }

// https://referencesource.microsoft.com/#System.Data.Entity.Design/System/Data/Entity/Design/PluralizationService/PluralizationServiceUtil.cs,a56c9a76ca325fc3
bool zPluralizer::TryInflectOnSuffixInWord(QString word, QStringList suffixes,  int l, QString newWord, QString *outWord)
        {
            *outWord = "";

            if (DoesWordContainSuffix(word, suffixes))
            {
                if(l>0)
                    *outWord =  word.left(word.length()-l) + newWord;
                else
                    *outWord = word+ newWord;

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
QString zPluralizer::Pluralize(QString word){

    // ha felvettük sajátba, akkor úgy konvertáljuk
    if (_userDictionary.contains(word)){
                return _userDictionary.value(word);
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

    if (TryInflectOnSuffixInWord(suffixWord, QStringList { "trix" }, 1, "ces", &newSuffixWord)){
        return prefixWord + newSuffixWord;
    }

    if (TryInflectOnSuffixInWord(suffixWord, QStringList { "eau", "ieu" }, 0, "x", &newSuffixWord)){
        return prefixWord + newSuffixWord;
    }


    if (_wordsEndingWithInxAnxYnxPluralizationDictionary.contains(suffixWord)){
        return prefixWord + _wordsEndingWithInxAnxYnxPluralizationDictionary.values().contains(suffixWord);
    }

    // [cs]h and ss that take es as plural form
    if (TryInflectOnSuffixInWord(suffixWord, QStringList { "ch", "sh", "ss" }, 0, "es", &newSuffixWord)){
        return prefixWord + newSuffixWord;
    }

    if (TryInflectOnSuffixInWord(suffixWord, QStringList { "deaf" }, 0, "", &newSuffixWord)){
        return prefixWord + newSuffixWord;
    }

    // f, fe that take ves as plural form
    if (TryInflectOnSuffixInWord(suffixWord, QStringList { "alf", "elf", "olf", "eaf", "arf" }, 1 , "ves", &newSuffixWord)){
        return prefixWord + newSuffixWord;
    }

    if (TryInflectOnSuffixInWord(suffixWord,QStringList { "nife", "life", "wife" },2, "ves", &newSuffixWord)){
        return prefixWord + newSuffixWord;
    }

    // y takes ys as plural form if preceded by a vowel, but ies if preceded by a consonant, e.g. stays, skies
    if (TryInflectOnSuffixInWord(suffixWord,QStringList { "ay", "ey", "iy", "oy", "uy" },0, "s", &newSuffixWord)){
        return prefixWord + newSuffixWord;
    }

    //

    if (suffixWord.endsWith("y")){
        return prefixWord + suffixWord.remove(suffixWord.length() - 1, 1) + "ies";
    }

    // handle some of the words o -> os, and [vowel]o -> os, and the rest are o->oes
    if (_oSuffixPluralizationDictionary.contains(suffixWord)){
        return prefixWord + _oSuffixPluralizationDictionary.values().contains(suffixWord);
    }

    if (TryInflectOnSuffixInWord(suffixWord, QStringList { "ao", "eo", "io", "oo", "uo" },0,  "s", &newSuffixWord)){
        return prefixWord + newSuffixWord;
    }

    if (suffixWord.endsWith("o") || suffixWord.endsWith("s"))
    {
        return prefixWord + suffixWord + "es";
    }

    if (suffixWord.endsWith("x"))
    {
        return prefixWord + suffixWord + "es";
    }

    // cats, bags, hats, speakers
    return prefixWord + suffixWord + "s";
}


QString zPluralizer::Singularize(QString word){

    // words that we know of
    if (_userDictionary.values().contains(word)){
        return _userDictionary.key(word);
    }

    if (IsNoOpWord(word)){
        return word;
    }

    QString prefixWord;
    QString suffixWord = GetSuffixWord(word, &prefixWord);

    if (IsNoOpWord(suffixWord)){
        return prefixWord + suffixWord;
    }

    // handle the word that is the same as the plural form
    if (IsUninflective(suffixWord)){
        return prefixWord + suffixWord;
    }

    // if word is one of the known singular words, then just return
    if (_knownSingluarWords.contains(suffixWord.toLower()))
    {
        return prefixWord + suffixWord;
    }

    // handle simple irregular verbs, e.g. was -> were
    if (_irregularVerbList.values().contains(suffixWord)){
        return prefixWord + _irregularVerbList.key(suffixWord);
    }

    // handle irregular plurals, e.g. "ox" -> "oxen"
    if (_irregularPluralsDictionary.values().contains(suffixWord)){
        return prefixWord + _irregularPluralsDictionary.key(suffixWord);
    }

    // handle singluarization for words ending with sis and pluralized to ses,
    // e.g. "ses" -> "sis"
    if (_wordsEndingWithSisDictionary.values().contains(suffixWord)){
        return prefixWord + _wordsEndingWithSisDictionary.key(suffixWord);
    }

    // handle words ending with se, e.g. "ses" -> "se"
    if (_wordsEndingWithSeDictionary.values().contains(suffixWord)){
        return prefixWord + _wordsEndingWithSeDictionary.key(suffixWord);
    }

    // handle words ending with sus, e.g. "suses" -> "sus"
    if (_wordsEndingWithSusDictionary.values().contains(suffixWord)){
        return prefixWord + _wordsEndingWithSusDictionary.key(suffixWord);
    }

    QString newSuffixWord;
    if (TryInflectOnSuffixInWord(suffixWord, QStringList { "men" },2, "an", &newSuffixWord)){
        return prefixWord + newSuffixWord;
    }

    // handle irregular inflections for common suffixes, e.g. "mouse" -> "mice"
    if (TryInflectOnSuffixInWord(suffixWord,QStringList { "lice", "mice" },3, "ouse", &newSuffixWord)){
        return prefixWord + newSuffixWord;
    }

    if (TryInflectOnSuffixInWord(suffixWord, QStringList { "teeth" },4,  "ooth", &newSuffixWord)){
        return prefixWord + newSuffixWord;
    }

    if (TryInflectOnSuffixInWord(suffixWord, QStringList { "geese" },4, "oose", &newSuffixWord)){
        return prefixWord + newSuffixWord;
    }

    if (TryInflectOnSuffixInWord(suffixWord, QStringList { "feet" },3,  "oot", &newSuffixWord)){
        return prefixWord + newSuffixWord;
    }
    if (TryInflectOnSuffixInWord(suffixWord,QStringList { "zoa" },2,  "oon", &newSuffixWord)){
        return prefixWord + newSuffixWord;
    }

    // [cs]h and ss that take es as plural form, this is being moved up since the sses will be override by the ses
    if (TryInflectOnSuffixInWord(suffixWord,QStringList { "ches", "shes", "sses" },2, "", &newSuffixWord)){
        return prefixWord + newSuffixWord;
    }


    // handle assimilated classical inflections, e.g. vertebra -> vertebrae
    if (_assimilatedClassicalInflectionDictionary.values().contains(suffixWord)){
        return prefixWord + _assimilatedClassicalInflectionDictionary.key(suffixWord);
    }

    // Handle the classical variants of modern inflections
    //
    if (_classicalInflectionDictionary.values().contains(suffixWord)){
        return prefixWord + _classicalInflectionDictionary.key(suffixWord);
        }

    if (TryInflectOnSuffixInWord(suffixWord, QStringList { "trices" },3,  "x", &newSuffixWord)){
        return prefixWord + newSuffixWord;
    }

    if (TryInflectOnSuffixInWord(suffixWord, QStringList { "eaux", "ieux" },1, "", &newSuffixWord)){
        return prefixWord + newSuffixWord;
    }

    if (_wordsEndingWithInxAnxYnxPluralizationDictionary.values().contains(suffixWord)){
        return prefixWord + _wordsEndingWithInxAnxYnxPluralizationDictionary.key(suffixWord);
    }

    // f, fe that take ves as plural form
    if (TryInflectOnSuffixInWord(suffixWord, QStringList { "alves", "elves", "olves", "eaves", "arves" },3, "f", &newSuffixWord)){
        return prefixWord + newSuffixWord;
    }

    if (TryInflectOnSuffixInWord(suffixWord, QStringList { "nives", "lives", "wives" },3, "fe", &newSuffixWord)){
        return prefixWord + newSuffixWord;
    }

    // y takes ys as plural form if preceded by a vowel, but ies if preceded by a consonant, e.g. stays, skies
    if (TryInflectOnSuffixInWord(suffixWord, QStringList { "ays", "eys", "iys", "oys", "uys" },1, "", &newSuffixWord)){
        return prefixWord + newSuffixWord;
    }

    //

    if (suffixWord.endsWith("ies")){
        return prefixWord + suffixWord.remove(suffixWord.length() - 3, 3) + "y";
    }

    // handle some of the words o -> os, and [vowel]o -> os, and the rest are o->oes
    if (_oSuffixPluralizationDictionary.values().contains(suffixWord)){
        return prefixWord + _oSuffixPluralizationDictionary.key(suffixWord);
    }

    if (TryInflectOnSuffixInWord(suffixWord, QStringList { "aos", "eos", "ios", "oos", "uos" },1, "", &newSuffixWord)){
        return prefixWord + newSuffixWord;
    }

    //

    if (TryInflectOnSuffixInWord(suffixWord, QStringList { "ces" },1, "", &newSuffixWord)){
        return prefixWord + newSuffixWord;
    }

    if (TryInflectOnSuffixInWord(suffixWord, QStringList { "ces", "ses", "xes" },2, "", &newSuffixWord)){
        return prefixWord + newSuffixWord;
    }

    if (suffixWord.endsWith("oes")){
        return prefixWord + suffixWord.remove(suffixWord.length() - 2, 2);
    }

    if (suffixWord.endsWith("ss")){
        return prefixWord + suffixWord;
    }

    if (suffixWord.endsWith("s")){
        return prefixWord + suffixWord.remove(suffixWord.length() - 1, 1);
    }

    // word is a singlar
    return prefixWord + suffixWord;
}

