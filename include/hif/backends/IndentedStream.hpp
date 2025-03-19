/// @file IndentedStream.hpp
/// @brief
/// @copyright (c) 2024-2025 Electronic Systems Design (ESD) Lab @ UniVR This
/// file is distributed under the BSD 2-Clause License. See LICENSE.md for
/// details.

#pragma once

#include <ostream>
#include <string>

#include "hif/application_utils/portability.hpp"

namespace hif
{
namespace backends
{

/// @brief Class implementing an output stream with indentation management.
///
/// By default, the stream indents by 4 spaces.
///
/// @todo Add comment support.
/// @todo Improve 80 column rule support.
///
class IndentedStream : public std::ostream
{
public:
    /// @name Traits.
    /// @{

    /// @brief The string type.
    typedef std::string String;

    /// @brief The size type.
    typedef std::size_t Size;

    /// @brief The output stream.
    typedef std::ostream OutStream;

    /// @brief The bool type.
    typedef bool Bool;

    /// @}

    /// @brief Constructor.
    /// This class aliases the stream, thus, the stream must remain valid
    /// till the deletion of the indented stream.
    ///
    /// @param fileName The output file name w/o extension, or empty for cout.
    /// @param extension The file extension.
    ///
    IndentedStream(const String &fileName, const String &extension);

    /// @brief Constructor.
    /// @param stringBuf An external string buffer.
    ///
    IndentedStream(std::stringbuf *stringBuf);

    /// @brief Constructor.
    /// @param streamBuf An external stream buffer.
    ///
    IndentedStream(std::streambuf *streamBuf);

    virtual ~IndentedStream();

    /// @name General accessors.
    /// @{

    /// @brief Gets the name with extension.
    /// @return The current name.
    const String getName() const;

    /// @brief Gets the name without extension.
    /// @return The current base name.
    const String &getBaseName() const;

    /// @brief Gets the extension.
    /// @return The current extension.
    const String &getExtension() const;

    /// @}

    /// @name Indentation related accessors and modifiers.
    ///@{

    /// @brief Sets a single indentation string.
    /// @param s The indentation.
    void setIndentationString(const String &s);

    /// @brief Gets the indentation string.
    /// @return The current indentation string.
    String getIndentationString() const;

    /// @brief Sets the current indentation level.
    /// @param s The new indentation level.
    void setIndentation(const Size s);

    /// @brief Gets the indentation level.
    /// @return The current name.
    Size getIndentation() const;

    /// @brief Goes to a new line.
    /// @param n The number of new lines.
    void newLine(const int n = 1);

    /// @brief Indents by one.
    void indent();

    /// @brief Unindents by one.
    void unindent();

    /// @brief Sets the maximum column width.
    /// A value of zero means no maximum column width.
    /// @param s The new column width.
    void setColumnWidth(const Size s);

    /// @brief Gets the maximum column width.
    /// @return The column width.
    Size getColumnWidth() const;

    /// @brief Sets whether to print next content as comment.
    /// @param r True if the rule must be used.
    void setCommentMode(const Bool r);

    /// @brief Gets if printing next content as comment is used.
    /// @return True if the rule is used.
    Bool isCommentMode() const;

    /// @brief Sets the comment string. Default is "// ", "// " and "".
    /// @param prefix The comment prefix string.
    /// @param infix The comment infix string.
    /// @param postfix The comment postfix string.
    void setComment(const String &prefix, const String &infix, const String &postfix);

    /// @brief Gets the comment prefix.
    /// @return The comment prefix.
    const String &getCommentPrefix() const;

    /// @brief Gets comment infix string.
    /// @return the comment infix.
    const String &getCommentInfix() const;

    /// @brief Gets comment postfix string.
    /// @return the comment postfix.
    const String &getCommentPostfix() const;

    /// @brief Sets whether to print next content as macro.
    /// @param r True if the rule must be used.
    void setMacroMode(const Bool r);

    /// @brief Gets if printing next content as macro is used.
    /// @return True if the rule is used.
    Bool isMacroMode() const;

    /// @brief Gets the current string printed before macro new line.
    /// @return The string.
    const String &getMacroNewLine() const;

    /// @brief Sets the current string printed before macro new line.
    /// @param n The string.
    void setMacroNewLine(const String &n);

