#include <cstddef>
#include <fstream>
#include <ios>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "UtScanDir.hpp"
#include "UtStringRef.hpp"
#include "UtUnitTypes.hpp"
#include "WsfPProxyDeserialize.hpp"
#include "WsfPProxyDiff.hpp"
#include "WsfPProxyRegistry.hpp"
#include "WsfPProxySerialize.hpp"
#include "WsfParseAlternate.hpp"
#include "WsfParseRecurrence.hpp"
#include "WsfParseRule.hpp"
#include "WsfParseSourceProvider.hpp"
#include "WsfParseType.hpp"
#include "WsfParseTypePath.hpp"
#include "WsfParser.hpp"
#include "mainwindow.h"
#include "qapplication.h"
#include "qchar.h"
#include "qdebug.h"
#include "qjsonarray.h"
#include "qjsondocument.h"
#include "qjsonobject.h"
#include "qjsonvalue.h"
#include "util/source/UtTextDocument.hpp"
#include "wsf_parser/source/WsfParseNode.hpp"
#include "wsf_parser/source/WsfParseRule.hpp"

namespace {
class ParseSourceProvider : public WsfParseSourceProvider {
    std::map<UtPath, UtTextDocument *> mCache;
    //! Derived classes should override this method to return
    //! a source if it exists at aPath
    virtual UtTextDocument *FindSource(const UtPath &aPath, bool aReadAccess) {
        if (mCache.find(aPath) != mCache.end()) {
            return mCache[aPath];
        }

        if (aPath.Stat() == UtPath::cFILE) {
            return mCache[aPath] = new UtTextDocument(aPath.GetSystemPath());
        }
        return 0;
    }
};

int GetLineNumber(UtTextDocumentRange &aRange) {
    int line = 1;
    for (int i = 0; i < aRange.GetBegin(); ++i) {
        if ((*aRange.mSource)[i] == '\n') {
            ++line;
        }
    }
    return line;
}

} // namespace

;

QJsonValue GetPrompt(WsfParseRule *aRule, std::set<WsfParseRule *> &stack) {
    if (!aRule) {
        return {};
    }

    QJsonValue res;
    stack.insert(aRule);

    switch (aRule->Type()) {
    case WsfParseRule::cLITERAL: {
        res = QString::fromStdString(
            static_cast<WsfParseLiteral *>((aRule))->mText);
        break;
    }
    case WsfParseRule::cINT: {
        res = "[INT]";
        break;
    }
    case WsfParseRule::cREAL: {
        res = "[REAL]";
        break;
    }
    case WsfParseRule::cSTRING: {
        res = "[String]";
        break;
    }
    case WsfParseRule::cSEQUENCE: {
        QJsonArray array;
        auto seq = aRule->GetSequence();
        for (const auto &rule : seq) {
            if (stack.count(rule)) {
                continue;
            }
            array.append(GetPrompt(rule, stack));
            stack.erase(rule);
        }
        res = array;
        break;
    }
    case WsfParseRule::cVALUE: {
        WsfParseValue *value = (WsfParseValue *)aRule;
        res = "[" + QString::fromStdString(value->GetFullName()) + "]";
        break;
    }
    case WsfParseRule::cSTRUCT:
    case WsfParseRule::cNAMED_RULE:
    case WsfParseRule::cRULE_REFERENCE: {
        if (stack.count(aRule->GetSubordinateRule())) {
            return "Stack Over Flow";
        }
        res = GetPrompt(aRule->GetSubordinateRule(), stack);
        stack.erase(aRule->GetSubordinateRule());
        break;
    }
    case WsfParseRule::cRECURRENCE: {
        QJsonArray array;
        WsfParseRecurrence *rec = static_cast<WsfParseRecurrence *>(aRule);
        if (stack.count(rec->GetSubordinateRule())) {
            return {};
        }
        array.append(GetPrompt(rec->GetSubordinateRule(), stack));
        array.append(QString::fromStdString("*"));
        stack.erase(aRule->GetSubordinateRule());
        res = array;
        break;
    }
    case WsfParseRule::cALTERNATE: {
        QJsonObject json;
        WsfParseAlternate *alternate = static_cast<WsfParseAlternate *>(aRule);
        const std::vector<WsfParseRule *> &rules = alternate->Alternates();
        for (int i = 0; i < rules.size(); ++i) {
            if (stack.count(rules[i])) {
                continue;
            }
            json.insert("Altenate " + QString::number(i),
                        GetPrompt(rules[i], stack));
            stack.erase(rules[i]);
        }
        res = json;
        break;
    }
        // case WsfParseRule::cTYPE_COMMAND: {
        //     res += "[ TypeCommand ]";
        //     break;
        // }
        // break;
    }

    return res;
}

