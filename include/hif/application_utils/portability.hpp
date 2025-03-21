/// @file portability.hpp
/// @brief
/// @copyright (c) 2024-2025 Electronic Systems Design (ESD) Lab @ UniVR This
/// file is distributed under the BSD 2-Clause License. See LICENSE.md for
/// details.

#pragma once

#include <cstdio>
#include <string>
#include <sys/stat.h>

#if (defined _MSC_VER)

#define HIF_DEPRECATED(msg) __declspec(deprecated(msg))
// dll-interface export problem
#pragma warning(disable : 4251)
// no suitable template instantation
#pragma warning(disable : 4661)

#if (defined COMPILE_HIF_LIB)
// Compiling dynamic hif
#define __declspec(dllexport)
#elif (defined USE_HIF_LIB)
// Linking dynamic libs
#define __declspec(dllimport)
#else
// Compiling or linking static libs
#define
#endif
#if (defined _WIN64)
#define HIF_64 1
#elif (defined _WIN32)
#define HIF_64 0
#error "HIF requires to be compiled with 64-bit compiler"
#else
#error "Unexpected architecture"
#endif

#elif (defined __GNUC__)

// Linux
#include <dirent.h>
#include <sys/types.h>

#define HIF_DEPRECATED(msg)     __attribute__((deprecated))
#define HIF_DIAGNOSTIC_PUSH_POP (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ > 5))
#pragma GCC diagnostic warning "-Wdeprecated-declarations"

#if (defined __x86_64__)
#define HIF_64 1
#elif (defined __i386__)
#define HIF_64 0
#error "HIF requires to be compiled with 64-bit compiler"
#else
#error "Unexpected architecture"
#endif

#else
#error "Unsupported compiler"
#endif