    /// @brief Sets wether indent when auto-wrapping.
    /// @param ind True if must be indented.
    void setIndentWhenWrapping(const Bool ind);

    /// @brief Gets wether indent when auto-wrapping.
    /// @return True if must be indented.
    Bool getIndentWhenWrapping() const;

    /// @brief Sets on which characters the stream can wrap.
    /// Default is: " \\t{}()[]". An empty string means on any char.
    /// @param s A string containing the possible wrapping chars.
    void setWrappingChars(const String &s);

    /// @brief Gets on which chars the stream can wrap.
    /// @return The current wrapping set of chars.
    const String &getWrappingChars() const;

    /// @brief Sets whether wrap text with double quotes.
    void setStringMode(const bool isString);

    /// @brief Tells whether text is wrapped with double quotes.
    bool isStringMode() const;

    /// @}

    /// @name File auto-split methods.
    /// @{

    /// @brief Sets the maximum number of lines for file.
    /// Zero means no maximum.
    /// @param max The maximum number of lines.
    void setMaxLines(const Size max);

    /// @brief Gets the maximum number of lines for file.
    /// Zero means no maximum.
    /// @return the current maximum.
    Size getMaxLines();

    /// @brief Initializes a block.
    /// Files can be split only when no blocks are active.
    void openBlock();

    /// @brief Closes a block.
    void closeBlock();

    /// @brief Checks if a block is active.
    Bool isBlockActive();

    /// @brief Initializes a top common block.
    /// When a file is plit, the common block is re-printed at the beginning.
    void openCommonTopBlock();

    /// @brief Closes a top common block.
    void closeCommonTopBlock();

    /// @brief Initializes a bottom common block.
    /// When a file is plit, the common block is re-printed at the ending.
    void openCommonBottomBlock();

    /// @brief Closes bottom a common block.
    void closeCommonBottomBlock();

    /// @brief Checks if a common block is active.
    Bool isCommonBlockActive();

    /// @}

private:
    IndentedStream(const IndentedStream &);
    IndentedStream &operator=(const IndentedStream &);
};

/// @name Manipulators.
///@{

/// @brief Indents the stream by one.
/// If the output stream is not an indented stream, it does nothing.

IndentedStream::OutStream &indent(IndentedStream::OutStream &);

/// @brief Un-indents the stream by one.
/// If the output stream is not an indented stream, it does nothing.

IndentedStream::OutStream &unindent(IndentedStream::OutStream &);

/// @brief Sets indent when wrapping.
/// If the output stream is not an indented stream, it does nothing.

IndentedStream::OutStream &indentWhenWrapping(IndentedStream::OutStream &);

/// @brief Sets to not indent when wrapping.
/// If the output stream is not an indented stream, it does nothing.

IndentedStream::OutStream &noIndentWhenWrapping(IndentedStream::OutStream &);

/// @brief Inits a block.
/// If the output stream is not an indented stream, it does nothing.

IndentedStream::OutStream &openBlock(IndentedStream::OutStream &);

/// @brief Closes a block.
/// If the output stream is not an indented stream, it does nothing.

IndentedStream::OutStream &closeBlock(IndentedStream::OutStream &);

/// @brief Inits a common block.
/// If the output stream is not an indented stream, it does nothing.

IndentedStream::OutStream &openCommonBlock(IndentedStream::OutStream &);

/// @brief Closes a common block.
/// If the output stream is not an indented stream, it does nothing.

IndentedStream::OutStream &closeCommonBlock(IndentedStream::OutStream &);

/// @brief Inits a common block.
/// If the output stream is not an indented stream, it does nothing.

IndentedStream::OutStream &openComment(IndentedStream::OutStream &);

/// @brief Closes a common block.
/// If the output stream is not an indented stream, it does nothing.

IndentedStream::OutStream &closeComment(IndentedStream::OutStream &);

/// @brief Inits a string mode.
/// If the output stream is not an indented stream, it does nothing.

IndentedStream::OutStream &openString(IndentedStream::OutStream &);

/// @brief Closes a string mode.
/// If the output stream is not an indented stream, it does nothing.

IndentedStream::OutStream &closeString(IndentedStream::OutStream &);

///@}

} // namespace backends
} // namespace hif
