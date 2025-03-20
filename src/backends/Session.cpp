/// @file Session.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include <cstdlib>
#include <fstream>
#include <list>
#include <string>
#include <vector>

#include "hif/backends/NodeVisitor.hpp"
#include "hif/backends/Session.hpp"
#include "hif/hif.hpp"

namespace hif
{
namespace backends
{

//
//  Class definition
//

//
// CNode()
CNode::CNode()
    : m_eMode(NEW)
    , m_eInstTag(ONLYONE)
    , m_vPath()
    , m_sInstName()
    , m_sEntName()
    , m_pnMovedTo(nullptr)
    , m_pnFirstInst(nullptr)
    , m_pnParent(nullptr)
    , m_lpChild()
{
}

//
// ~CNode()
CNode::~CNode()
{
    std::list<CNode *>::iterator iElt;

    for (iElt = m_lpChild.begin(); iElt != m_lpChild.end(); iElt = m_lpChild.erase(iElt)) {
        delete *iElt;
    }
}

//
// SetParent(CNode * pnParent)
void CNode::SetParent(CNode *pnParent) { m_pnParent = pnParent; }

//
// SetPath(std::string& sPath)
void CNode::SetPath(std::string &sPath)
{
    if (!(sPath.empty())) {
        // FileStructure fPath(RxCfg.eval(sPath));
        messageInfo("MODIFIED   -> " + sPath);
        application_utils::FileStructure fPath(sPath);
        m_vPath = fPath.getAbstractName();
    }
}

//
// SetInstName(std::stringsName)
void CNode::SetInstName(std::string sName) { m_sInstName = sName; }

//
// SetEntName(std::stringsName)
void CNode::SetEntName(std::string sName)
{
    m_sEntName += (m_sEntName.size() != 0) ? std::string("__") + sName : sName;
}

//
// SetInstTag(InstTag_T eTag)
void CNode::SetInstTag(InstTag_T eTag) { m_eInstTag = eTag; }

//
// SetMode(Mode_T eMode)
void CNode::SetMode(Mode_T eMode) { m_eMode = eMode; }

//
// SetAttributes(CNode & rN)
void CNode::SetAttributes(CNode &rN)
{
    m_sInstName   = rN.GetInstName();
    m_sEntName    = rN.GetEntName();
    m_eMode       = rN.GetMode();
    m_eInstTag    = rN.GetInstTag();
    m_pnMovedTo   = rN.MovedToParent();
    m_pnFirstInst = rN.GetOrgNode();
    m_vPath       = rN.GetPath();
}

//
// SetOrgPath(CNode * pnInst)
void CNode::SetOrgPath(CNode *pnInst) { m_pnFirstInst = pnInst; }

//
// SetMovedPath(CNode * rnParent)
void CNode::SetMovedPath(CNode *pnParent) { m_pnMovedTo = pnParent; }

//
// getName()
const std::string CNode::getName()
{
    if (m_sInstName.size() != 0) {
        return m_sInstName;
    } else {
        return m_sEntName;
    }
}

//
// GetPath()
std::vector<std::string> CNode::GetPath()
{
    if (!(m_vPath.empty())) {
        return m_vPath;
    }
    std::vector<std::string> vsPath;
    vsPath.push_back(this->getName());
    CNode *pnCur = m_pnParent;
    while (pnCur) {
        vsPath.insert(vsPath.begin(), pnCur->getName());
        pnCur = pnCur->GetParent();
    }
    return vsPath;
}

//
// operator ==(const CNode & rNr)
bool CNode::operator==(const CNode &rNr)
{
    if (this->m_sInstName != rNr.m_sInstName)
        return false;

    if (this->m_sEntName != rNr.m_sEntName)
        return false;

    return true;
}

//
// acceptVisitor(CNodeVisitor & rVis)
int CNode::acceptVisitor(CNodeVisitor &rVis) { return rVis.visitCNode(*this); }

//
// CSession constants definition
const char *const CSession::CSuffix[] = {"src", "inc", "doc", "lib", "exe", "obj", ""};

//
// CSession methods definition
//
// CSession()
CSession::CSession()
    : m_lpHead()
{
    // nothing to do
}

//
// ~CSession()
CSession::~CSession()
{
    std::list<CNode *>::iterator iElt;

    for (iElt = m_lpHead.begin(); iElt != m_lpHead.end(); iElt = m_lpHead.erase(iElt))
        delete *iElt;
}

//
// FindAnInstance(CNode & rnElt)
CNode *CSession::FindAnInstance(CNode &rnElt)
{
    CFindVisitor fvPath(*this, rnElt, false);
    CNode *pnElt;

    pnElt = fvPath.GetNode();
    if (pnElt) {
        pnElt->SetInstTag(CNode::ORIGINAL);
    }
    return pnElt;
}

//
// FindTheInstance(CNode & rnElt)
CNode *CSession::FindTheInstance(CNode &rnElt)
{
    CFindVisitor fvPath(*this, rnElt, true);

    return fvPath.GetNode();
}

//
// PreCheck()
int CSession::PreCheck()
{
    CUpdateVisitor uvPreCheck(this, CUpdateVisitor::PRECHECK);

    return 0;
}

//
// Update()
int CSession::Update()
{
    CUpdateVisitor uvUpdate(this, CUpdateVisitor::UPDATE);

    return 0;
}

//
// Find(LibraryDef & rlTgt)
std::vector<std::string> CSession::Find(LibraryDef &rlTgt)
{
    CNode nElt, *pnRes;
    std::string nIdent;
    std::vector<std::string> vsNull;

    nIdent = rlTgt.getName();
    nElt.SetEntName(nIdent);
    CFindVisitor fvPath(*this, nElt, false);
    pnRes = fvPath.GetNode();
    if (!pnRes) {
        return vsNull;
    }
    //
    return pnRes->GetPath();
}

//
// Find(DesignUnit & rduTgt)
std::vector<std::string> CSession::Find(DesignUnit &rduTgt)
{
    CNode nElt, *pnRes;
    messageDebugAssert(rduTgt.views.size() == 1, "Unexpected view list size", nullptr, nullptr);
    View *pvTgt = rduTgt.views.front();
    std::string nIdent;
    std::vector<std::string> vsNull;

    nIdent = rduTgt.getName();
    nElt.SetEntName(nIdent);
    if (!pvTgt) {
        return vsNull;
    }
    nIdent = pvTgt->getName();
    nElt.SetEntName(nIdent);
    CFindVisitor fvPath(*this, nElt, false);
    pnRes = fvPath.GetNode();
    if (!pnRes) {
        return vsNull;
    }
    // If is a multi-instantiation node
    // get the original node
    if ((pnRes->GetInstTag() == CNode::ANOTHERONE) && (pnRes->GetOrgNode())) {
        pnRes = pnRes->GetOrgNode();
    }
    //
    return pnRes->GetPath();
}

//
// Find(View & rvTgt)
std::vector<std::string> CSession::Find(View &rvTgt)
{
    CNode nElt, *pnRes;
    DesignUnit *pduTgt = hif::getNearestParent<DesignUnit>(&rvTgt);
    std::string nIdent;
    std::vector<std::string> vsNull;

    if (!pduTgt) {
        return vsNull;
    }
    nIdent = pduTgt->getName();
    nElt.SetEntName(nIdent);
    nIdent = rvTgt.getName();
    nElt.SetEntName(nIdent);
    CFindVisitor fvPath(*this, nElt, false);
    pnRes = fvPath.GetNode();
    if (!pnRes) {
        return vsNull;
    }
    // If is a multi-instantiation node
    // get the original node
    if ((pnRes->GetInstTag() == CNode::ANOTHERONE) && (pnRes->GetOrgNode())) {
        pnRes = pnRes->GetOrgNode();
    }
    //
    return pnRes->GetPath();
}

//
// Find(Instance & riTgt)
std::vector<std::string> CSession::Find(Instance &riTgt)
{
    CNode nElt, *pnRes;
    ViewReference *pvrTgt = dynamic_cast<ViewReference *>(riTgt.getReferencedType());
    messageAssert(pvrTgt != nullptr, "Unexpected referenced type", riTgt.getReferencedType(), nullptr);
    std::string nIdent;
    std::vector<std::string> vsNull;

    if (!pvrTgt) {
        return vsNull;
    }
    nIdent = pvrTgt->getDesignUnit();
    nElt.SetEntName(nIdent);
    nIdent = pvrTgt->getName();
    nElt.SetEntName(nIdent);
    //
    nIdent = riTgt.getName();
    nElt.SetInstName(nIdent);
    //
    CFindVisitor fvPath(*this, nElt, true);
    pnRes = fvPath.GetNode();
    if (!pnRes) {
        return vsNull;
    }
    // If is a multi-instantiation node
    // get the original node
    if ((pnRes->GetInstTag() == CNode::ANOTHERONE) && (pnRes->GetOrgNode())) {
        pnRes = pnRes->GetOrgNode();
    }
    //
    return pnRes->GetPath();
}

//
// Find(std::string& sBase, std::string & sView)
std::vector<std::string> CSession::Find(std::string &sBase, std::string &sView)
{
    CNode nElt, *pnRes;
    std::vector<std::string> vsNull;

    nElt.SetEntName(sBase);
    nElt.SetEntName(sView);
    CFindVisitor fvPath(*this, nElt, false);
    pnRes = fvPath.GetNode();
    if (!pnRes) {
        return vsNull;
    }
    // If is a multi-instantiation node
    // get the original node
    if ((pnRes->GetInstTag() == CNode::ANOTHERONE) && (pnRes->GetOrgNode())) {
        pnRes = pnRes->GetOrgNode();
    }
    //
    return pnRes->GetPath();
}

//
// Find(std::vector<std::string> & vsTgt, std::vector<std::string> & vsSrc)
std::vector<std::string> CSession::Find(std::vector<std::string> &vsTgt, std::vector<std::string> &vsSrc)
{
    std::vector<std::string>::iterator iTgt, iSrc;
    std::vector<std::string> vsRes;
    std::vector<std::string> vsNull;

    if (vsTgt.size() == 0) {
        return vsNull;
    }
    if (vsSrc.size() == 0) {
        return vsTgt;
    }
    iTgt = vsTgt.begin();
    iSrc = vsSrc.begin();
    while ((iTgt != vsTgt.end()) && (iSrc != vsSrc.end()) && (*iTgt == *iSrc)) {
        iTgt++;
        iSrc++;
    }
    while (iSrc != vsSrc.end()) {
        vsRes.push_back(std::string(".."));
        iSrc++;
    }
    while (iTgt != vsTgt.end()) {
        vsRes.push_back(*iTgt);
        iTgt++;
    }
    if (vsRes.size() == 0) {
        vsRes.push_back(std::string("."));
    }
    return vsRes;
}

//
// Find related to DESIGNUNITS
//

//
// Find(DesignUnit & rduTgt, DesignUnit & rduSrc)
std::vector<std::string> CSession::Find(DesignUnit &rduTgt, DesignUnit &rduSrc)
{
    std::vector<std::string> vsTgt = Find(rduTgt);
    std::vector<std::string> vsSrc = Find(rduSrc);

    return Find(vsTgt, vsSrc);
}

//
// Find(DesignUnit & rduTgt, View & rvSrc)
std::vector<std::string> CSession::Find(DesignUnit &rduTgt, View &rvSrc)
{
    std::vector<std::string> vsTgt = Find(rduTgt);
    std::vector<std::string> vsSrc = Find(rvSrc);

    return Find(vsTgt, vsSrc);
}

//
// Find(DesignUnit & rduTgt, Instance & riSrc)
std::vector<std::string> CSession::Find(DesignUnit &rduTgt, Instance &riSrc)
{
    std::vector<std::string> vsTgt = Find(rduTgt);
    std::vector<std::string> vsSrc = Find(riSrc);

    return Find(vsTgt, vsSrc);
}

// Find(DesignUnit & rduTgt, LibraryDef & rlSrc)
std::vector<std::string> CSession::Find(DesignUnit &rduTgt, LibraryDef &rlSrc)
{
#ifdef HIFDIR_DBG
    cout << INDENT << METHOD << "CSession Find(DesignUnit & rduTgt, LibraryDef & rlSrc)" << endl;
#endif // HIFDIR_DBG
    std::vector<std::string> vsTgt = Find(rduTgt);
    std::vector<std::string> vsSrc = Find(rlSrc);

    return Find(vsTgt, vsSrc);
}

// Find(DesignUnit & rduTgt, std::string & sbSrc, std::string svSrc)
std::vector<std::string> CSession::Find(DesignUnit &rduTgt, std::string &sbSrc, std::string &svSrc)
{
#ifdef HIFDIR_DBG
    cout << INDENT << METHOD << "CSession Find(DesignUnit & rduTgt, std::string & sbSrc, std::string & svSrc)" << endl;
#endif // HIFDIR_DBG
    std::vector<std::string> vsTgt = Find(rduTgt);
    std::vector<std::string> vsSrc = Find(sbSrc, svSrc);

    return Find(vsTgt, vsSrc);
}

//
// Find related to VIEWS
//

//
// Find(View & rvTgt, DesignUnit & rduSrc)
std::vector<std::string> CSession::Find(View &rvTgt, DesignUnit &rduSrc)
{
#ifdef HIFDIR_DBG
    cout << INDENT << METHOD << "CSession Find(View & rvTgt, DesignUnit & rduSrc)" << endl;
#endif // HIFDIR_DBG
    std::vector<std::string> vsTgt = Find(rvTgt);
    std::vector<std::string> vsSrc = Find(rduSrc);

    return Find(vsTgt, vsSrc);
}

//
// Find(View & rvTgt, View & rvSrc)
std::vector<std::string> CSession::Find(View &rvTgt, View &rvSrc)
{
#ifdef HIFDIR_DBG
    cout << INDENT << METHOD << "CSession Find(View & rvTgt, View & rvSrc)" << endl;
#endif // HIFDIR_DBG
    std::vector<std::string> vsTgt = Find(rvTgt);
    std::vector<std::string> vsSrc = Find(rvSrc);

    return Find(vsTgt, vsSrc);
}

//
// Find(View & rvTgt, Instance & riSrc)
std::vector<std::string> CSession::Find(View &rvTgt, Instance &riSrc)
{
#ifdef HIFDIR_DBG
    cout << INDENT << METHOD << "CSession Find(View & rvTgt, Instance & riSrc)" << endl;
#endif // HIFDIR_DBG
    std::vector<std::string> vsTgt = Find(rvTgt);
    std::vector<std::string> vsSrc = Find(riSrc);

    return Find(vsTgt, vsSrc);
}

// Find(View & rvTgt, LibraryDef & rlSrc)
std::vector<std::string> CSession::Find(View &rvTgt, LibraryDef &rlSrc)
{
#ifdef HIFDIR_DBG
    cout << INDENT << METHOD << "CSession Find(View & rvTgt, LibraryDef & rlSrc)" << endl;
#endif // HIFDIR_DBG
    std::vector<std::string> vsTgt = Find(rvTgt);
    std::vector<std::string> vsSrc = Find(rlSrc);

    return Find(vsTgt, vsSrc);
}

// Find(View & rvTgt, std::string & sbSrc, std::string svSrc)
std::vector<std::string> CSession::Find(View &rvTgt, std::string &sbSrc, std::string &svSrc)
{
#ifdef HIFDIR_DBG
    cout << INDENT << METHOD << "CSession Find(View & rvTgt, std::string & sbSrc, std::string & svSrc)" << endl;
#endif // HIFDIR_DBG
    std::vector<std::string> vsTgt = Find(rvTgt);
    std::vector<std::string> vsSrc = Find(sbSrc, svSrc);

    return Find(vsTgt, vsSrc);
}

//
// Find related to INSTANCES
//

//
// Find(Instance & riTgt, DesignUnit & rduSrc)
std::vector<std::string> CSession::Find(Instance &riTgt, DesignUnit &rduSrc)
{
#ifdef HIFDIR_DBG
    cout << INDENT << METHOD << "CSession Find(Instance & riTgt, DesignUnit & rduSrc)" << endl;
#endif // HIFDIR_DBG
    std::vector<std::string> vsTgt = Find(riTgt);
    std::vector<std::string> vsSrc = Find(rduSrc);

    return Find(vsTgt, vsSrc);
}

//
// Find(Instance & riTgt, View & rvSrc)
std::vector<std::string> CSession::Find(Instance &riTgt, View &rvSrc)
{
#ifdef HIFDIR_DBG
    cout << INDENT << METHOD << "CSession Find(Instance & riTgt, View & rvSrc)" << endl;
#endif // HIFDIR_DBG
    std::vector<std::string> vsTgt = Find(riTgt);
    std::vector<std::string> vsSrc = Find(rvSrc);

    return Find(vsTgt, vsSrc);
}

//
// Find(Instance & riTgt, Instance & riSrc)
std::vector<std::string> CSession::Find(Instance &riTgt, Instance &riSrc)
{
#ifdef HIFDIR_DBG
    cout << INDENT << METHOD << "CSession Find(Instance & riTgt, Instance & riSrc)" << endl;
#endif // HIFDIR_DBG
    std::vector<std::string> vsTgt = Find(riTgt);
    std::vector<std::string> vsSrc = Find(riSrc);

    return Find(vsTgt, vsSrc);
}

// Find(Instance & riTgt, LibraryDef & rlSrc)
std::vector<std::string> CSession::Find(Instance &riTgt, LibraryDef &rlSrc)
{
#ifdef HIFDIR_DBG
    cout << INDENT << METHOD << "CSession Find(Instance & riTgt, LibraryDef & rlSrc)" << endl;
#endif // HIFDIR_DBG
    std::vector<std::string> vsTgt = Find(riTgt);
    std::vector<std::string> vsSrc = Find(rlSrc);

    return Find(vsTgt, vsSrc);
}

// Find(Instance & riTgt, std::string & sbSrc, std::string svSrc)
std::vector<std::string> CSession::Find(Instance &riTgt, std::string &sbSrc, std::string &svSrc)
{
#ifdef HIFDIR_DBG
    cout << INDENT << METHOD << "CSession Find(Instance & riTgt, std::string & sbSrc, std::string & svSrc)" << endl;
#endif // HIFDIR_DBG
    std::vector<std::string> vsTgt = Find(riTgt);
    std::vector<std::string> vsSrc = Find(sbSrc, svSrc);

    return Find(vsTgt, vsSrc);
}

//
// Find related to LIBRARYDEFS
//

//
// Find(LibraryDef & rlTgt, DesignUnit & rduSrc)
std::vector<std::string> CSession::Find(LibraryDef &rlTgt, DesignUnit &rduSrc)
{
#ifdef HIFDIR_DBG
    cout << INDENT << METHOD << "CSession Find(LibraryDef & rlTgt, DesignUnit & rduSrc)" << endl;
#endif // HIFDIR_DBG
    std::vector<std::string> vsTgt = Find(rlTgt);
    std::vector<std::string> vsSrc = Find(rduSrc);

    return Find(vsTgt, vsSrc);
}

//
// Find(LibraryDef & rlTgt, View & rvSrc)
std::vector<std::string> CSession::Find(LibraryDef &rlTgt, View &rvSrc)
{
#ifdef HIFDIR_DBG
    cout << INDENT << METHOD << "CSession Find(LibraryDef & rlTgt, View & rvSrc)" << endl;
#endif // HIFDIR_DBG
    std::vector<std::string> vsTgt = Find(rlTgt);
    std::vector<std::string> vsSrc = Find(rvSrc);

    return Find(vsTgt, vsSrc);
}

//
// Find(LibraryDef & rlTgt, Instance & riSrc)
std::vector<std::string> CSession::Find(LibraryDef &rlTgt, Instance &riSrc)
{
#ifdef HIFDIR_DBG
    cout << INDENT << METHOD << "CSession Find(LibraryDef & rlTgt, Instance & riSrc)" << endl;
#endif // HIFDIR_DBG
    std::vector<std::string> vsTgt = Find(rlTgt);
    std::vector<std::string> vsSrc = Find(riSrc);

    return Find(vsTgt, vsSrc);
}

// Find(LibraryDef & rlTgt, LibraryDef & rlSrc)
std::vector<std::string> CSession::Find(LibraryDef &rlTgt, LibraryDef &rlSrc)
{
#ifdef HIFDIR_DBG
    cout << INDENT << METHOD << "CSession Find(LibraryDef & rlTgt, LibraryDef & rlSrc)" << endl;
#endif // HIFDIR_DBG
    std::vector<std::string> vsTgt = Find(rlTgt);
    std::vector<std::string> vsSrc = Find(rlSrc);

    return Find(vsTgt, vsSrc);
}

// Find(LibraryDef & rlTgt, std::string & sbSrc, std::string svSrc)
std::vector<std::string> CSession::Find(LibraryDef &rlTgt, std::string &sbSrc, std::string &svSrc)
{
#ifdef HIFDIR_DBG
    cout << INDENT << METHOD << "CSession Find(LibraryDef & rlTgt, std::string & sbSrc, std::string & svSrc)" << endl;
#endif // HIFDIR_DBG
    std::vector<std::string> vsTgt = Find(rlTgt);
    std::vector<std::string> vsSrc = Find(sbSrc, svSrc);

    return Find(vsTgt, vsSrc);
}

//
// Find related to STRINGS (DUBASE)
//

// Find(std::string& sbTgt, std::string & svTgt, DesignUnit & rduSrc)
std::vector<std::string> CSession::Find(std::string &sbTgt, std::string &svTgt, DesignUnit &rduSrc)
{
#ifdef HIFDIR_DBG
    cout << INDENT << METHOD << "CSession Find(std::string& sbTgt, std::string & svTgt, DesignUnit & rduSrc)" << endl;
#endif // HIFDIR_DBG
    std::vector<std::string> vsTgt = Find(sbTgt, svTgt);
    std::vector<std::string> vsSrc = Find(rduSrc);

    return Find(vsTgt, vsSrc);
}

// Find(std::string& sbTgt, std::string & svTgt, View & rvSrc)
std::vector<std::string> CSession::Find(std::string &sbTgt, std::string &svTgt, View &rvSrc)
{
#ifdef HIFDIR_DBG
    cout << INDENT << METHOD << "CSession Find(std::string& sbTgt, std::string & svTgt, View & rvSrc)" << endl;
#endif // HIFDIR_DBG
    std::vector<std::string> vsTgt = Find(sbTgt, svTgt);
    std::vector<std::string> vsSrc = Find(rvSrc);

    return Find(vsTgt, vsSrc);
}

// Find(std::string& sbTgt, std::string & svTgt, Instance & riSrc)
std::vector<std::string> CSession::Find(std::string &sbTgt, std::string &svTgt, Instance &riSrc)
{
#ifdef HIFDIR_DBG
    cout << INDENT << METHOD << "CSession Find(std::string& sbTgt, std::string & svTgt, Instance & riSrc)" << endl;
#endif // HIFDIR_DBG
    std::vector<std::string> vsTgt = Find(sbTgt, svTgt);
    std::vector<std::string> vsSrc = Find(riSrc);

    return Find(vsTgt, vsSrc);
}

// Find(std::string& sbTgt, std::string & svTgt, LibraryDef & rlSrc)
std::vector<std::string> CSession::Find(std::string &sbTgt, std::string &svTgt, LibraryDef &rlSrc)
{
#ifdef HIFDIR_DBG
    cout << INDENT << METHOD << "CSession Find(std::string& sbTgt, std::string & svTgt, LibraryDef & rlSrc)" << endl;
#endif // HIFDIR_DBG
    std::vector<std::string> vsTgt = Find(sbTgt, svTgt);
    std::vector<std::string> vsSrc = Find(rlSrc);

    return Find(vsTgt, vsSrc);
}

// Find(std::string& sbTgt, std::string & svTgt, std::string & sbSrc, std::string svSrc)
std::vector<std::string> CSession::Find(std::string &sbTgt, std::string &svTgt, std::string &sbSrc, std::string &svSrc)
{
#ifdef HIFDIR_DBG
    cout << INDENT << METHOD
         << "CSession Find(std::string& sbTgt, std::string & svTgt, std::string & sbSrc, std::string & svSrc)" << endl;
#endif // HIFDIR_DBG
    std::vector<std::string> vsTgt = Find(sbTgt, svTgt);
    std::vector<std::string> vsSrc = Find(sbSrc, svSrc);

    return Find(vsTgt, vsSrc);
}

//
// Apply Methods
//

//
// Apply(const char * pcLine)
int CSession::Apply(const char *pcLine)
{
#ifdef HIFDIR_DBG
    cout << INDENT << METHOD << "CSession Apply(const char * pcLine)" << endl;
#endif // HIFDIR_DBG
    if (!pcLine) {
        return 4;
    }

    CApplyVisitor avSys(*this, pcLine);
    return avSys.GetErrorCode();
}

//
// Apply(DesignUnit & rduTgt, const char * pcLine, const char * pcSuffix)
int CSession::Apply(DesignUnit &rduTgt, const char *pcLine, const char *pcSuffix)
{
#ifdef HIFDIR_DBG
    cout << INDENT << METHOD << "CSession Apply(DesignUnit & rduTgt, const char * pcLine, const char * pcSuffix)"
         << endl;
#endif // HIFDIR_DBG
    std::vector<std::string> vsPath;
    BList<View>::iterator iElt;
    application_utils::FileStructure fPDir(".");
    application_utils::FileStructure fPADir;

    fPADir = fPDir.getAbsoluteFile();

    if (!pcLine) {
        return 4;
    }

    for (iElt = rduTgt.views.begin(); iElt != rduTgt.views.end(); iElt++) {
        vsPath = Find(**iElt);
        if (vsPath.size() != 0) {
            if (pcSuffix) {
                vsPath.push_back(std::string(pcSuffix));
            }
            application_utils::FileStructure fCDir(vsPath);
            if (!(fCDir.chdir())) {
                return 2;
            }
            if (system(pcLine) == -1) {
                return 5;
            }
            if (!(fPADir.chdir())) {
                return 2;
            }
        }
    }
    return 0;
}

//
// Apply(View & rvTgt, const char * pcLine, const char * pcSuffix)
int CSession::Apply(View &rvTgt, const char *pcLine, const char *pcSuffix)
{
#ifdef HIFDIR_DBG
    cout << INDENT << METHOD << "CSession Apply(View & rvTgt, const char * pcLine, const char * pcSuffix)" << endl;
#endif // HIFDIR_DBG
    std::vector<std::string> vsPath;

    if (!pcLine) {
        return 4;
    }

    vsPath = Find(rvTgt);
    if (pcSuffix) {
        vsPath.push_back(std::string(pcSuffix));
    }
    application_utils::FileStructure fCDir(vsPath);
    if (!(fCDir.chdir())) {
        return 2;
    }
    if (system(pcLine) == -1) {
        return 5;
    }
    return 0;
}

//
// Apply(Instance & riTgt, const char * pcLine, const char * pcSuffix)
int CSession::Apply(Instance &riTgt, const char *pcLine, const char *pcSuffix)
{
#ifdef HIFDIR_DBG
    cout << INDENT << METHOD << "CSession Apply(Instance & riTgt, const char * pcLine, const char * pcSuffix)" << endl;
#endif // HIFDIR_DBG
    std::vector<std::string> vsPath;

    if (!pcLine) {
        return 4;
    }

    vsPath = Find(riTgt);
    if (pcSuffix) {
        vsPath.push_back(std::string(pcSuffix));
    }
    application_utils::FileStructure fCDir(vsPath);
    if (!(fCDir.chdir())) {
        return 2;
    }
    if (system(pcLine) == -1) {
        return 5;
    }
    return 0;
}

//
// Apply(LibraryDef & rlTgt, const char * pcLine, const char * pcSuffix)
int CSession::Apply(LibraryDef &rlTgt, const char *pcLine, const char *pcSuffix)
{
#ifdef HIFDIR_DBG
    cout << INDENT << METHOD << "CSession Apply(LibraryDef & rlTgt, const char * pcLine, const char * pcSuffix)"
         << endl;
#endif // HIFDIR_DBG
    std::vector<std::string> vsPath;
    BList<View>::iterator iElt;
    application_utils::FileStructure fPDir(".");
    application_utils::FileStructure fPADir;

    fPADir = fPDir.getAbsoluteFile();

    if (!pcLine) {
        return 4;
    }

    vsPath = Find(rlTgt);
    if (vsPath.size() != 0) {
        if (pcSuffix) {
            vsPath.push_back(std::string(pcSuffix));
        }
        application_utils::FileStructure fCDir(vsPath);
        if (!(fCDir.chdir())) {
            return 2;
        }
        if (system(pcLine) == -1) {
            return 5;
        }
        if (!(fPADir.chdir())) {
            return 2;
        }
    }
    return 0;
}

//
// Apply(std::string& sbTgt, std::string & svTgt, const char * pcLine, const char * pcSuffix)
int CSession::Apply(std::string &sbTgt, std::string &svTgt, const char *pcLine, const char *pcSuffix)
{
#ifdef HIFDIR_DBG
    cout << INDENT << METHOD
         << "CSession Apply(std::string& sbTgt, std::string & svTgt, const char * pcLine, const char * pcSuffix)"
         << endl;
#endif // HIFDIR_DBG
    std::vector<std::string> vsPath;
    BList<View>::iterator iElt;
    application_utils::FileStructure fPDir(".");
    application_utils::FileStructure fPADir;

    fPADir = fPDir.getAbsoluteFile();

    if (!pcLine) {
        return 4;
    }

    vsPath = Find(sbTgt, svTgt);
    if (vsPath.size() != 0) {
        if (pcSuffix) {
            vsPath.push_back(std::string(pcSuffix));
        }
        application_utils::FileStructure fCDir(vsPath);
        if (!(fCDir.chdir())) {
            return 2;
        }
        if (system(pcLine) == -1) {
            return 5;
        }
        if (!(fPADir.chdir())) {
            return 2;
        }
    }
    return 0;
}

//
// Debug Methods
//

#ifdef HIFDIR_DBG
char *PutnChar(int nI)
{
    char *pStr = new char[nI + 1];
    for (int i = 0; i < nI; i++) {
        pStr[i] = ' ';
    }
    pStr[nI] = '\0';
    return pStr;
}

const char *PrintSVector(std::vector<std::string> vsVec)
{
    std::vector<std::string>::iterator iElt;
    std::string sRes;

    for (iElt = vsVec.begin(); iElt != vsVec.end(); iElt++)
        sRes += *iElt + '#';
    return sRes.c_str();
}
#endif // HIFDIR_DBG

} // namespace backends
} // namespace hif