QJsonValue GetPromptExpand(WsfParseRule *aRule,
                           std::set<WsfParseRule *> &stack) {
    if (!aRule) {
        return {};
    }

    QJsonValue res;
    stack.insert(aRule);

    switch (aRule->Type()) {
    case WsfParseRule::cLITERAL: {
        res = QString::fromStdString(
            static_cast<WsfParseLiteral *>((aRule))->mText);
        break;
    }
    case WsfParseRule::cINT: {
        res = "[INT]";
        break;
    }
    case WsfParseRule::cREAL: {
        res = "[REAL]";
        break;
    }
    case WsfParseRule::cSTRING: {
        res = "[String]";
        break;
    }
    case WsfParseRule::cQUOTED_STRING: {
        res = "[QuoteString]";
        break;
    }
    case WsfParseRule::cLINE_STRING: {
        res = "[LineString]";
        break;
    }
    case WsfParseRule::cSEQUENCE: {
        QJsonArray array;
        auto seq = aRule->GetSequence();
        for (const auto &rule : seq) {
            if (stack.count(rule)) {
                continue;
            }
            QJsonValue res = GetPromptExpand(rule, stack);
            if (seq.size() == 1 && res.isObject()) {
                return res;
            } else {
                array.append(res);
            }
            stack.erase(rule);
        }
        res = array;
        break;
    }
    case WsfParseRule::cVALUE: {
        WsfParseValue *value = (WsfParseValue *)aRule;
        res = "[" + QString::fromStdString(value->GetFullName()) + "]";
        break;
    }
    case WsfParseRule::cSTRUCT:
    case WsfParseRule::cNAMED_RULE:
    case WsfParseRule::cRULE_REFERENCE: {
        if (stack.count(aRule->GetSubordinateRule())) {
            return "Stack Over Flow";
        }
        res = GetPromptExpand(aRule->GetSubordinateRule(), stack);
        stack.erase(aRule->GetSubordinateRule());
        break;
    }
    case WsfParseRule::cRECURRENCE: {
        QJsonArray array;
        WsfParseRecurrence *rec = static_cast<WsfParseRecurrence *>(aRule);
        if (stack.count(rec->GetSubordinateRule())) {
            return {};
        }
        array.append(GetPromptExpand(rec->GetSubordinateRule(), stack));
        array.append(QString::fromStdString("*"));
        stack.erase(aRule->GetSubordinateRule());
        res = array;
        break;
    }
    case WsfParseRule::cALTERNATE: {
        QJsonObject json;
        WsfParseAlternate *alternate = static_cast<WsfParseAlternate *>(aRule);
        const std::vector<WsfParseRule *> &rules = alternate->Alternates();
        for (int i = 0; i < rules.size(); ++i) {
            if (stack.count(rules[i])) {
                continue;
            }
            QJsonValue nestedValue = GetPromptExpand(rules[i], stack);
            if (nestedValue.isObject()) {
                QJsonObject obj = nestedValue.toObject();
                auto keys = obj.keys();
                for (const auto &key : keys) {
                    json.insert("Altenate " + QString::number(json.size()),
                                obj[key]);
                }
            } else {
                json.insert("Altenate " + QString::number(i),
                            GetPromptExpand(rules[i], stack));
            }
            stack.erase(rules[i]);
        }
        res = json;
        break;
    }
        // case WsfParseRule::cTYPE_COMMAND: {
        //     res += "[ TypeCommand ]";
        //     break;
        // }
        // break;
    }

    return res;
}

struct ParserWarpper {
    ParseSourceProvider *mSourceProviderPtr;
    WsfParser *mParserPtr;
    WsfPProxyRegistry *mProxyRegistry;
    bool mPrintOutput;
    bool mDumpProxy;
    std::ofstream mOutputStream;

    ParserWarpper()
        : mSourceProviderPtr(nullptr), mParserPtr(nullptr),
          mProxyRegistry(nullptr), mPrintOutput(false), mDumpProxy(false),
          mOutputStream() {}

