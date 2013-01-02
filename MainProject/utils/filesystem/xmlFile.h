#pragma once

#include "pugiXML\pugixml.hpp"


class CXmlNode
{
    pugi::xml_node m_node;

public:
    CXmlNode(){}
    CXmlNode(pugi::xml_node node):m_node(node){}

    CXmlNode GetChildByIndex(uint index)
    {
        CXmlNode ret;

        uint count = 0;

        for (pugi::xml_node childNode = m_node.first_child(); childNode; childNode = childNode.next_sibling())
        {
            if(count == index)
            {
                ret.SetPugiNode(childNode);
                break;
            }
            count++;
        }

        return ret;
    }

    CXmlNode GetChildByName(const std::wstring& name)
    {
        CXmlNode ret;
        return ret;
    }

    uint GetChildsCount()
    {
        uint count = 0;
      
        for (pugi::xml_node childNode = m_node.first_child(); childNode; childNode = childNode.next_sibling())
            count++;

        return count;
    }
    uint GetAttributesCount()
    {
        return 0;
    }

    void SetPugiNode(pugi::xml_node node) { m_node = node; }

    float GetFloatAttr(const std::wstring& attrName)
    {
        return m_node.attribute(attrName.c_str()).as_float();
    }
    bool GetBoolAttr(const std::wstring& attrName)
    {
        return m_node.attribute(attrName.c_str()).as_bool();
    }
    bool GetIntAttr(const std::wstring& attrName)
    {
        return m_node.attribute(attrName.c_str()).as_int();
    }
    bool GetUIntAttr(const std::wstring& attrName)
    {
        return m_node.attribute(attrName.c_str()).as_uint();
    }
    std::wstring GetStringAttr(const std::wstring& attrName)
    {
        return m_node.attribute(attrName.c_str()).as_string();
    }

    XMFLOAT3 GetVec3Attr(const std::wstring& attrName)
    {
        XMFLOAT3 ret;

        std::wstring attrValue = GetStringAttr(attrName);
        std::wstringstream ss(attrValue);
        std::wstring item;

        float values[3];
        for(int i = 0; i < 3; i++)
        {
            std::getline(ss, item, L' ');
            values[i] = static_cast<float>(wcstod(item.c_str(), 0));
        }

        ret.x =  values[0];
        ret.y =  values[1];
        ret.z =  values[2];

        return ret;
    }

    XMFLOAT4 GetVec43Attr(const std::wstring& attrName)
    {
        XMFLOAT4 ret;

        std::wstring attrValue = GetStringAttr(attrName);
        std::wstringstream ss(attrValue);
        std::wstring item;

        float values[4];
        for(int i = 0; i < 4; i++)
        {
            std::getline(ss, item, L' ');
            values[i] = static_cast<float>(wcstod(item.c_str(), 0));
        }

        ret.x =  values[0];
        ret.y =  values[1];
        ret.z =  values[2];
        ret.w =  values[3];

        return ret;
    }
};


class CXmlFile
{
    pugi::xml_document m_document;

    

public:
    CXmlFile();
    ~CXmlFile();

    bool        Open(std::wstring fileName);

    CXmlNode    GetRootNode();
    CXmlNode    GetNodeByIndex(uint index);

    uint        GetNodesCount();
 
};