namespace hif
{
namespace application_utils
{

/// @name Common portability methods.
/// @{

/// @brief Case-insensitive string comparison.
/// @param s1 First null-terminated string.
/// @param s2 Second null-terminated string.
/// @return 0 if the strings are equal (case-insensitive),
///         negative value if s1 is less than s2 (case-insensitive),
///         positive value if s1 is greater than s2 (case-insensitive).

auto hif_strcasecmp(const char *s1, const char *s2) -> int;

/// @brief Case-insensitive string comparison with size limit.
/// @param s1 First null-terminated string.
/// @param s2 Second null-terminated string.
/// @param size Maximum number of characters to compare.
/// @return 0 if the strings are equal (case-insensitive),
///         negative value if s1 is less than s2 (case-insensitive),
///         positive value if s1 is greater than s2 (case-insensitive).

auto hif_strncasecmp(const char *s1, const char *s2, size_t size) -> int;

/// @brief Converts a string to a long long integer.
/// @param s1 The string to convert.
/// @param s2 Pointer to the next character in the string after the number.
/// @param base Numerical base to use for conversion.
/// @return The converted value as a long long integer.

auto hif_strtoll(const char *s1, char **s2, int base) -> long long;

/// @brief Gets the current working directory.
/// @param buf Buffer to store the directory path.
/// @param size Size of the buffer.
/// @return Pointer to the buffer containing the directory path, or nullptr on failure.

auto hif_getcwd(char *buf, size_t size) -> char *;

/// @brief Changes the file mode (permissions) of a given file.
/// @param path Path to the file.
/// @param m New mode to set.
/// @return 0 on success, or -1 on failure.

auto hif_chmod(const char *path, int m) -> int;

/// @brief Changes the current working directory.
/// @param path Path to the directory to switch to.
/// @return 0 on success, or -1 on failure.

auto hif_chdir(const char *path) -> int;

/// @brief Removes a directory.
/// @param path Path to the directory to remove.
/// @return 0 on success, or -1 on failure.

auto hif_rmdir(const char *path) -> int;

/// @brief Creates a directory with the specified mode.
/// @param path Path to the directory to create.
/// @param mode Permissions mode for the new directory.
/// @return 0 on success, or -1 on failure.

auto hif_mkdir(const char *path, int mode) -> int;

/// @brief Duplicates a string.
/// @param s The string to duplicate.
/// @return Pointer to the newly allocated string, or nullptr on failure.

auto hif_strdup(const char *s) -> char *;

/// @brief Gets the file descriptor of a file stream.
/// @param f The file stream.
/// @return The file descriptor, or -1 on failure.

auto hif_fileno(FILE *f) -> int;

/// @brief Checks if the file descriptor refers to a terminal.
/// @param fd The file descriptor.
/// @return Non-zero if the file descriptor refers to a terminal, 0 otherwise.

auto hif_isatty(int fd) -> int;

/// @brief Checks if a given mode corresponds to a directory.
/// @param mode The mode to check.
/// @return Non-zero if the mode corresponds to a directory, 0 otherwise.

auto hif_isdir(unsigned int mode) -> int;

/// @brief Checks if a given mode corresponds to a symbolic link.
/// @param mode The mode to check.
/// @return Non-zero if the mode corresponds to a symbolic link, 0 otherwise.

auto hif_islink(unsigned int mode) -> int;

/// @brief Gets the size of a file from its stat structure.
/// @param s The stat structure of the file.
/// @return The size of the file in bytes.

auto hif_getfilesize(struct stat &s) -> int;

/// @brief Creates a symbolic link.
/// @param s1 Path to the target file.
/// @param s2 Path to the symbolic link to create.
/// @return 0 on success, or -1 on failure.

auto hif_symlink(const char *s1, const char *s2) -> int;

/// @brief Rounds a double to the nearest integer value.
/// @param d The double value to round.
/// @return The rounded value as a double.

auto hif_round(double d) -> double;

/// @brief Calculates the base-2 logarithm of a double.
/// @param d The double value.
/// @return The base-2 logarithm of the value.

auto hif_log2(double d) -> double;

/// @brief Opens a memory buffer as a file stream.
/// @param buffer The memory buffer.
/// @param size The size of the buffer.
/// @param mode The file mode (e.g., "r", "w").
/// @param path Path for the memory file (used for debugging or symbolic purposes).
/// @return Pointer to the opened file stream, or nullptr on failure.

auto hif_fmemopen(const char *buffer, int size, const char *mode, const char *path) -> FILE *;

/// @brief Opens a file descriptor as a file stream.
/// @param fd The file descriptor.
/// @param mode The file mode (e.g., "r", "w").
/// @return Pointer to the opened file stream, or nullptr on failure.

auto hif_fdopen(int fd, const char *mode) -> FILE *;

/// @brief Gets the current time as a string.
/// @return The current time as a string in HH:MM:SS format.

auto hif_getCurrentTimeAsString() -> std::string;

/// @brief Gets the current date as a string.
/// @return The current date as a string in YYYY-MM-DD format.

auto hif_getCurrentDateAsString() -> std::string;

/// @brief Gets the current date and time as a string.
/// @return The current date and time as a string in YYYY-MM-DD HH:MM:SS format.

auto hif_getCurrentDateAndTimeAsString() -> std::string;

/// @brief Gets the current date and time in FMI-compatible string format.
/// @return The current date and time as a string in YYYYMMDDTHHMMSS format.

auto hif_getCurrentDateAndTimeAsFMIStringFormat() -> std::string;

/// @}

/// @name Common portability constants.
/// @{

/// @brief Read, write, and execute permissions for the user.

extern const int PERMISSION_RWX_USR;

/// @brief Read permission for the user.

extern const int PERMISSION_R_USR;

/// @brief Write permission for the user.

extern const int PERMISSION_W_USR;

/// @brief Execute permission for the user.

extern const int PERMISSION_X_USR;

/// @brief Read, write, and execute permissions for the group.

extern const int PERMISSION_RWX_GRP;

/// @brief Read permission for the group.

extern const int PERMISSION_R_GRP;

/// @brief Write permission for the group.

extern const int PERMISSION_W_GRP;

/// @brief Execute permission for the group.

extern const int PERMISSION_X_GRP;

/// @brief Read, write, and execute permissions for others.

extern const int PERMISSION_RWX_OTH;

/// @brief Read permission for others.

extern const int PERMISSION_R_OTH;

/// @brief Write permission for others.

extern const int PERMISSION_W_OTH;

/// @brief Execute permission for others.

extern const int PERMISSION_X_OTH;

/// @}

} // namespace application_utils
} // namespace hif

