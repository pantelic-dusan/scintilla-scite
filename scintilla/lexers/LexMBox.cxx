// Scintilla source code edit control
/** @file LexMBox.cxx
 ** Lexer for MBox.
 **/
// Copyright 2018 - by Dusan Pantelic <dusan.pantelic96@gmail.com> and Randy Kramer <rhkramer@gmail.com>
// The License.txt file describes the conditions under which this software may be distributed.

#include <stdlib.h>
#include <string.h>
#include <regex>
#include <stdio.h>
#include <stdarg.h>
#include <assert.h>
#include <ctype.h>

#include "ILexer.h"
#include "Scintilla.h"
#include "SciLexer.h"

#include "WordList.h"
#include "LexAccessor.h"
#include "Accessor.h"
#include "StyleContext.h"
#include "CharacterSet.h"
#include "LexerModule.h"

#ifndef SCI_NAMESPACE
using namespace Scintilla;
#endif

static const char *const MBoxWordlistDesc[] = {
    0
};

std::map<Sci_Position, char> dataMap;
std::map<Sci_Position, char> stateMap;

class LexerMBox : public ILexer4 {
public:
    LexerMBox() {

    }

    virtual ~LexerMBox() {

    }

    int SCI_METHOD Version() const {
        return lvRelease4;
    }

    void SCI_METHOD Release() {
        delete this;
    }

    const char * SCI_METHOD PropertyNames() {
        return NULL;
    }
    
    int SCI_METHOD PropertyType(const char *name) {
        return -1;
    }

    const char * SCI_METHOD DescribeProperty(const char *name) {
        return NULL;
    }

    Sci_Position SCI_METHOD PropertySet(const char *key, const char *val) {
        return -1;
    }

    const char * SCI_METHOD DescribeWordListSets() {
        return NULL;
    }

    Sci_Position SCI_METHOD WordListSet(int n, const char *wl) {
        return -1;
    }

    void SCI_METHOD Lex(Sci_PositionU startPos, Sci_Position lengthDoc, int initStyle, IDocument *pAccess);

    void SCI_METHOD Fold(Sci_PositionU startPos, Sci_Position lengthDoc, int initStyle, IDocument *pAccess);

    void * SCI_METHOD PrivateCall(int operation, void *pointer) {
        return NULL;
    }

    int SCI_METHOD LineEndTypesSupported() {
        return -1;
    }

    int SCI_METHOD AllocateSubStyles(int styleBase, int numberStyles) {
        return -1;
    }

    int SCI_METHOD SubStylesStart(int styleBase) {
        return -1;
    }

	int SCI_METHOD SubStylesLength(int styleBase) {
        return -1;
    }

	int SCI_METHOD StyleFromSubStyle(int subStyle) {
        return -1;
    }

	int SCI_METHOD PrimaryStyleFromStyle(int style) {
        return -1;
    }

    void SCI_METHOD FreeSubStyles() {
        
    }

	void SCI_METHOD SetIdentifiers(int style, const char *identifiers) {
        
    }

	int SCI_METHOD DistanceToSecondaryStyles() {
        return -1;
    }

	const char * SCI_METHOD GetSubStyleBases() {
        return NULL;
    }

	int SCI_METHOD NamedStyles() {
        return -1;
    }

	const char * SCI_METHOD NameOfStyle(int style) {
        return NULL;
    }

	const char * SCI_METHOD TagsOfStyle(int style) {
        return NULL;
    }

	const char * SCI_METHOD DescriptionOfStyle(int style) {
        return NULL;
    }

    static ILexer4 *LexerFactoryMBox() {
        return new LexerMBox();
    }

};


bool IsFromLine(std::string line) {

    std::string fromKeywordRegex("\\s*From\\s+");
    std::string fromSourceRegex("\\s*(\\S+|(\".*\")+)\\s*");
    std::string fromDateRegex("\\s*(Mon|Tue|Wed|Thu|Fri|Sat|Sun)\\s*(Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec)\\s+([0-2][1-9]|3[0-1])\\s*([0-1][0-9]|2[0-4]):([0-5][0-9]):([0-5][0-9])\\s*\\d{4}\\s*");
    std::regex r(fromKeywordRegex+fromSourceRegex+fromDateRegex);
    return std::regex_match(line, r);
}

bool IsDateLine(std::string line) {

    std::string dateKeywordRegex("\\s*Date:\\s+");
    std::string dateDateRegex("\\s*(0[1-9]|1[0-2])/([0-2][0-9]|3[0-1])/(\\d{2})\\s+([0-1][0-9]|2[0-4]):([0-5][0-9])\\s+(am|pm)\\s*");
    std::regex r(dateKeywordRegex+dateDateRegex);
    return std::regex_match(line, r);
}

bool IsSubjectLine(std::string line) {

    std::string subjectKeywordRegex("\\s*Subject:\\s+");
    std::string subjectTextRegex("\\s*.*?\\s*");

    std::regex r(subjectKeywordRegex+subjectTextRegex);
    return std::regex_match(line, r);
}

bool IsCustomKeywordLine(std::string line) {

    std::string customKeywordRegex("^[A-Za-z]+:.*?\\s+");

    std::regex r(customKeywordRegex);
    return std::regex_match(line, r);
}

