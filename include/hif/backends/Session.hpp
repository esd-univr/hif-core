/// @file Session.hpp
/// @brief
/// @copyright (c) 2024 Electronic Systems Design (ESD) Lab @ UniVR
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#ifdef HIFDIR_DBG
#include <iomanip>
#include <iostream>
#endif

#include "hif/classes/classes.hpp"

namespace hif
{
namespace backends
{

#ifdef HIFDIR_DBG

static int nIndent = 0;
char *PutnChar(int nI);

#define INC_INDENT nIndent += 2
#define DEC_INDENT nIndent -= 2
#define INDENT     PutnChar(nIndent)

#define METHOD "<<METHOD>> : "
#endif // HIFDIR_DBG
class CNodeVisitor;

/// @brief Describe a node of the model hierarchy
/// A node is dedicated to an instance or a library
/// @ingroup hif_dir
class CNode
{
public:
    //
    /// @brief Node status
    enum Mode_T : unsigned char {
        UNCHANGED, ///< Node is unchanged
        NEW,       ///< Is a new node
        MOVED,     ///< Node was moved
        REMOVED    ///< Node was removed
    };
    //
    /// @brief Instance tag : used for multi-instantiation
    /// Just one directory must exist.
    enum InstTag_T : unsigned char {
        ONLYONE,   ///< Just one instance
        ORIGINAL,  ///< Original instance
        ANOTHERONE ///< Another one
    };

private:
    CNode(const CNode &);
    CNode &operator=(const CNode &);

    /// @brief Node status
    Mode_T m_eMode;
    /// @brief Instance flag
    InstTag_T m_eInstTag;

    /// @brief Path fixed by the property "path"
    std::vector<std::string> m_vPath;
    /// @brief Instance name
    std::string m_sInstName;
    /// @brief design unit name + view name
    std::string m_sEntName;

    /// @brief Link to the target parent node
    CNode *m_pnMovedTo;
    /// @brief If it the node is already instantiated :
    /// The pointer refers to the target node.
    CNode *m_pnFirstInst;

    /// @brief Parent node
    CNode *m_pnParent;

public:
    /// @brief List of child nodes
    std::list<CNode *> m_lpChild;

    /// @brief Default constructor
    CNode();

    /// @brief Destructor
    virtual ~CNode();

    /// @brief Set a parent node
    void SetParent(CNode *pnParent);

    /// @brief Set a path (access to the node)
    void SetPath(std::string &sPath);
    /// @brief Set an instance name
    void SetInstName(std::string sName);
    /// @brief Set an entity name
    void SetEntName(std::string sName);
    /// @brief Set an instance flag
    void SetInstTag(InstTag_T eTag);
    /// @brief Set a status
    void SetMode(Mode_T eMode);

    /// @brief Set all attributes of a node : status, path, name...
    void SetAttributes(CNode &rN);

    /// @brief Set m_pnFirstInst
    void SetOrgPath(CNode *pnInst);
    /// @brief Set m_pnMovedTo
    void SetMovedPath(CNode *pnParent);

    /// @brief Get the node name
    const std::string getName();
    /// @brief Get an abstract name which defining the node path
    std::vector<std::string> GetPath();
    /// @brief Get an instance name
    const std::string GetInstName() { return m_sInstName; }
    /// @brief Get the entity name
    const std::string GetEntName() { return m_sEntName; }
    /// @brief Get the node status
    Mode_T GetMode() { return m_eMode; }
    /// @brief Get the instance flag
    InstTag_T GetInstTag() { return m_eInstTag; }

    /// @brief Get the first instance of a node
    CNode *GetOrgNode() { return m_pnFirstInst; }
    /// @brief Get the directory where the node must move to
    CNode *MovedToParent() { return m_pnMovedTo; }

    /// @brief Compare two nodes
    bool operator==(const CNode &rNr);

    /// @brief Get a parent node of the current node
    CNode *GetParent() { return m_pnParent; }

    /// @brief Define a visitor access on nodes
    int acceptVisitor(CNodeVisitor &rVis);

#ifdef HIFDIR_DBG
    const char *PrintMode();
    const char *PrintInstTag();
#endif // HIFDIR_DBG
};

//
/// @brief Define a session : set of nodes.
/// @ingroup hif_dir
class CSession
{
public:
    /// @brief List of sub-directories
    /// (applied on a design unit or a library def directory)
    enum Suffix_T : unsigned char {
        SRC, ///< "src" source directory
        INC, ///< "inc" include directory
        DOC, ///< "doc" documentation directory
        LIB, ///< "lib" library directory
        EXE, ///< "exe" executable directory
        OBJ  ///< "obj" object .o directory
    };
    /// @brief List of sub-directories
    /// (applied on a design unit or a library def directory)
    static const char *const CSuffix[];

    /// @brief List of head nodes
    std::list<CNode *> m_lpHead;