/// @brief Helper macro to instantiate a class template for all HIF object types.
/// @details This macro generates explicit template instantiations for a given class `TemplateClass`
/// across all HIF object types, ensuring all required specializations are available
/// during linking. Use this macro when implementing classes that operate on HIF objects.
/// @param TemplateClass The class template to be instantiated.
#define HIF_INSTANTIATE_TEMPLATE_FOR_HIF_OBJECTS(TemplateClass)                                                        \
    template class TemplateClass<Action>;                                                                              \
    template class TemplateClass<Aggregate>;                                                                           \
    template class TemplateClass<AggregateAlt>;                                                                        \
    template class TemplateClass<Alias>;                                                                               \
    template class TemplateClass<Alt>;                                                                                 \
    template class TemplateClass<Array>;                                                                               \
    template class TemplateClass<Assign>;                                                                              \
    template class TemplateClass<BaseContents>;                                                                        \
    template class TemplateClass<Bit>;                                                                                 \
    template class TemplateClass<BitValue>;                                                                            \
    template class TemplateClass<Bitvector>;                                                                           \
    template class TemplateClass<BitvectorValue>;                                                                      \
    template class TemplateClass<Bool>;                                                                                \
    template class TemplateClass<BoolValue>;                                                                           \
    template class TemplateClass<Break>;                                                                               \
    template class TemplateClass<Cast>;                                                                                \
    template class TemplateClass<Char>;                                                                                \
    template class TemplateClass<CharValue>;                                                                           \
    template class TemplateClass<CompositeType>;                                                                       \
    template class TemplateClass<Const>;                                                                               \
    template class TemplateClass<ConstValue>;                                                                          \
    template class TemplateClass<Contents>;                                                                            \
    template class TemplateClass<Continue>;                                                                            \
    template class TemplateClass<DataDeclaration>;                                                                     \
    template class TemplateClass<Declaration>;                                                                         \
    template class TemplateClass<DesignUnit>;                                                                          \
    template class TemplateClass<Entity>;                                                                              \
    template class TemplateClass<Enum>;                                                                                \
    template class TemplateClass<EnumValue>;                                                                           \
    template class TemplateClass<Event>;                                                                               \
    template class TemplateClass<Expression>;                                                                          \
    template class TemplateClass<Field>;                                                                               \
    template class TemplateClass<FieldReference>;                                                                      \
    template class TemplateClass<File>;                                                                                \
    template class TemplateClass<For>;                                                                                 \
    template class TemplateClass<ForGenerate>;                                                                         \
    template class TemplateClass<Function>;                                                                            \
    template class TemplateClass<FunctionCall>;                                                                        \
    template class TemplateClass<Generate>;                                                                            \
    template class TemplateClass<GlobalAction>;                                                                        \
    template class TemplateClass<Identifier>;                                                                          \
    template class TemplateClass<If>;                                                                                  \
    template class TemplateClass<IfAlt>;                                                                               \
    template class TemplateClass<IfGenerate>;                                                                          \
    template class TemplateClass<Instance>;                                                                            \
    template class TemplateClass<Int>;                                                                                 \
    template class TemplateClass<IntValue>;                                                                            \
    template class TemplateClass<Library>;                                                                             \
    template class TemplateClass<LibraryDef>;                                                                          \
    template class TemplateClass<Member>;                                                                              \
    template class TemplateClass<Null>;                                                                                \
    template class TemplateClass<Transition>;                                                                          \
    template class TemplateClass<Object>;                                                                              \
    template class TemplateClass<PPAssign>;                                                                            \
    template class TemplateClass<Parameter>;                                                                           \
    template class TemplateClass<ParameterAssign>;                                                                     \
    template class TemplateClass<Pointer>;                                                                             \
    template class TemplateClass<Port>;                                                                                \
    template class TemplateClass<PortAssign>;                                                                          \
    template class TemplateClass<PrefixedReference>;                                                                   \
    template class TemplateClass<Procedure>;                                                                           \
    template class TemplateClass<ProcedureCall>;                                                                       \
    template class TemplateClass<Range>;                                                                               \
    template class TemplateClass<Real>;                                                                                \
    template class TemplateClass<RealValue>;                                                                           \
    template class TemplateClass<Record>;                                                                              \
    template class TemplateClass<RecordValue>;                                                                         \
    template class TemplateClass<RecordValueAlt>;                                                                      \
    template class TemplateClass<Reference>;                                                                           \
    template class TemplateClass<ReferencedAssign>;                                                                    \
    template class TemplateClass<ReferencedType>;                                                                      \
    template class TemplateClass<Return>;                                                                              \
    template class TemplateClass<Scope>;                                                                               \
    template class TemplateClass<ScopedType>;                                                                          \
    template class TemplateClass<Signal>;                                                                              \
    template class TemplateClass<Signed>;                                                                              \
    template class TemplateClass<SimpleType>;                                                                          \
    template class TemplateClass<Slice>;                                                                               \
    template class TemplateClass<State>;                                                                               \
    template class TemplateClass<StateTable>;                                                                          \
    template class TemplateClass<String>;                                                                              \
    template class TemplateClass<SubProgram>;                                                                          \
    template class TemplateClass<Switch>;                                                                              \
    template class TemplateClass<SwitchAlt>;                                                                           \
    template class TemplateClass<System>;                                                                              \
    template class TemplateClass<TPAssign>;                                                                            \
    template class TemplateClass<StringValue>;                                                                         \
    template class TemplateClass<Time>;                                                                                \
    template class TemplateClass<TimeValue>;                                                                           \
    template class TemplateClass<Type>;                                                                                \
    template class TemplateClass<TypeDeclaration>;                                                                     \
    template class TemplateClass<TypeDef>;                                                                             \
    template class TemplateClass<TypeReference>;                                                                       \
    template class TemplateClass<TypeTP>;                                                                              \
    template class TemplateClass<TypeTPAssign>;                                                                        \
    template class TemplateClass<TypedObject>;                                                                         \
    template class TemplateClass<Unsigned>;                                                                            \
    template class TemplateClass<Value>;                                                                               \
    template class TemplateClass<ValueStatement>;                                                                      \
    template class TemplateClass<ValueTP>;                                                                             \
    template class TemplateClass<ValueTPAssign>;                                                                       \
    template class TemplateClass<Variable>;                                                                            \
    template class TemplateClass<View>;                                                                                \
    template class TemplateClass<ViewReference>;                                                                       \
    template class TemplateClass<Wait>;                                                                                \
    template class TemplateClass<When>;                                                                                \
    template class TemplateClass<WhenAlt>;                                                                             \
    template class TemplateClass<While>;                                                                               \
    template class TemplateClass<With>;                                                                                \
    template class TemplateClass<WithAlt>;