void ProcessLines(Sci_PositionU startPos, LexAccessor &styler) {

    std::string lineBuffer;
    Sci_Position currentPos = startPos;
    Sci_Position endPos = styler.Length();
    Sci_Position currentLine = styler.GetLine(startPos);
    while(currentPos < endPos) {
        char c = static_cast<char>(styler.SafeGetCharAt(currentPos++));
        
        lineBuffer += c;

        if (c == '\n') {
            
            if (lineBuffer.substr(0,5) == "From " && IsFromLine(lineBuffer)) {
                if (dataMap.find(currentLine) == dataMap.end()) {
                        dataMap.insert(std::make_pair(currentLine, SCE_MBOX_FROM)); 
                }
                else {
                    dataMap[currentLine] = SCE_MBOX_FROM;
                }
            }
            else if (lineBuffer.substr(0,5) == "Date:" && IsDateLine(lineBuffer)) {
                
                if (dataMap.find(currentLine) == dataMap.end()) {
                    dataMap.insert(std::make_pair(currentLine, SCE_MBOX_DATE)); 
                }
                else {
                    dataMap[currentLine] = SCE_MBOX_DATE;
                }
            }
            else if (lineBuffer.substr(0,8) == "Subject:" && IsSubjectLine(lineBuffer)) {
                if (dataMap.find(currentLine) == dataMap.end()) {
                    dataMap.insert(std::make_pair(currentLine, SCE_MBOX_SUBJECT)); 
                }
                else {
                    dataMap[currentLine] = SCE_MBOX_SUBJECT;
                }
            }
            else {
                if (dataMap.find(currentLine) == dataMap.end()) {
                    dataMap.insert(std::make_pair(currentLine, SCE_MBOX_DEFAULT)); 
                }
                else {
                    dataMap[currentLine] = SCE_MBOX_DEFAULT;
                }
            }

            lineBuffer.clear();
            currentLine++;
        } 
    }

}

void ProcessStates(void) {
    for (auto data = dataMap.begin(); data != dataMap.end(); data++) {

        if (stateMap.find(data->first) == stateMap.end()) {
            stateMap.insert(std::make_pair(data->first, data->second)); 
        }
        else {
            stateMap[data->first] = data->second;
        }

        if (data->second != SCE_MBOX_DEFAULT) {
            Sci_Position begin = data->first;
            Sci_Position end = data->first;
            while (data != dataMap.end() && data->second != SCE_MBOX_DEFAULT) {
                end = data->first;
                data++;
            }
            data--;

            if (dataMap[begin] != SCE_MBOX_FROM) {
                for (Sci_Position i = begin; i <= end; i++) {
                    if (stateMap.find(i) == stateMap.end()) {
                        stateMap.insert(std::make_pair(i, SCE_MBOX_DEFAULT)); 
                    }
                    else {
                        stateMap[i] = SCE_MBOX_DEFAULT;
                    }
                } 
                if (data != dataMap.end())  {
                    continue;
                }
                else {
                    break;
                }
            }

            if (dataMap[begin+1] != SCE_MBOX_DATE) {
                for (Sci_Position i = begin; i <= end; i++) {
                    if (stateMap.find(i) == stateMap.end()) {
                        stateMap.insert(std::make_pair(i, SCE_MBOX_DEFAULT)); 
                    }
                    else {
                        stateMap[i] = SCE_MBOX_DEFAULT;
                    }
                } 
                if (data != dataMap.end())  {
                    continue;
                }
                else {
                    break;
                }
            }

            if (dataMap[begin+2] != SCE_MBOX_SUBJECT) {
                for (Sci_Position i = begin; i <= end; i++) {
                    if (stateMap.find(i) == stateMap.end()) {
                        stateMap.insert(std::make_pair(i, SCE_MBOX_DEFAULT)); 
                    }
                    else {
                        stateMap[i] = SCE_MBOX_DEFAULT;
                    }
                } 
                if (data != dataMap.end())  {
                    continue;
                }
                else {
                    break;
                }
            }

            for (Sci_Position i = begin; i <= end; i++) {
                if (stateMap.find(i) == stateMap.end()) {
                    stateMap.insert(std::make_pair(i, dataMap[i])); 
                }
                else {
                    stateMap[i] = dataMap[i];
                }
                if (data != dataMap.end())  {
                    continue;
                }
                else {
                    break;
                }
            } 

        }
    }
}

void SCI_METHOD LexerMBox::Lex(Sci_PositionU startPos, Sci_Position lengthDoc, int initStyle, IDocument *pAccess) {
    LexAccessor styler(pAccess);
    // ne zaboravi da objasnis za 0
    StyleContext scCTX(0, lengthDoc, initStyle, styler);
    Sci_Position currentLine = styler.GetLine(0);

    ProcessLines(startPos, styler);
    ProcessStates();
    for (; scCTX.More() ; scCTX.Forward()) {
        if (scCTX.atLineStart) {
            scCTX.SetState(stateMap[currentLine]);
        }
        if(scCTX.ch == '\n') {
            currentLine += 1;
            continue;
        }
        switch(scCTX.state) {
            case SCE_MBOX_FROM:
                if (scCTX.Match('m', ' ')) {
                    scCTX.Forward();
                    scCTX.SetState(SCE_MBOX_FROM_VALUE);
                }
                break;
            case SCE_MBOX_DATE:
                if (scCTX.Match(':', ' ')) {
                    scCTX.Forward();
                    scCTX.SetState(SCE_MBOX_DATE_VALUE);
                }
                break;
            case SCE_MBOX_SUBJECT:
                if (scCTX.Match(':', ' ')) {
                    scCTX.Forward();
                    scCTX.SetState(SCE_MBOX_SUBJECT_VALUE);
                }
                break;
        };

    }
    scCTX.Complete();
}

void SCI_METHOD LexerMBox::Fold(Sci_PositionU startPos, Sci_Position lengthDoc, int initStyle, IDocument *pAccess) {

}

LexerModule lmMBox(SCLEX_MBOX, LexerMBox::LexerFactoryMBox, "mbox", MBoxWordlistDesc);
