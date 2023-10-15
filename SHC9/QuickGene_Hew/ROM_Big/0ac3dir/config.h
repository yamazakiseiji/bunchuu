/******************************************************************************
*                                                             \  ___  /       *
*                                                               /   \         *
* Edison Design Group C++  Runtime                           - | \^/ | -      *
*                                                               \   /         *
* Proprietary information of Edison Design Group Inc.         /  | |  \       *
* Copyright 1992 Edison Design Group Inc.                        [_]          *
*                                                                             *
******************************************************************************/
/*

Configuration parameters for the runtime.

*/

#ifndef CONFIG_H
#define CONFIG_H 1

/*
Flag used to retain ABI (Application Binary Interface, i.e., runtime layout
and calling sequence) compatibility with older versions.  The value is the
version number of the EDG C++ front end, e.g., 227 for version 2.27, for
which compatibility should be maintained.  ABI changes made after that
version will be suppressed.  Of course, that may suppress certain language
features that cannot be implemented without the corresponding ABI changes.
The default is 228, which results in the ABI being compatible with earlier
versions of the front end.

Beginning with version 2.29, the front end defines a preprocessing symbol
called __EDG_ABI_COMPATIBILITY_VERSION that defines the ABI level
being used by the front end.  This value is used, if it is defined.
*/
#ifndef ABI_COMPATIBILITY_VERSION
#ifdef __EDG_ABI_COMPATIBILITY_VERSION
#define ABI_COMPATIBILITY_VERSION __EDG_ABI_COMPATIBILITY_VERSION
#else /* ifndef __EDG_ABI_COMPATIBILITY_VERSION */
#define ABI_COMPATIBILITY_VERSION 228
#endif /* ifdef __EDG_ABI_COMPATIBILITY_VERSION */
#endif /* ifndef ABI_COMPATIBILITY_VERSION */

/*
This flag indicates that the runtime system may be called by cfront-generated
code, and consequently that it must behave as expected by the cfront code.
For example, cfront passes eight NULL pointers to constructors called
from vec_new.  The NULL pointers initialize parameters that point to
virtual base classes.  We only do this in cfront compatibility mode.
*/
#ifndef CFRONT_COMPATIBILITY_MODE
#define CFRONT_COMPATIBILITY_MODE TRUE
#endif /* ifndef CFRONT_COMPATIBILITY_MODE */


/*
The runtime uses one of several different mechanisms to invoke static
destructors upon completion of the program.  On Suns, on_exit is used.
For ANSI C environments, atexit is used.  Otherwise a version of
exit is supplied in our runtime library to replace the one normally
found in the C library (e.g., libc.a).  Using our exit may affect
usage that requires an alternate version of the exit function, such
as generating profiling information.

USE_ATEXIT indicates that the atexit function should be used.
*/
#ifndef USE_ATEXIT
#if defined(sun) && __EDG_BSD
#define USE_ATEXIT FALSE
#else /* !(defined(sun) && __EDG_BSD) */
#define USE_ATEXIT TRUE
#endif /* !(defined(sun) && __EDG_BSD) */
#endif /* ifndef USE_ATEXIT */

/*
Should the components of the runtime system that implement
exception handling be included.
*/
#ifndef EXCEPTION_HANDLING
#define EXCEPTION_HANDLING TRUE
#endif /* ifndef EXCEPTION_HANDLING */


/*
Should the components of the runtime system that implement run-time
type identification be included.  Note that enabling ABI_CHANGES_FOR_RTTI
alters the structure of the a_type_info_impl that is shared by
ABI_CHANGES_FOR_RTTI and exception handling, consequently
ABI_CHANGES_FOR_RTTI cannot be enabled when preserving ABI compatibility
with versions up to 2.28.  When the --building_runtime option is specified,
the front end defines __EDG_ABI_CHANGES_FOR_RTTI as 0 or 1 to indicate
how it is configured.
*/
#ifndef ABI_CHANGES_FOR_RTTI
#ifdef __EDG_ABI_CHANGES_FOR_RTTI
#define ABI_CHANGES_FOR_RTTI __EDG_ABI_CHANGES_FOR_RTTI
#else /* !defined(__EDG_ABI_CHANGES_FOR_RTTI) */
#if ABI_COMPATIBILITY_VERSION <= 228
#define ABI_CHANGES_FOR_RTTI FALSE /* Versions up to 2.28. */
#else /* ABI_COMPATIBILITY_VERSION > 228 */
#define ABI_CHANGES_FOR_RTTI TRUE  /* Versions after 2.28. */
#endif /* ABI_COMPATIBILITY_VERSION <= 228 */
#endif /* ifdef __EDG_ABI_CHANGES_FOR_RTTI */
#endif /* ifndef ABI_CHANGES_FOR_RTTI */
#if ABI_CHANGES_FOR_RTTI && (ABI_COMPATIBILITY_VERSION <= 228)
 #error -- ABI_CHANGES_FOR_RTTI TRUE is incompatible with \
           ABI_COMPATIBILITY_VERSION <= 228