/// @brief Helper macro to instantiate a method template for all HIF object types.
/// @details This macro generates explicit template instantiations for a method `HIF_TEMPLATE_METHOD`
/// across all HIF object types. Before using this macro, define the macro `HIF_TEMPLATE_METHOD(T)`
/// as the method to be instantiated.
/// @note Ensure that `HIF_TEMPLATE_METHOD(T)` is properly defined before invoking this macro.
#define HIF_INSTANTIATE_METHOD()                                                                                       \
    template HIF_TEMPLATE_METHOD(Object);                                                                              \
    template HIF_TEMPLATE_METHOD(Action);                                                                              \
    template HIF_TEMPLATE_METHOD(Aggregate);                                                                           \
    template HIF_TEMPLATE_METHOD(AggregateAlt);                                                                        \
    template HIF_TEMPLATE_METHOD(Alias);                                                                               \
    template HIF_TEMPLATE_METHOD(Alt);                                                                                 \
    template HIF_TEMPLATE_METHOD(Array);                                                                               \
    template HIF_TEMPLATE_METHOD(Assign);                                                                              \
    template HIF_TEMPLATE_METHOD(BaseContents);                                                                        \
    template HIF_TEMPLATE_METHOD(Bit);                                                                                 \
    template HIF_TEMPLATE_METHOD(BitValue);                                                                            \
    template HIF_TEMPLATE_METHOD(Bitvector);                                                                           \
    template HIF_TEMPLATE_METHOD(BitvectorValue);                                                                      \
    template HIF_TEMPLATE_METHOD(Bool);                                                                                \
    template HIF_TEMPLATE_METHOD(BoolValue);                                                                           \
    template HIF_TEMPLATE_METHOD(Break);                                                                               \
    template HIF_TEMPLATE_METHOD(Cast);                                                                                \
    template HIF_TEMPLATE_METHOD(Char);                                                                                \
    template HIF_TEMPLATE_METHOD(CharValue);                                                                           \
    template HIF_TEMPLATE_METHOD(CompositeType);                                                                       \
    template HIF_TEMPLATE_METHOD(Const);                                                                               \
    template HIF_TEMPLATE_METHOD(ConstValue);                                                                          \
    template HIF_TEMPLATE_METHOD(Contents);                                                                            \
    template HIF_TEMPLATE_METHOD(Continue);                                                                            \
    template HIF_TEMPLATE_METHOD(DataDeclaration);                                                                     \
    template HIF_TEMPLATE_METHOD(Declaration);                                                                         \
    template HIF_TEMPLATE_METHOD(DesignUnit);                                                                          \
    template HIF_TEMPLATE_METHOD(Entity);                                                                              \
    template HIF_TEMPLATE_METHOD(Enum);                                                                                \
    template HIF_TEMPLATE_METHOD(EnumValue);                                                                           \
    template HIF_TEMPLATE_METHOD(Event);                                                                               \
    template HIF_TEMPLATE_METHOD(Expression);                                                                          \
    template HIF_TEMPLATE_METHOD(Field);                                                                               \
    template HIF_TEMPLATE_METHOD(FieldReference);                                                                      \
    template HIF_TEMPLATE_METHOD(File);                                                                                \
    template HIF_TEMPLATE_METHOD(For);                                                                                 \
    template HIF_TEMPLATE_METHOD(ForGenerate);                                                                         \
    template HIF_TEMPLATE_METHOD(Function);                                                                            \
    template HIF_TEMPLATE_METHOD(FunctionCall);                                                                        \
    template HIF_TEMPLATE_METHOD(Generate);                                                                            \
    template HIF_TEMPLATE_METHOD(GlobalAction);                                                                        \
    template HIF_TEMPLATE_METHOD(Identifier);                                                                          \
    template HIF_TEMPLATE_METHOD(If);                                                                                  \
    template HIF_TEMPLATE_METHOD(IfAlt);                                                                               \
    template HIF_TEMPLATE_METHOD(IfGenerate);                                                                          \
    template HIF_TEMPLATE_METHOD(Instance);                                                                            \
    template HIF_TEMPLATE_METHOD(Int);                                                                                 \
    template HIF_TEMPLATE_METHOD(IntValue);                                                                            \
    template HIF_TEMPLATE_METHOD(Library);                                                                             \
    template HIF_TEMPLATE_METHOD(LibraryDef);                                                                          \
    template HIF_TEMPLATE_METHOD(Member);                                                                              \
    template HIF_TEMPLATE_METHOD(Null);                                                                                \
    template HIF_TEMPLATE_METHOD(Transition);                                                                          \
    template HIF_TEMPLATE_METHOD(PPAssign);                                                                            \
    template HIF_TEMPLATE_METHOD(Parameter);                                                                           \
    template HIF_TEMPLATE_METHOD(ParameterAssign);                                                                     \
    template HIF_TEMPLATE_METHOD(Pointer);                                                                             \
    template HIF_TEMPLATE_METHOD(Port);                                                                                \
    template HIF_TEMPLATE_METHOD(PortAssign);                                                                          \
    template HIF_TEMPLATE_METHOD(PrefixedReference);                                                                   \
    template HIF_TEMPLATE_METHOD(Procedure);                                                                           \
    template HIF_TEMPLATE_METHOD(ProcedureCall);                                                                       \
    template HIF_TEMPLATE_METHOD(Range);                                                                               \
    template HIF_TEMPLATE_METHOD(Real);                                                                                \
    template HIF_TEMPLATE_METHOD(RealValue);                                                                           \
    template HIF_TEMPLATE_METHOD(Record);                                                                              \
    template HIF_TEMPLATE_METHOD(RecordValue);                                                                         \
    template HIF_TEMPLATE_METHOD(RecordValueAlt);                                                                      \
    template HIF_TEMPLATE_METHOD(Reference);                                                                           \
    template HIF_TEMPLATE_METHOD(ReferencedAssign);                                                                    \
    template HIF_TEMPLATE_METHOD(ReferencedType);                                                                      \
    template HIF_TEMPLATE_METHOD(Return);                                                                              \
    template HIF_TEMPLATE_METHOD(Scope);                                                                               \
    template HIF_TEMPLATE_METHOD(ScopedType);                                                                          \
    template HIF_TEMPLATE_METHOD(Signal);                                                                              \
    template HIF_TEMPLATE_METHOD(Signed);                                                                              \
    template HIF_TEMPLATE_METHOD(SimpleType);                                                                          \
    template HIF_TEMPLATE_METHOD(Slice);                                                                               \
    template HIF_TEMPLATE_METHOD(State);                                                                               \
    template HIF_TEMPLATE_METHOD(StateTable);                                                                          \
    template HIF_TEMPLATE_METHOD(String);                                                                              \
    template HIF_TEMPLATE_METHOD(SubProgram);                                                                          \
    template HIF_TEMPLATE_METHOD(Switch);                                                                              \
    template HIF_TEMPLATE_METHOD(SwitchAlt);                                                                           \
    template HIF_TEMPLATE_METHOD(System);                                                                              \
    template HIF_TEMPLATE_METHOD(TPAssign);                                                                            \
    template HIF_TEMPLATE_METHOD(StringValue);                                                                         \
    template HIF_TEMPLATE_METHOD(Time);                                                                                \
    template HIF_TEMPLATE_METHOD(TimeValue);                                                                           \
    template HIF_TEMPLATE_METHOD(Type);                                                                                \
    template HIF_TEMPLATE_METHOD(TypeDeclaration);                                                                     \
    template HIF_TEMPLATE_METHOD(TypeDef);                                                                             \
    template HIF_TEMPLATE_METHOD(TypeReference);                                                                       \
    template HIF_TEMPLATE_METHOD(TypeTP);                                                                              \
    template HIF_TEMPLATE_METHOD(TypeTPAssign);                                                                        \
    template HIF_TEMPLATE_METHOD(TypedObject);                                                                         \
    template HIF_TEMPLATE_METHOD(Unsigned);                                                                            \
    template HIF_TEMPLATE_METHOD(Value);                                                                               \
    template HIF_TEMPLATE_METHOD(ValueStatement);                                                                      \
    template HIF_TEMPLATE_METHOD(ValueTP);                                                                             \
    template HIF_TEMPLATE_METHOD(ValueTPAssign);                                                                       \
    template HIF_TEMPLATE_METHOD(Variable);                                                                            \
    template HIF_TEMPLATE_METHOD(View);                                                                                \
    template HIF_TEMPLATE_METHOD(ViewReference);                                                                       \
    template HIF_TEMPLATE_METHOD(Wait);                                                                                \
    template HIF_TEMPLATE_METHOD(When);                                                                                \
    template HIF_TEMPLATE_METHOD(WhenAlt);                                                                             \
    template HIF_TEMPLATE_METHOD(While);                                                                               \
    template HIF_TEMPLATE_METHOD(With);                                                                                \
    template HIF_TEMPLATE_METHOD(WithAlt);