    bool InitParser(const std::string &aGrammarFilesLocation,
                    const std::string &aWorkingDirectory) {
        mSourceProviderPtr = new ParseSourceProvider;
        WsfParseDefinitions *defs = new WsfParseDefinitions;
        UtScanDir::DirEntries entries;
        int found = UtScanDir::ScanDir(aGrammarFilesLocation, entries, "*.ag");
        for (size_t i = 0; i < entries.size(); ++i) {
            if (!entries[i].mDirectory) {
                UtPath path(aGrammarFilesLocation);
                if (entries[i].mFilename != "script_data.ag") {
                    path += entries[i].mFilename;
                    std::ifstream infile(path.GetSystemPath().c_str());
                    defs->AddGrammar(entries[i].mFilename, infile);
                }
            }
        }

        const std::list<std::string> &errors = defs->GetDefinitionErrors();
        bool ok = errors.empty();
        if (ok) {
            ok = defs->Initialize();
        }
        ok = ok && errors.empty();
        if (!ok) {
            std::cout
                << "***** ERROR: Encountered problems parsing WSF grammar:\n";
            for (std::list<std::string>::const_iterator i = errors.begin();
                 i != errors.end(); ++i) {
                std::cout << "\t" << *i << '\n';
            }
            return false;
        }

        mParserPtr = new WsfParser(mSourceProviderPtr, aWorkingDirectory);
        mParserPtr->SetDefinitions(defs);

        mProxyRegistry = new WsfPProxyRegistry;
        mProxyRegistry->Setup(defs->GetRootStruct(), defs->mBasicTypes);
        return true;
    }

    WsfParseNode *Parse(UtPath aFile) {
        if (aFile.IsNull()) {
            return nullptr;
        }

        // WsfPProxyValue desiredProxy;
        // {
        WsfPProxyIndex *proxyIndex = new WsfPProxyIndex;
        WsfPProxyDeserialize des(mProxyRegistry, proxyIndex);
        mParserPtr->Reset();
        std::vector<UtPath> fileList;
        fileList.push_back(aFile);
        WsfParseNode *treePtr = mParserPtr->ParseFiles(fileList, 0);
        // std::cout << "\n\n --------- DESIRED Parser --------- \n\n";
        // std::cout << WsfParseNode::GetNodeString(treePtr) << "\n";

        //   desiredProxy =
        //       WsfPProxyValue::ConstructNew(mProxyRegistry->GetType("root"));
        // desiredProxy = mProxyRegistry->GetType("root")->ConstructNew();
        // WsfPProxyUndoValue desiredUndo(desiredProxy);
        // desiredUndo.DisableUndo();
        // des.Deserialize(desiredUndo, treePtr);

        //   std::cout << "\n\n --------- DESIRED PROXY --------- \n\n";
        //   mProxyRegistry->OutputJSON(desiredProxy, desiredProxy, std::cout,
        //   0);
        //   // WsfPProxyRegistry::cHIDE_INHERITED_VALUES);
        // }

        return treePtr;
    }
};

// template<typename CONTAINER, typename COMPARE_FN>
// void UtMinimumCommonSubsequence(const CONTAINER& aLHS, const CONTAINER& aRHS,
// const COMPARE_FN& aCompare)
// {
//    UtMinimumCommonSubsequence_Detail<CONTAINER, COMPARE_FN> detail(aLHS,
//    aRHS, aCompare); detail.sub(0,0); detail.backtrack(); int stop=5;
// }

// match head
// match tail

int main(int argc, char *argv[]) {
    UtPath grammerRoot("grammer"), workingRoot("script");
    ParserWarpper parseWarpper;
    bool ok = parseWarpper.InitParser(grammerRoot.GetSystemPath(),
                                      workingRoot.GetSystemPath());
    if (!ok) {
        return 1;
    }

    std::string fileName = "helloworld.txt";

    WsfParseNode *res = nullptr;
    if (!fileName.empty()) {
        res = parseWarpper.Parse(fileName);
    }

    if (!res) {
        return 1;
    }

    // processor type
    WsfParseTypePath path;
    path.push_back(UtStringRef{"moverType"});
    path.push_back(UtStringRef{"WSF_AIR_MOVER"});
    auto aType =
        parseWarpper.mParserPtr->GetDefinitions()->mBasicTypes->FindType(path);
    auto rule = aType->GetRule();

    std::set<WsfParseRule *> stack;
    auto value = GetPromptExpand(rule, stack);

    if (value.isObject() || value.isArray()) {
        // 将 QJsonValue 包装成 QJsonDocument
        QJsonDocument doc;
        if (value.isObject()) {
            doc = QJsonDocument(value.toObject());
        } else {
            doc = QJsonDocument(value.toArray());
        }

        std::ofstream ofs{"output.json"};
        if (ofs.is_open()) {
            ofs << doc.toJson(QJsonDocument::Indented).toStdString();
        }

        // 使用 Indented 格式输出，并去掉 qDebug 自动添加的引号
        qDebug().noquote() << doc.toJson(QJsonDocument::Indented);
    }
    // 如果 value 是基本类型 (字符串、数字等)
    else {
        qDebug() << value;
    }

    // weapon type

    // sensor type

    QApplication app(argc, argv);

    inv::MainWindow win;
    win.SetASTTree(res);
    win.show();

    return app.exec();
}