#endif /* ABI_CHANGES_FOR_RTTI && (ABI_COMPATIBILITY_VERSION <= 228) */

/*
This switch controls whether or not the ABI changes for array
new and delete are done.  New runtime routines are added.
The changes are upward-compatible (you can use old object code
with new object code and the new library).  If the switch is off,
compatibility with versions up to 2.28 is preserved, but the
array new and delete language features are turned off.
When the --building_runtime option is specified, the front end
defines __EDG_ABI_CHANGES_FOR_ARRAY_NEW_AND_DELETE as 0 or 1 to
indicate how it is configured.
*/
#ifndef ABI_CHANGES_FOR_ARRAY_NEW_AND_DELETE
#ifdef __EDG_ABI_CHANGES_FOR_ARRAY_NEW_AND_DELETE
#define ABI_CHANGES_FOR_ARRAY_NEW_AND_DELETE \
				__EDG_ABI_CHANGES_FOR_ARRAY_NEW_AND_DELETE
#else /* !defined(__EDG_ABI_CHANGES_FOR_ARRAY_NEW_AND_DELETE) */
#if ABI_COMPATIBILITY_VERSION <= 228
#define ABI_CHANGES_FOR_ARRAY_NEW_AND_DELETE FALSE /* Versions up to 2.28. */
#else /* ABI_COMPATIBILITY_VERSION > 228 */
#define ABI_CHANGES_FOR_ARRAY_NEW_AND_DELETE TRUE  /* Versions after 2.28. */
#endif /* ABI_COMPATIBILITY_VERSION <= 228 */
#endif /* ifdef __EDG_ABI_CHANGES_FOR_ARRAY_NEW_AND_DELETE */
#endif /* ifndef ABI_CHANGES_FOR_ARRAY_NEW_AND_DELETE */
#if ABI_CHANGES_FOR_ARRAY_NEW_AND_DELETE && (ABI_COMPATIBILITY_VERSION <= 228)
 #error -- ABI_CHANGES_FOR_ARRAY_NEW_AND_DELETE TRUE is incompatible with \
           ABI_COMPATIBILITY_VERSION <= 228
#endif /* ABI_CHANGES_FOR_ARRAY_NEW_AND_DELETE && ... */


/*
This switch controls whether or not the ABI changes for placement
delete are done.  New runtime routines/variables are added.
The changes are upward-compatible (you can use old object code
with new object code and the new library).  If the switch is off,
compatibility with versions up to 2.33 is preserved, but the
placement delete language feature is turned off.  Allocating an
array with placement new and then using the delete operator on it
is also considered part of "placement delete" and is controlled by
this switch.
*/
#ifndef ABI_CHANGES_FOR_PLACEMENT_DELETE
/* If this value is not defined, use the value defined by the front end,
   if one is provided.  Otherwise, assume that the feature is not supported. */
#ifdef __EDG_ABI_CHANGES_FOR_PLACEMENT_DELETE
#define ABI_CHANGES_FOR_PLACEMENT_DELETE __EDG_ABI_CHANGES_FOR_PLACEMENT_DELETE
#else /* ifndef __EDG_ABI_CHANGES_FOR_PLACEMENT_DELETE */
#define ABI_CHANGES_FOR_PLACEMENT_DELETE FALSE
#endif /* ifndef __EDG_ABI_CHANGES_FOR_PLACEMENT_DELETE */
#endif /* ifndef ABI_CHANGES_FOR_PLACEMENT_DELETE */

/*
This switch controls how the runtime keeps track of information about the
size of arrays that have been allocated.  Through version 2.28 this
information was maintained using a separate data structure allocated
by vec_new.  Beginning with 2.29 this information is maintained in
header block that is part of the array memory allocated.  The new
mechanism can be used with older ABI versions.   The only consequences
of this are that the size of the memory blocks requested from operator
new will change because they now include the size of the prefix information,
and the behavior of programs that perform undefined operations (e.g.,
writing to memory that preceded the beginning of the array) may change.
*/
#ifndef USE_PREFIX_FOR_ARRAY_ALLOC_INFO
#if ABI_COMPATIBILITY_VERSION <= 228
#define USE_PREFIX_FOR_ARRAY_ALLOC_INFO FALSE /* Versions up to 2.28. */
#else /* ABI_COMPATIBILITY_VERSION > 228 */
#define USE_PREFIX_FOR_ARRAY_ALLOC_INFO TRUE  /* Versions after 2.28. */
#endif /* ABI_COMPATIBILITY_VERSION <= 228 */
#endif /* ifndef USE_PREFIX_FOR_ARRAY_ALLOC_INFO */


