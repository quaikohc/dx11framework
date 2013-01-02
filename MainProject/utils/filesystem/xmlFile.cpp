#include "stdafx.h"
#include "xmlFile.h"
#include <iostream>



CXmlFile::CXmlFile()
{

}

CXmlFile::~CXmlFile()
{

}

bool CXmlFile::Open(std::wstring fileName)
{
    pugi::xml_parse_result result = m_document.load_file(fileName.c_str());

    if (result)
    {
        std::cout << "parsed without errors, attr value: ";
    }
    else
    {
        OutputDebugStringA(result.description() );
    }

    return false;
}

CXmlNode CXmlFile::GetRootNode()
{
    CXmlNode retNode;

    pugi::xml_node root = m_document.document_element();
    retNode.SetPugiNode(root);

    return retNode;
}

uint CXmlFile::GetNodesCount()
{
    uint count = 0;
    pugi::xml_node root = m_document.document_element();

    for (pugi::xml_node childNode = root.first_child(); childNode; childNode = childNode.next_sibling())
    {
        count++;
    }
    return count;
}

CXmlNode CXmlFile::GetNodeByIndex(uint index)
{
    CXmlNode retNode;

    pugi::xml_node root = m_document.document_element();

    for (pugi::xml_node childNode = root.first_child(); childNode; childNode = childNode.next_sibling())
    {
        pugi::xml_node_type nodeType = childNode.type();

        if (nodeType == pugi::node_element)
        {
            if(wcscmp(childNode.name(), childNode.name()) == 0)
            {
                retNode.SetPugiNode(childNode);
                break;
            }
        }
        else
        {
            assert(false);
        }
    }

    return retNode;
}