/// @brief Helper macro to instantiate a subset of symbol-related methods.
/// @details This macro is a specialized version of `HIF_INSTANTIATE_METHOD`,
/// targeting only symbol-related HIF objects, such as `FieldReference`, `Identifier`,
/// and `TypeReference`. Use this for methods that deal exclusively with HIF symbols.
#define HIF_INSTANTIATE_SYMBOLS()                                                                                      \
    template HIF_TEMPLATE_METHOD(FieldReference);                                                                      \
    template HIF_TEMPLATE_METHOD(FunctionCall);                                                                        \
    template HIF_TEMPLATE_METHOD(Identifier);                                                                          \
    template HIF_TEMPLATE_METHOD(Instance);                                                                            \
    template HIF_TEMPLATE_METHOD(Library);                                                                             \
    template HIF_TEMPLATE_METHOD(ParameterAssign);                                                                     \
    template HIF_TEMPLATE_METHOD(PortAssign);                                                                          \
    template HIF_TEMPLATE_METHOD(ProcedureCall);                                                                       \
    template HIF_TEMPLATE_METHOD(TypeTPAssign);                                                                        \
    template HIF_TEMPLATE_METHOD(TypeReference);                                                                       \
    template HIF_TEMPLATE_METHOD(ValueTPAssign);                                                                       \
    template HIF_TEMPLATE_METHOD(ViewReference);
