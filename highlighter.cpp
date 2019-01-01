#include "highlighter.h"

Highlighter::Highlighter(QTextDocument *parent): QSyntaxHighlighter(parent)
{
    HighlightingRule rule;

    keywordFormat.setForeground(Qt::darkBlue);
    keywordFormat.setFontWeight(QFont::Bold);
    //QStringList keywordPatterns;
//    keywordPatterns << "\\bchar\\b" << "\\bclass\\b" << "\\bconst\\b"
//                    << "\\bdouble\\b" << "\\benum\\b" << "\\bexplicit\\b"
//                    << "\\bfriend\\b" << "\\binline\\b" << "\\bint\\b"
//                    << "\\blong\\b" << "\\bnamespace\\b" << "\\boperator\\b"
//                    << "\\bprivate\\b" << "\\bprotected\\b" << "\\bpublic\\b"
//                    << "\\bshort\\b" << "\\bsignals\\b" << "\\bsigned\\b"
//                    << "\\bslots\\b" << "\\bstatic\\b" << "\\bstruct\\b"
//                    << "\\btemplate\\b" << "\\btypedef\\b" << "\\btypename\\b"
//                    << "\\bunion\\b" << "\\bunsigned\\b" << "\\bvirtual\\b"
//                    << "\\bvoid\\b" << "\\bvolatile\\b" << "\\bbool\\b"
//                    << "\\bstring\\b" <<"\\bDateTime\\b";

//    foreach (const QString &pattern, keywordPatterns)
//    {
//        rule.pattern = QRegularExpression(pattern);
//        rule.format = keywordFormat;
//        highlightingRules.append(rule);
//    }

    classFormat.setFontWeight(QFont::Bold);
    //classFormat.setFontItalic(true);
    classFormat.setForeground(Qt::darkCyan);
    rule.pattern = QRegularExpression(QStringLiteral(R"(class\s*([\w\S]*))"));//(R"(class\s+\w+\s+(\{(?>[^{}]+|(?1))*\}))"));
    rule.format = classFormat;
    highlightingRules.append(rule);

    singleLineCommentFormat.setForeground(Qt::darkGreen);
    rule.pattern = QRegularExpression(QStringLiteral("//[^\n]*"));
    rule.format = singleLineCommentFormat;
    highlightingRules.append(rule);

    multiLineCommentFormat.setForeground(Qt::darkGreen);

//    quotationFormat.setForeground(Qt::darkRed);
//    rule.pattern = QRegularExpression(QStringLiteral("\".*\""));
//    rule.format = quotationFormat;
//    highlightingRules.append(rule);

//    functionFormat.setFontItalic(true);
//    functionFormat.setForeground(Qt::blue);
//    rule.pattern = QRegularExpression(QStringLiteral("\\b[A-Za-z0-9_]+(?=\\()"));
//    rule.format = functionFormat;
//    highlightingRules.append(rule);

    commentStartExpression = QRegularExpression(QStringLiteral("/\\*"));
    commentEndExpression = QRegularExpression(QStringLiteral("\\*/"));
}

void Highlighter::setKeywords(const QStringList &keywordPatterns)
{
    HighlightingRule rule;

    foreach (const QString &pattern, keywordPatterns)
    {
        rule.pattern = QRegularExpression("\\b"+pattern+"\\b");
        rule.format = keywordFormat;
        highlightingRules.append(rule);
    }
}

void Highlighter::highlightBlock(const QString &text)
{
    foreach (const HighlightingRule &rule, highlightingRules)
    {
        QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
        while (matchIterator.hasNext())
        {
            QRegularExpressionMatch match = matchIterator.next();
            if(match.lastCapturedIndex()>0)
            {
                setFormat(match.capturedStart(1), match.capturedLength(1), rule.format);
            }
            else
            {
                setFormat(match.capturedStart(), match.capturedLength(), rule.format);
            }
        }
    }
    setCurrentBlockState(0);

    int startIndex = 0;
    if (previousBlockState() != 1) startIndex = text.indexOf(commentStartExpression);

    while (startIndex >= 0)
    {
        QRegularExpressionMatch match = commentEndExpression.match(text, startIndex);
        int endIndex = match.capturedStart();
        int commentLength = 0;
        if (endIndex == -1)
        {
            setCurrentBlockState(1);
            commentLength = text.length() - startIndex;
        }
        else
        {
            commentLength = endIndex - startIndex
                + match.capturedLength();
        }
        setFormat(startIndex, commentLength, multiLineCommentFormat);
        startIndex = text.indexOf(commentStartExpression, startIndex + commentLength);
    }
}
