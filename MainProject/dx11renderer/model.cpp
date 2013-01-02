#include "stdafx.h"
#include "model.h"



CModel::CModel() : texScale(1.0f)
{

}

CModel::~CModel()
{


}


bool CModel::Initialize(EEffectType effectType)
{
 //  m_world  = XMMatrixIdentity();

   switch(effectType)
   {
        case EEffectType::EBasic:

        break;
        case EEffectType::EBasicTextured:
            m_effect = new CTexturedEffect(this);

        break;
        case EEffectType::EParallaxMapping:
            m_effect = new CParallaxEffect(this);

        break;
   }


   return true;
}