    /// @brief Default constructor
    CSession();

    /// @brief Destructor
    ~CSession();

    /// @brief Get a related path between the two inputs : vsTgt - vsSrc
    std::vector<std::string> Find(std::vector<std::string> &vsTgt, std::vector<std::string> &vsSrc);

    /// @brief Return a node instance corresponding to rnElt
    CNode *FindAnInstance(CNode &rnElt);
    /// @brief Return the original node instance corresponding to rnElt
    CNode *FindTheInstance(CNode &rnElt);

    /// @brief Update the session with the PRECHECK flag
    int PreCheck();
    /// @brief Update the session with the UPDATE flag
    int Update();

    /// @brief Absolute Find methods
    std::vector<std::string> Find(hif::LibraryDef &rlTgt);
    std::vector<std::string> Find(hif::DesignUnit &rduTgt);
    std::vector<std::string> Find(hif::View &rvTgt);
    std::vector<std::string> Find(hif::Instance &riTgt);
    std::vector<std::string> Find(std::string &sBase, std::string &sView);

    /// @brief Relative Find methods
    std::vector<std::string> Find(hif::DesignUnit &rduTgt, hif::DesignUnit &rduSrc);
    std::vector<std::string> Find(hif::DesignUnit &rduTgt, hif::View &rvSrc);
    std::vector<std::string> Find(hif::DesignUnit &rduTgt, hif::Instance &riSrc);
    std::vector<std::string> Find(hif::DesignUnit &rduTgt, hif::LibraryDef &rlSrc);
    std::vector<std::string> Find(hif::DesignUnit &rduTgt, std::string &sbSrc, std::string &svSrc);

    /// @brief Relative Find methods
    std::vector<std::string> Find(hif::View &rvTgt, hif::DesignUnit &rduSrc);
    std::vector<std::string> Find(hif::View &rvTgt, hif::View &rvSrc);
    std::vector<std::string> Find(hif::View &rvTgt, hif::Instance &riSrc);
    std::vector<std::string> Find(hif::View &rvTgt, hif::LibraryDef &rlSrc);
    std::vector<std::string> Find(hif::View &rvTgt, std::string &sbSrc, std::string &svSrc);

    /// @brief Relative Find methods
    std::vector<std::string> Find(hif::Instance &riTgt, hif::DesignUnit &rduSrc);
    std::vector<std::string> Find(hif::Instance &riTgt, hif::View &rvSrc);
    std::vector<std::string> Find(hif::Instance &riTgt, hif::Instance &riSrc);
    std::vector<std::string> Find(hif::Instance &riTgt, hif::LibraryDef &rlSrc);
    std::vector<std::string> Find(hif::Instance &riTgt, std::string &sbSrc, std::string &svSrc);

    /// @brief Relative Find methods
    std::vector<std::string> Find(hif::LibraryDef &rlTgt, hif::DesignUnit &rduSrc);
    std::vector<std::string> Find(hif::LibraryDef &rlTgt, hif::View &rvSrc);
    std::vector<std::string> Find(hif::LibraryDef &rlTgt, hif::Instance &riSrc);
    std::vector<std::string> Find(hif::LibraryDef &rlTgt, hif::LibraryDef &rlSrc);
    std::vector<std::string> Find(hif::LibraryDef &rlTgt, std::string &sbSrc, std::string &svSrc);

    /// @brief Relative Find methods : target node = du base + view
    std::vector<std::string> Find(std::string &sbTgt, std::string &svTgt, hif::DesignUnit &rduSrc);
    std::vector<std::string> Find(std::string &sbTgt, std::string &svTgt, hif::View &rvSrc);
    std::vector<std::string> Find(std::string &sbTgt, std::string &svTgt, hif::Instance &riSrc);
    std::vector<std::string> Find(std::string &sbTgt, std::string &svTgt, hif::LibraryDef &rlSrc);
    std::vector<std::string> Find(std::string &sbTgt, std::string &svTgt, std::string &sbSrc, std::string &svSrc);

    /// @brief Apply the command line pcLine
    int Apply(const char *pcLine);
    /// @brief Apply methods on a specific node
    int Apply(hif::DesignUnit &rduTgt, const char *pcLine, const char *pcSuffix);
    int Apply(hif::View &rvTgt, const char *pcLine, const char *pcSuffix);
    int Apply(hif::Instance &riTgt, const char *pcLine, const char *pcSuffix);
    int Apply(hif::LibraryDef &rlTgt, const char *pcLine, const char *pcSuffix);
    /// \attention Target = base + view
    int Apply(std::string &sbTgt, std::string &svTgt, const char *pcLine, const char *pcSuffix);
};

#ifdef HIFDIR_DBG
const char *PrintSVector(std::vector<std::string> vsVec);
#endif // HIFDIR_DBG

} // namespace backends
} // namespace hif
