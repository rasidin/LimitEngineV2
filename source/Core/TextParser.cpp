/***********************************************************
 LIMITEngine Source File
 Copyright (C), LIMITGAME, 2020
 -----------------------------------------------------------
 @file  LE_TextParser.cpp
 @brief Text Parser
 @author minseob (leeminseob@outlook.com)
 ***********************************************************/

#include <string.h>
#include "Core/TextParser.h"

namespace LimitEngine {
TextParser::TextParser()
{
    mNodes.Clear();
}

TextParser::TextParser(const char *text)
{
    mNodes.Clear();
    Parse(text);
}

TextParser::~TextParser()
{
    for(uint32 i=0;i<mNodes.count();i++)
    {
        delete mNodes[i];
    }
}

bool TextParser::Parse(const char *text)
{
#define TEXTPARSER_MAX_WORD     256
    char buf[TEXTPARSER_MAX_WORD];
    uint32 word_length = 0;
    const char *ptr = text;
    char currentWord = *(ptr++);
    NODE *currentNode = NULL;
    NODE *parentNode = NULL;
    bool sequenceIn = false;
    bool escapeIn = false;
    bool stringIn = false;
    bool comment = false;
    while (currentWord) {
        if (!stringIn && currentWord < 0x21) {
            if (!comment)
            {
                if (word_length) {
                    buf[word_length] = 0;
                    inputData(parentNode, &currentNode, buf, word_length, sequenceIn);
                }
                if (*ptr == '\r') {
                    ptr++;
                }
                if (*ptr == '\n') {
                    ptr++;
                }
            }
            else if (currentWord == '\r') {
                if (*ptr == '\n') {
                    ptr++;
                }
                comment = false;
            }
            else if (currentWord == '\n') {
                comment = false;
            }
            word_length = 0;
        }
        else if(!comment) {
            if (stringIn) {
                if (currentWord == '\\' && !escapeIn) {
                    escapeIn = true;
                }
                else if (currentWord == '\"' && !escapeIn) {
                    stringIn = false;
                }
                else {
                    buf[word_length++] = currentWord;
                    escapeIn = false;
                }
            }
            else {
                if (currentWord == '{') {
                    if (currentNode == nullptr) {
                        addNewNode(parentNode, nullptr, &currentNode);
                    }
                    parentNode = currentNode;
                    currentNode = NULL;
                    //String NewNodeName;
                    //if (word_length) {
                    //    buf[word_length] = 0;
                    //    NewNodeName = buf;
                    //    word_length = 0;
                    //}
                    //else {
                    //    if (parentNode->values.count()) {
                    //        NewNodeName = parentNode->values[parentNode->values.count() - 1];
                    //        parentNode->values.erase(parentNode->values.count() - 1);
                    //    }
                    //}
                    //addNewNode(parentNode, NewNodeName.GetCharPtr(), &currentNode);
                }
                else if (currentWord == '}') {
                    parentNode = parentNode->parent;
                    buf[word_length] = 0;
                    inputData(parentNode, &currentNode, buf, word_length, sequenceIn);
                    word_length = 0;
                }
                else if (currentWord == '[') {
                    sequenceIn = true;
                    buf[word_length] = 0;
                    inputData(parentNode, &currentNode, buf, word_length, sequenceIn);
                    word_length = 0;
                }
                else if (currentWord == ']') {
                    if (word_length) {
                        buf[word_length] = 0;
                        inputData(parentNode, &currentNode, buf, word_length, sequenceIn);
                        word_length = 0;
                    }
                    sequenceIn = false;
                    currentNode = NULL;
                }
                else if (currentWord == '\"') {
                    stringIn = true;
                }
                else if (currentWord == '/') {
                    if (word_length && buf[word_length-1] == '/') 
                    {
                        comment = true;   
                    }
                    else buf[word_length++] = currentWord;
                }
                else {
                    buf[word_length++] = currentWord;
                }
            }
        }
        currentWord = *(ptr++);
    }
    return true;
}

bool TextParser::Save(const char *filename)
{
    if (!filename) return false;
    FILE *fp = nullptr;
    fopen_s(&fp, filename, "w");
    if (!fp) return false;
    int indent = 0;
    for(uint32 i=0;i<mNodes.size();i++)
    {
        mNodes[i]->Save(fp, indent);
    }
    return true;
}

void TextParser::NODE::Save(FILE *fp, int indent)
{
    if (children.size())
    {
        for(int i=0;i<indent;i++) fprintf(fp, "\t");
        fprintf(fp, name.GetCharPtr());
        fprintf(fp, " {\n");
        for(uint32 i=0;i<children.size();i++)
        {
            children[i]->Save(fp, indent+1);
        }
        for(int i=0;i<indent;i++) fprintf(fp, "\t");
        fprintf(fp, "}\n");
    }
    else
    {
        for(int i=0;i<indent;i++) fprintf(fp, "\t");
        fprintf(fp, name.GetCharPtr());
        if (values.size()>1) fprintf(fp, " [");
        fprintf(fp, " ");
        for(int i=0, count=static_cast<int>(values.size());i<count;i++) 
        {
            if (!IsValueNumber(i)) fprintf(fp, "\"");
            fprintf(fp, values[i].GetCharPtr());
            if (!IsValueNumber(i)) fprintf(fp, "\"");
            fprintf(fp, " ");
        }
        if (values.size()>1) fprintf(fp, "]");
        fprintf(fp, "\n");
    }
}

TextParser::NODE* TextParser::GetNode(const char *name)
{
    for(uint32 n=0;n<mNodes.count();n++)
    {
        if (mNodes[n]->name == name) return mNodes[n];
        mNodes[n]->FindChild(name);
    }
    return NULL;
}

void TextParser::addNewNode(NODE *Parent, const char *NodeName, NODE **OutNode)
{
    *OutNode = new NODE();
    (*OutNode)->name = NodeName;
    (*OutNode)->parent = Parent;
    if (Parent) Parent->children.Add(*OutNode);
    else mNodes.Add(*OutNode);
}

void TextParser::inputData(NODE *parent, NODE **node, char *buf, uint32 length, bool sequenceIn)
{
    if (length)
    {
        if (*node) {
            (*node)->values.Add(buf);
            if (!sequenceIn) *node = NULL;
        }
        else {
            *node = new NODE();
            (*node)->name = buf;
            (*node)->parent = parent;
            if (parent) parent->children.Add(*node);
            else mNodes.Add(*node);
        }
    }    
}
}