#if EXCEPTION_HANDLING
/*
The EH runtime allocates a static block of memory to be used for purposes
of tracking pending exceptions, making a copy of the thrown object, etc.
Additional space is allocated if needed.  This parameter specifies the
size of the initial block of memory allocated and the minimum size of
any additional blocks that are required.
*/
#ifndef EH_MEMORY_ALLOCATION_INCREMENT
#define EH_MEMORY_ALLOCATION_INCREMENT 8192
#endif /* ifndef EH_MEMORY_ALLOCATION_INCREMENT */

/*
A type that, when used, will be aligned with the strictest alignment
requirements.
*/
#ifndef TYPE_WITH_MOST_STRICT_ALIGNMENT
#define TYPE_WITH_MOST_STRICT_ALIGNMENT double
#endif /* ifndef TYPE_WITH_MOST_STRICT_ALIGNMENT */

#if ABI_CHANGES_FOR_RTTI
/*
The mangled name of the unique ID for a void type.
*/
#ifndef MANGLED_NAME_OF_UNIQUE_ID_OF_VOID
#define MANGLED_NAME_OF_UNIQUE_ID_OF_VOID __TID_v
#endif /* ifndef MANGLED_NAME_OF_UNIQUE_ID_OF_VOID */
#else /* ! ABI_CHANGES_FOR_RTTI */
/*
The mangled name of the typeinfo record for a void type.
*/
#ifndef MANGLED_NAME_OF_VOID
#define MANGLED_NAME_OF_VOID __T_v
#endif /* ifndef MANGLED_NAME_OF_VOID */
#endif /* ! ABI_CHANGES_FOR_RTTI */

/*
Flag that is TRUE if the definition of jmp_buf from the setjmp.h header
file should be used instead of the element type and array size passed
by the front end.  This is useful on systems where the setjmp buffer
is a structure instead of an array.
*/
#ifndef USE_SYSTEM_JMP_BUF_DEFINITION
#if defined(__linux__) || defined(__hpux)
#define USE_SYSTEM_JMP_BUF_DEFINITION TRUE
#else /* if !(defined(__linux__) || defined(__hpux)) */
#define USE_SYSTEM_JMP_BUF_DEFINITION FALSE
#endif /* if defined(__linux__) || defined(__hpux) */
#endif /* ifndef USE_SYSTEM_JMP_BUF_DEFINITION */

/*
Flag that is TRUE if the stack should be unwound before terminate()
is called when an exception with no matching handler is thrown.
According to the Working Paper, it is unspecified whether or not the
stack is unwound in this case.
*/
#ifndef UNWIND_STACK_BEFORE_CALLING_TERMINATE
#define UNWIND_STACK_BEFORE_CALLING_TERMINATE TRUE
#endif /* ifndef UNWIND_STACK_BEFORE_CALLING_TERMINATE */

#endif /* EXCEPTION_HANDLING */

/*
Define macros that are used to define exception specifications.  The
macros are used so that the throw specifications can be discarded when
exception handling is not being used.
*/
#if EXCEPTION_HANDLING
#define THROW(throw_spec) throw(throw_spec)
#define THROW_NOTHING() throw()
#else /* !EXCEPTION_HANDLING */
#define THROW(throw_spec) /* Nothing. */
#define THROW_NOTHING() /* Nothing. */
#endif /* EXCEPTION_HANDLING */

/*
The strictest alignment required of any data type.  This should be
the alignment that malloc uses for memory that is allocated.
*/
#ifndef MOST_STRICT_ALIGNMENT
#define MOST_STRICT_ALIGNMENT 8
#endif /* ifndef MOST_STRICT_ALIGNMENT */

/*
Flag that is TRUE if a message containing the reason for a runtime
abort should be displayed.
*/
#ifndef DISPLAY_ABORT_DESCRIPTION
#define DISPLAY_ABORT_DESCRIPTION TRUE
#endif /* ifndef DISPLAY_ABORT_DESCRIPTION */

#endif /* CONFIG_H */


/******************************************************************************
*                                                             \  ___  /       *
*                                                               /   \         *
* Edison Design Group C++  Runtime                           - | \^/ | -      *
*                                                               \   /         *
* Proprietary information of Edison Design Group Inc.         /  | |  \       *
* Copyright 1992 Edison Design Group Inc.                        [_]          *
*                                                                             *
******************************************************************************/
